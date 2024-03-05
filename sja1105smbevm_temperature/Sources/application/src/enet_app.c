/*
 * Copyright 2017-2018 NXP
 * All rights reserved.
 *
 * THIS SOFTWARE IS PROVIDED BY NXP "AS IS" AND ANY EXPRESSED OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL NXP OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "FreeRTOS.h"
#include "spi_sw.h"
#include "uart.h"
#include "enet_sw.h"
#include "enet_dbg.h"
#include "phy_cfg.h"

#include "sja1105smbevm_console.h"
#include "sja1105smbevm_gpio.h"
#include "sja1105smbevm_ethsetup.h"
#include "app.h"
#include "enet_callbacks.h"
#include "enet_app.h"

#include "task.h"

#include "NXP_SJA1105P_switchCore.h"
#include "NXP_SJA1105P_config.h"
#include "NXP_SJA1105P_spi.h"

#include "NXP_SJA1105P_diagnostics.h"

/* packet send demo: if set: periodically send out packets on both interfaces */
#define PACKET_SEND_DEMO 0
/* enables a workaround which is needed when older phy is used (TJA110xA) */
#define WORKAROUND_TJA110xA 1
/* enables additional debug information via uart */
#define DEBUG_MSG 1
/* Delay in ms for polling */
#define PHYLOOP_POLL_DELAY 10U

/* ==== Ethernet section ================================================= */
/* local data structures for management */
static SemaphoreHandle_t switchMutex;
static volatile status_t sw_dspi_st;
static TaskHandle_t phyPollTaskHandle;

/* SJA1105x SPI frame format */
#define SWITCH_MAX_DATA_WORDS 64
#define SWITCH_READ_WRITE_SHIFT 31U
#define SWITCH_WORD_COUNT_SHIFT 25U
#define SWITCH_WORD_COUNT_MASK 0x7E000000UL
#define SWITCH_ADDR_SHIFT 4U
#define SWITCH_ADDR_MASK 0x01FFFFF0UL
#define SWITCH_BUFFER_CONTENTS_READ 0xcccc5555
#define EXTENDED_CTRL_LINK_CTRL_FLAG 0x8000U
#define CONF1_MASTER_SLAVE_FLAG 0x8000U

static uint8_t
switchspi_transfer32(bool isWriteTransaction, uint8_t deviceSelect, uint8_t regCount, uint32_t registerAddress, uint32_t *p_registerValue)
{
  status_t res;
  uint8_t ret = 0;
  uint32_t txBuffer[SWITCH_MAX_DATA_WORDS + 1];
  uint32_t rxBuffer[SWITCH_MAX_DATA_WORDS + 1];
  int i;

  if (regCount > SWITCH_MAX_DATA_WORDS)
  {
    ret = 5;
  }
  else if (p_registerValue == NULL)
  {
    ret = 6;
  }

  if (ret)
    return ret;

  /* prepare buffer */
  txBuffer[0] = (((uint32_t)(isWriteTransaction ? 1U : 0U)) << SWITCH_READ_WRITE_SHIFT);
  txBuffer[0] |= (SWITCH_WORD_COUNT_MASK & ((uint32_t)((uint32_t)regCount) << SWITCH_WORD_COUNT_SHIFT));
  txBuffer[0] |= (SWITCH_ADDR_MASK & ((uint32_t)((uint32_t)registerAddress) << SWITCH_ADDR_SHIFT));

  for (i = 0; i < regCount; i++)
  {
    if (isWriteTransaction)
    {
      txBuffer[i + 1] = p_registerValue[i];
    }
    else
    {
      txBuffer[i + 1] = SWITCH_BUFFER_CONTENTS_READ;
    }
  }

  /* take semaphore for the bus */
  if (switchMutex != NULL)
  {
    /*
     * See if we can obtain the semaphore.  If the semaphore is not
     * available wait 10 ticks to see if it becomes free.
     */
    if (xSemaphoreTake(switchMutex, (TickType_t)10) == pdTRUE)
    {

      /* got the semaphore */

      /* set CS */
      DSPI_UpdateCS(INST_SPI_SW, deviceSelect);

      /* call transaction blocking */
      res = DSPI_MasterTransferBlocking(INST_SPI_SW, txBuffer, rxBuffer, regCount + 1, 100);

      xSemaphoreGive(switchMutex);
    }
    else
    {
      res = STATUS_TIMEOUT;
    }
  }
  else
  {
    res = STATUS_ERROR;
  }

  if (res == STATUS_SUCCESS)
  {

    if (!isWriteTransaction)
    {
      for (i = 0; i < regCount; i++)
      {
        p_registerValue[i] = rxBuffer[i + 1];
      }
    }

    ret = 0;
  }
  else
  {
    switch (res)
    {
    case STATUS_SPI_ABORTED:
      ret = 1;
      break;
    case STATUS_SPI_TX_UNDERRUN:
      ret = 2;
      break;
    case STATUS_SPI_RX_OVERRUN:
      ret = 3;
      break;
    case STATUS_TIMEOUT:
      ret = 4;
      break;
    case STATUS_ERROR:
      ret = 5;
      break;
    default:
      ret = 10;
    }
  }
  return ret;
}

/* these must be separate functions leading to the same worker function, because the CallBacks need a function pointer */
static uint8_t
switchspi_Read32(uint8_t deviceSelect, uint8_t regCount, uint32_t registerAddress, uint32_t *p_registerValue)
{
  return switchspi_transfer32(false, deviceSelect, regCount, registerAddress, p_registerValue);
}

static uint8_t
switchspi_Write32(uint8_t deviceSelect, uint8_t regCount, uint32_t registerAddress, uint32_t *p_registerValue)
{
  return switchspi_transfer32(true, deviceSelect, regCount, registerAddress, p_registerValue);
}

typedef enum
{
  TC10_WAKE_IGNORE,
  TC10_WAKE_PHY_ONLY,
  TC10_WAKE_PHY_AND_FORWARD
} TC10_WAKE_SETUP;

typedef struct
{
  uint8_t phy_config_idx;
  TC10_WAKE_SETUP wakeinoutline; /* how to deal with wakeup from WAKE_IN_OUT */
  TC10_WAKE_SETUP remote;        /* how to deal with wakeup from link peer */
} TC10_ALLOWANCE;

static TC10_ALLOWANCE const *tc10_allowance_matrix;
static uint8_t tc10_allowance_matrix_len;

static const TC10_ALLOWANCE tc10_allowance_matrix_setup_1[] = {
    /*     conf_idx  wakeinoutline             remote     */
    /* A1 */ {3U, TC10_WAKE_PHY_AND_FORWARD, TC10_WAKE_PHY_AND_FORWARD},
    /* B0 */ {4U, TC10_WAKE_PHY_AND_FORWARD, TC10_WAKE_PHY_AND_FORWARD},
    /* B1 */ {5U, TC10_WAKE_PHY_AND_FORWARD, TC10_WAKE_PHY_AND_FORWARD},
    /* B2 */ {6U, TC10_WAKE_PHY_AND_FORWARD, TC10_WAKE_PHY_AND_FORWARD},
    /* B3 */ {7U, TC10_WAKE_PHY_AND_FORWARD, TC10_WAKE_PHY_AND_FORWARD},
};
static const uint8_t tc10_allowance_matrix_setup_1_len = sizeof(tc10_allowance_matrix_setup_1) / sizeof(*tc10_allowance_matrix_setup_1);
static const TC10_ALLOWANCE tc10_allowance_matrix_setup_2[] = {
    /*     conf_idx  wakeinoutline             remote     */
    /* A1 */ {3U, TC10_WAKE_PHY_AND_FORWARD, TC10_WAKE_PHY_AND_FORWARD},
    /* B0 */ {4U, TC10_WAKE_PHY_AND_FORWARD, TC10_WAKE_PHY_ONLY},
    /* B1 */ {5U, TC10_WAKE_PHY_AND_FORWARD, TC10_WAKE_PHY_AND_FORWARD},
    /* B2 */ {6U, TC10_WAKE_PHY_AND_FORWARD, TC10_WAKE_IGNORE},
    /* B3 */ {7U, TC10_WAKE_PHY_ONLY, TC10_WAKE_PHY_AND_FORWARD},
};
static const uint8_t tc10_allowance_matrix_setup_2_len = sizeof(tc10_allowance_matrix_setup_2) / sizeof(*tc10_allowance_matrix_setup_2);
static const uint8_t tc10_allowance_matrix_cnt = 2;

/* this is from the device driver */
#define BASIC_CTRL_ADDR 0U
#define EXTENDED_CTRL_ADDR 17U
#define CONFIG1_ADDR 18U
#define IRQ_STATUS_ADDR 21U
#define IRQ_ENABLE_ADDR 22U
#define COMM_STATUS_ADDR 23U
#define GENERAL_STATUS_ADDR 24U
#define COMMON_CONFIG_ADDR 27U
#define BASIC_CTRL_LOOPBACK_FLAG 0x4000U

#define EXTENDED_CTRL_LINK_CTRL_FLAG 0x8000U
#define EXTENDED_CTRL_LOOPBACK_MODE_MASK 0x0018U
#define EXTENDED_CTRL_LOOPBACK_MODE_INTERNAL 0x0000U
#define EXTENDED_CTRL_LOOPBACK_MODE_EXTERNAL 0x0008U
#define EXTENDED_CTRL_LOOPBACK_MODE_REMOTE 0x0018U
#define EXTENDED_CTRL_POWER_MODE_MASK 0x7800U
#define EXTENDED_CTRL_POWER_MODE_SLEEP_REQ 0x5800U
#define EXTENDED_CTRL_POWER_MODE_NORMAL_REQ 0x1800U
#define EXTENDED_CTRL_POWER_MODE_STANDBY 0x6000U
#define EXTENDED_CTRL_POWER_MODE_SLEEP 0x5000U
#define EXTENDED_CTRL_WAKE_REQ_FLAG 0x0001U
#define EXTENDED_CTRL_CONFIG_EN_FLAG 0x0004U
#define COMM_STATUS_LINK_UP 0x8000U
#define GENERAL_STATUS_LOCAL_WU 0x2000U

static uint16_t fixup_wake_timeout[] = {
    /* A1 */ 0U,
    /* B0 */ 0U,
    /* B1 */ 0U,
    /* B2 */ 0U,
    /* B3 */ 0U,
};
static const size_t fixup_wake_timeout_len = sizeof(fixup_wake_timeout) / sizeof(*fixup_wake_timeout);

#define LINKUP_DELAY_MAX 120U /* msec for timeout */
#define MAXLOOPS ((LINKUP_DELAY_MAX / PHYLOOP_POLL_DELAY) + 1)

#if WORKAROUND_TJA110xA
/* idx is the phy idx for driver calls */
static void
sja11xx_fixup_wake(uint8_t idx)
{
  uint8_t phyidx; /* PHY driver device number */
  uint16_t commStat;
  uint16_t extCtrl;
  uint16_t genStat;
  uint16_t timer;
  status_t phyStatus;

  if ((idx >= fixup_wake_timeout_len) ||
      (idx >= tc10_allowance_matrix_len))
  {
    return;
  }

  phyidx = tc10_allowance_matrix[idx].phy_config_idx;

  extCtrl = 0xaaaa;
  commStat = 0x5555;
  genStat = 0xcccc;

  /* check if wakeup is hanging */
  if (((phyStatus = PHY_Read(phyidx, EXTENDED_CTRL_ADDR, &extCtrl)) == STATUS_SUCCESS) &&
      ((phyStatus = PHY_Read(phyidx, COMM_STATUS_ADDR, &commStat)) == STATUS_SUCCESS) &&
      ((phyStatus = PHY_Read(phyidx, GENERAL_STATUS_ADDR, &genStat)) == STATUS_SUCCESS))
  {

    timer = fixup_wake_timeout[idx];
    /*    ((commStat & COMM_STATUS_LINK_UP) == 0))  */
    if ((timer == 0) &&
        ((extCtrl & EXTENDED_CTRL_WAKE_REQ_FLAG) != 0) &&
        ((extCtrl & EXTENDED_CTRL_LINK_CTRL_FLAG) != 0))
    {

      /* found a potential "hang" situation - type1 */
      /* WAKE_REQ and LINK_CTRL are set */

      timer++;
      fixup_wake_timeout[idx] = timer;
    }
    else if ((timer == 0) &&
             ((genStat & GENERAL_STATUS_LOCAL_WU) != 0))
    {

      /* found a potential "hang" situation - type2 */
      /* LOCAL_WU is set, but link is not up */

      timer++;
      fixup_wake_timeout[idx] = timer;
    }
    else if ((timer != 0) &&
             ((commStat & COMM_STATUS_LINK_UP) == 0))
    {

      /* link still not up - on timeout: do the workaround */

      timer++;
      fixup_wake_timeout[idx] = timer;

      if (timer >= MAXLOOPS)
      {
        switch (timer)
        {
        case (MAXLOOPS + 0):
          /* step 1: clear link control */
          phyStatus = PHY_RMW(phyidx, EXTENDED_CTRL_ADDR,
                              0,
                              EXTENDED_CTRL_LINK_CTRL_FLAG | EXTENDED_CTRL_WAKE_REQ_FLAG);
          break;
        case (MAXLOOPS + 1):
          /* step 2: send wake request again */
          phyStatus = PHY_RMW(phyidx, EXTENDED_CTRL_ADDR,
                              EXTENDED_CTRL_WAKE_REQ_FLAG,
                              EXTENDED_CTRL_WAKE_REQ_FLAG);
          break;
        case (MAXLOOPS + 2):
          /* step 3: set link request again  */
          phyStatus = PHY_RMW(phyidx, EXTENDED_CTRL_ADDR,
                              EXTENDED_CTRL_LINK_CTRL_FLAG,
                              EXTENDED_CTRL_LINK_CTRL_FLAG);
          break;
        case (MAXLOOPS + 3):
          /* step 4: reset the state machine by starting from 0 */
          fixup_wake_timeout[idx] = 0;
          break;
        default:;
        }
      }
    }
    else if (timer != 0)
    {
      fixup_wake_timeout[idx] = 0;
    }
  }
}
#endif

static void
genericPhypollTask(void *pvParameters)
{
  int i;
  status_t stat;
  SJA1105SMBEVM_ENETTASK_DEFS *enetTaskDefsPtr = (SJA1105SMBEVM_ENETTASK_DEFS *)pvParameters;
  uint32_t loopcnt = 0;
  uint8_t phy; /* PHY driver device number */

  /* wait for PhypollTask to start up until main task has finished startup blink pattern */
  vTaskDelay(DELAY_MS(1300));

#if DEBUG_MSG
  console_puts("genericPhypollTask started\r\n");
#endif

  for (;;)
  {
    vTaskDelay(DELAY_MS(PHYLOOP_POLL_DELAY));

    for (i = 0; i < N_PHY_DEVS; i++)
    {
      stat = PHY_MainFunction(i);
      if (stat != STATUS_SUCCESS)
      {
#if DEBUG_MSG
        console_puts("MainFunction status ");
        console_putint(stat, 5);
        console_puts(" idx ");
        console_putint(i, 5);
        console_puts("\n\r");
#endif
      }
    }

#if WORKAROUND_TJA110xA
    /* this is a fix for the PHY HW bug according to errata #xxxyyy */
    for (i = 0; i < N_PHY_DEVS; i++)
    {
      sja11xx_fixup_wake(i);
    }
#endif /* WORKAROUND_TJA110xA */

    if (enetTaskDefsPtr->enettask_featureFlags & ENETTASK_FEATURE_TC10)
    {
      int i;
      bool allActivePHYasleep = true;
      uint16_t reg;

      for (i = 0; i < tc10_allowance_matrix_len; i++)
      {
        phy = tc10_allowance_matrix[i].phy_config_idx;
        PHY_Read(phy, 23, &reg);
        if (reg & 0x8000)
        {
          allActivePHYasleep = false;
        }

        PHY_Read(phy, 17, &reg);
        switch ((reg >> 11) & 0xf)
        {
        case 0xc:
        case 0xa:
          GPIO_SETLED(phy_callbacks_getLedByPhyCfgIdx(phy), false);
          break;
        }
      }
      GPIO_SETLED(LED_GP, allActivePHYasleep);
    }
    loopcnt++;
  }
}

static const uint8_t destAddr[] = {0x08, 0x00, 0x27, 0x02, 0x12, 0x88};
static const uint8_t bcAddr[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const uint16_t type = 0x3456;
volatile status_t stat;
#ifdef PACKET_SEND_DEMO
static uint8_t transmit_buffer[128]; /* outside to relieve task stack memory */
#endif
static void
errorloop_forever(const char *message, uint32_t led)
{
#if DEBUG_MSG
  console_puts(message);
  console_puts("  - execution stopped\n\r");
#endif

  for (;;)
  {
    GPIO_SETLED(led, true);
    vTaskDelay(DELAY_MS(200));
    GPIO_SETLED(led, false);
    vTaskDelay(DELAY_MS(200));
  }
}

static void
enetResetEnableSwitchAndPHYs(void)
{
  /* assert reset to all non-BroadRReach PHYs */
  /* do not touch the BroadRReach PHYs to not disturb any setting during wake-up */
  GPIO_SETPRODUCTRESET(RESET_GIG_PHY_1, true);
  GPIO_SETPRODUCTRESET(RESET_GIG_PHY_2, true);
  GPIO_SETPRODUCTRESET(RESET_FE_PHY, true); /* does not work due to connection to HW config */

  /* pull reset for the switches */
  GPIO_SETPRODUCTRESET(RESET_PRI_SJA1105, true);
  GPIO_SETPRODUCTRESET(RESET_SEC_SJA1105, true);

  vTaskDelay(DELAY_MS(2)); /* honor minimum reset pulse time - plenty of surplus time */

  GPIO_SETPRODUCTRESET(RESET_GIG_PHY_1, false);
  GPIO_SETPRODUCTRESET(RESET_GIG_PHY_2, false);
  GPIO_SETPRODUCTRESET(RESET_FE_PHY, false); /* does not work */

  GPIO_SETPRODUCTRESET(RESET_PRI_SJA1105, false);
  GPIO_SETPRODUCTRESET(RESET_SEC_SJA1105, false);

  vTaskDelay(DELAY_MS(8)); /* make sure reset line has enough time to creep up to full voltage */

  /* enable the PHYs */
  GPIO_SETPHYENABLES(ENAB_TJAS_PHY_3, true);
  GPIO_SETPHYENABLES(ENAB_TJA_PHY_D1, true);
  GPIO_SETPHYENABLES(ENAB_TJA_PHY_D2, true);

  vTaskDelay(DELAY_MS(8));
}

#define SJA1105S_ID (0xAE00030e)
static int
enetConfigureSwitches(uint8_t configId)
{
  int ictr, octr;
  uint32_t deviceID0, deviceID1;
  unsigned int errorcnt = 0;

  /* ====== setup switches =========== */
  /* attempt to access both switches - give some time in case chip is not yet ready */
  octr = 5;
  do
  {
    ictr = 10;
    do
    {
      if (((SJA1105P_getDeviceId(&deviceID0, 0)) == 0) &&
          ((SJA1105P_getDeviceId(&deviceID1, 1)) == 0))
      {
        break;
      }

      vTaskDelay(DELAY_MS(8));
    } while (--ictr > 0);

    if ((deviceID0 == SJA1105S_ID) && (deviceID1 == SJA1105S_ID))
    {
      break;
    }
  } while (--octr > 0);

  if (octr <= 0)
  {
    errorcnt++;
  }
  else if (SJA1105P_initSwitch(configId) > 0)
  {
    errorcnt++;
  }

  return errorcnt;
}

#if WORKAROUND_TJA110xA
static inline void handleReg(uint8_t phy, uint8_t addr, const char *name)
{
  uint16_t reg;

  PHY_Read(phy, addr, &reg);
  console_puts("  ");
  console_puts(name);
  console_puts("  ");
  console_puthex(reg);
}
#endif

void enet_diag_button_handler(bool newState)
{
#if DEBUG_MSG
  int i;
  uint16_t conf1;
  TC10_ALLOWANCE const *t_ptr;
  if (!newState)
  {
    console_puts("phy diag:\n\r");
    for (i = 0; i < tc10_allowance_matrix_len; i++)
    {
      t_ptr = &tc10_allowance_matrix[i];
      console_puts("phy ");
      console_putint(t_ptr->phy_config_idx, 3);

      // phy master or slave
      PHY_Read(t_ptr->phy_config_idx, 18, &conf1);
      if ((conf1 & 0x8000) != 0U)
        console_puts(" master\t");
      else
        console_puts(" slave\t");

      // read register from phy
      handleReg(t_ptr->phy_config_idx, 17, "ext_ctrl_reg");
      handleReg(t_ptr->phy_config_idx, 23, "comm_stat_reg");
      handleReg(t_ptr->phy_config_idx, 18, "config1_reg");
      handleReg(t_ptr->phy_config_idx, 27, "common_config_reg");

      console_puts("\n\r");
    }
    SJA1105P_tempThreshold_t a_temperature[SJA1105P_N_SWITCHES];
    SJA1105P_getTemperature(a_temperature);
    console_puts("sw0 temperature:\n\r");
    console_putint(a_temperature[0], 2);
    console_puts("\n\r");
    console_puts("sw1 temperature:\n\r");
    console_putint(a_temperature[1], 2);
    console_puts("\n\r");
  }

#endif
}

void enet_sleep_button_handler(bool newState)
{
  int i;
  static uint32_t errorcnt;

  if (!newState)
  {
    /* react on RELEASE of button */

    /* since generic PHYs don't support sleep, roll over them */
    for (i = 0; i < tc10_allowance_matrix_len; i++)
    {
      if ((stat = PHY_Sleep(tc10_allowance_matrix[i].phy_config_idx)) != STATUS_SUCCESS)
      {
        errorcnt++;
      }
    }
#if DEBUG_MSG
    console_puts("sleep requested; err count ");
    console_putint(errorcnt, 4);
    console_puts("\n\r");
#endif
  }
}

void enet_task(void *parameters)
{
  BaseType_t freertos_stat;
  status_t stat;
  int i;
  unsigned int errorcnt = 0;
  SJA1105SMBEVM_ENETTASK_DEFS *enetTaskDefsPtr = (SJA1105SMBEVM_ENETTASK_DEFS *)parameters;
  uint16_t extCtrl;
  uint16_t conf1;

  /* according to the rotary setting, use one or the other tc10_allowance matrix */
  switch (enetTaskDefsPtr->enettask_tc10_setupNumber)
  {
  case 1:
    tc10_allowance_matrix = tc10_allowance_matrix_setup_2;
    tc10_allowance_matrix_len = tc10_allowance_matrix_setup_2_len;
    break;
  case 0:
  default:
    tc10_allowance_matrix = tc10_allowance_matrix_setup_1;
    tc10_allowance_matrix_len = tc10_allowance_matrix_setup_1_len;
  }

  /* switch SPI bus init */
  sw_dspi_st = DSPI_MasterInit(INST_SPI_SW, &spi_swState, &spi_sw_0);

  /* register the switch SPI callbacks */
  SJA1105P_registerSpiRead32CB(switchspi_Read32);
  SJA1105P_registerSpiWrite32CB(switchspi_Write32);

  switchMutex = xSemaphoreCreateMutex(); /* needed for protecting SPI bus from simultaneous accesses from different tasks */

  /* ===== init processor's ethernet interfaces - must be done before SMI is used */

  /* initialize ethernet driver for 100MBit interface to switch and to the 100MBit debug port */
  ENET_DRV_Init(INST_ENET_SW, &enet_sw_State, &enet_sw_InitConfig0, enet_sw_buffConfigArr0, enet_sw_MacAddr);
  ENET_DRV_Init(INST_ENET_DBG, &enet_dbg_State, &enet_dbg_InitConfig0, enet_dbg_buffConfigArr0, enet_dbg_MacAddr);
  ENET_DRV_EnableMDIO(INST_ENET_SW, false);

  enetResetEnableSwitchAndPHYs();

  if (enetConfigureSwitches(enetTaskDefsPtr->switchConfigNumber) > 0)
  {
    errorloop_forever("switch config error", LED_A1);
  }

  /* task for calling the generic PHY's polling handler */
  /* PHYs' ISR handling is vendor specific */
  /* therefore no interrupts for a GENERIC PHY, but polling */
  /* genericPhypollTask must not run before PHY_FrameworkInit() has terminated */
  freertos_stat = xTaskCreate(genericPhypollTask, (const char *const)"genericPhyPollTask", 4 * configMINIMAL_STACK_SIZE,
                              (void *)enetTaskDefsPtr, 11, &phyPollTaskHandle);

  if (freertos_stat != pdPASS)
    errorcnt++;

  if (errorcnt == 0)
  {
    if ((stat = PHY_FrameworkInit(phyConfig, phyDrivers)) != STATUS_SUCCESS)
    {
      errorcnt++;
    }
    else
      for (i = 0; i < N_PHY_DEVS; i++)
      {
        if ((stat = PHY_Init(i)) != STATUS_SUCCESS)
        {
          errorcnt++;
          continue;
        }
      }
  }

  /*
   * enable wakeup forwarding in the TJA11 devices - set Config1 register bit #2 (FWDPHYREM)
   */

  uint16_t tmpReg;
  TC10_ALLOWANCE const *t_ptr;

  for (i = 0; i < tc10_allowance_matrix_len; i++)
  {
    t_ptr = &tc10_allowance_matrix[i];

    if (phyConfig[t_ptr->phy_config_idx].type != 1U)
    { /* 1U == PHY_TYPE_TJA110X */
      /* plausibility: this is NOT a 100BASE-T1 PHY */
    }
    else
    {
      PHY_RMW(t_ptr->phy_config_idx, EXTENDED_CTRL_ADDR, (1U << 2), (1U << 2)); /* set CONFIG_EN */

      /*
       * Reg 18 setting
       * REMWUPHY        11  0x0800    PHY needs to react to a remote wakeup        link2phy
       * FWDPHYLOC       14  0x4000    wakeup event forwarded locally               link2wakeline
       * LOCWUPHY        10  0x0400    PHY reacts to local wakeup                   wakeline2phy
       * FWDPHYREM        2  0x0004    wakeup event forwarded remotely              wakeline2link
       */

      switch (t_ptr->remote)
      {
      case TC10_WAKE_IGNORE:
        tmpReg = 0;
        break;
      case TC10_WAKE_PHY_ONLY:
        tmpReg = 0x0800;
        break;
      case TC10_WAKE_PHY_AND_FORWARD:
        tmpReg = 0x4800;
        break;
      default:
        tmpReg = 0;
        break;
      }

      PHY_RMW(t_ptr->phy_config_idx, CONFIG1_ADDR, tmpReg, 0x4800);

      switch (t_ptr->wakeinoutline)
      {
      case TC10_WAKE_IGNORE:
        tmpReg = 0;
        break;
      case TC10_WAKE_PHY_ONLY:
        tmpReg = 0x0400;
        break;
      case TC10_WAKE_PHY_AND_FORWARD:
        tmpReg = 0x0404;
        break;
      default:
        tmpReg = 0;
        break;
      }

      PHY_RMW(t_ptr->phy_config_idx, CONFIG1_ADDR, tmpReg, 0x0404);

      /* CONFIG_WAKE = 1 -> ratiometric; this only works reliably since VDDIO is available during sleep, as only the PHYs sleep, and the board is powered */
      /* AUTO_OP = 0 -> set MANAGED_MODE; clear 0x8000; this requires to explicitly set link up after wakeup */
      PHY_RMW(t_ptr->phy_config_idx, COMMON_CONFIG_ADDR, 0x0040, 0x8040);

      PHY_RMW(t_ptr->phy_config_idx, EXTENDED_CTRL_ADDR, 0, (1U << 2)); /* clear CONFIG_EN */
    }
  }

  /* check if PHYs are in standby after reset button on s-board pressed */
  for (int i = 0; i < N_PHY_DEVS; i++)
  {
    PHY_Read(i, CONFIG1_ADDR, &conf1);

    if ((conf1 & CONF1_MASTER_SLAVE_FLAG) != 0U)
    {
      PHY_Read(i, EXTENDED_CTRL_ADDR, &extCtrl);
      if ((extCtrl & (1U << 14U)) && (extCtrl & (1U << 13U)))
      {
        PHY_RMW(i, EXTENDED_CTRL_ADDR, EXTENDED_CTRL_POWER_MODE_NORMAL_REQ, EXTENDED_CTRL_POWER_MODE_MASK); // enter normal mode
        vTaskDelay(DELAY_MS(2));                                                                            // wait 2 ms for phy to stabilize
        PHY_RMW(i, EXTENDED_CTRL_ADDR, EXTENDED_CTRL_LINK_CTRL_FLAG, EXTENDED_CTRL_LINK_CTRL_FLAG);         // set LINK_CONTROL to initiate training
      }
    }
  }

  /* for the dp83848 ANEG is not needed, so disable it */
  /* 0 is the index to the dp83848 PHY for the debug port */
  /* once names can be assigned to the PHYs, a #define from phy_cfg.h can be used instead */
  if ((stat = PHY_DisableAutoneg(0U)) != STATUS_SUCCESS)
  {
#if DEBUG_MSG
    console_puts("error PHY_DisableAutoneg\n\r");
#endif
    errorcnt++;
  }

  if (verboseFlag > 0)
  {
    console_puts("ethernet init errcnt ");
    console_putint(errorcnt, 4);
    console_puts("\n\r");
  }

  if (errorcnt)
  {
    errorloop_forever("PHY config error", LED_B0);
  }

#if PACKET_SEND_DEMO
  memcpy(&transmit_buffer[0], bcAddr, 6);
  memcpy(&transmit_buffer[6], enet_sw_MacAddr, 6);
  transmit_buffer[12] = type >> 8;
  transmit_buffer[13] = type & 0xff;

  vTaskDelay(DELAY_MS(3000));
#endif

#define ENET_TASK_PERIOD_MS 50 /* 50ms */
  for (;;)
  {
    uint8_t callNumber;

#if PACKET_SEND_DEMO
    if (!(callNumber % (2000 / ENET_TASK_PERIOD_MS)))
    {
      enet_buffer_t txBuff;

      for (i = 14; i < 100; i++)
      {
        transmit_buffer[i] = callNumber + (i / 16);
      }
      txBuff.data = transmit_buffer;
      txBuff.length = 100; /* could also be 98 */

      if (enetTaskDefsPtr->enettask_featureFlags & ENETTASK_FEATURE_SENDPKT)
      {
        /*
         * send out ethernet telegrams on both interfaces
         */
        transmit_buffer[14] = 0x2; /* 2 (looks like a s) for "switch" */
        stat = ENET_DRV_SendFrame(INST_ENET_SW, 0, &txBuff, NULL);

        transmit_buffer[14] = 0xd; /* d for "debug" */
        stat = ENET_DRV_SendFrame(INST_ENET_DBG, 0, &txBuff, NULL);
      }
    }
#endif

    vTaskDelay(DELAY_MS(ENET_TASK_PERIOD_MS));
    callNumber++;
  }
}
