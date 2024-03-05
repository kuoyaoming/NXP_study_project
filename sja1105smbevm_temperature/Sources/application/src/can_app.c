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

#include "Cpu.h"
#include "FreeRTOS.h"
#include "spi_can.h"
#include "can.h"
#include "pin_mux.h"

#include "app.h"
#include "can_app.h"
#include "sja1105smbevm_console.h"
#include "sja1105smbevm_gpio.h"

#include "task.h"

/* ==== CAN section ====================================================== */

static void
printSpiCanResult(int idx, char * dir, uint8_t addr, uint8_t val, uint8_t exp)
{
  if (verboseFlag > 0) {
    console_putint(idx, 6); console_puts("  ");
    console_puts(dir);      console_puts(" adr: ");
    console_puthex(addr);   console_puts(" val: ");
    console_puthex(val);    console_puts(" exp: ");
    console_puthex(exp);    console_puts("\n\r");
  }
}

static uint8_t
canspi_transaction(bool isWriteTransaction, uint8_t addr, uint8_t *val, uint8_t len)
{
uint16_t receivebuffer;
uint16_t sendbuffer;
volatile status_t res;

  if ((res = DSPI_UpdateCS(INST_SPI_CAN, 0)) != STATUS_SUCCESS) {
    return -1;
  }

  receivebuffer = 0x3333;
  sendbuffer = ((((uint16_t) addr) << 9) |
                ((isWriteTransaction ? 0U : 1U) << 8) |
                (*val << 0));

  res = DSPI_MasterTransferBlocking(INST_SPI_CAN, &sendbuffer, &receivebuffer, 1U, 1U);

  if (res == STATUS_SUCCESS) {
    if (!isWriteTransaction) {
      *val = receivebuffer & 0xff;
    }
    return 0;
  } else {
    return -2;
  }

}

typedef struct {
  uint8_t addr;
  bool    write;
  uint8_t value;
  uint8_t exp;
  bool    mandatoryReadback;
} CAN_TRANSACTION;

static CAN_TRANSACTION can_ta_list[] = {
  { 0x7e, false, 0,    0x74,  true},
  /* main status register */
  { 0x3,  false, 0,    0x20,  false},
  /* read mode control register and read it back */
  { 0x1,  false, 0,    0x02,  false},
  { 0x1,  true,  0x7,  0,     false},
  { 0x1,  false, 0,    0x07,  true},
  /* main status register */
  { 0x3,  false, 0,    0x00,  true},
  /* set CAN control register CMC = 10*/
  { 0x20, false, 0,    0,     false},
  { 0x20, true,  2,    0,     false},
  { 0x20, false, 0,    2,     true},
  /* read transceiver status register */
  { 0x22, false, 0,    0,     false},
};
static const size_t can_ta_list_len = sizeof(can_ta_list) / sizeof(*can_ta_list);

/* this is to prepare the CAN transceiver */
static void
sendSpiCanSequence(const CAN_TRANSACTION *ta_list, const size_t len)
{
int i;
const CAN_TRANSACTION * te;
uint8_t res;
uint8_t value;
uint8_t mandatoryReadbackError = 0;

  if (verboseFlag > 0) console_puts("\n\rsendSpiCanSequence\n\r");
  for (i = 0; i < len; i++) {
    te = &ta_list[i];
    if (te->write) {
      value = te->value;
      if ((res = canspi_transaction(true, te->addr, &value, 1)) == 0) {
        printSpiCanResult(i, "w", te->addr, te->value, 0x00);
      }
    } else {
      if ((res = canspi_transaction(false, te->addr, &value, 1)) == 0) {
        printSpiCanResult(i, "r", te->addr, value, te->exp);
        if (te->mandatoryReadback && (value != te->exp)) {
          mandatoryReadbackError++;
        }
      }
    }
  }

}

/* Set information about the data to be received - we only handle 4Byte telegrams */
const flexcan_data_info_t dataInfo = {
    .data_length = 4U,
    .msg_id_type = FLEXCAN_MSG_ID_STD,
    .enable_brs  = false,
    .fd_enable   = false,
    .fd_padding  = 0U
};

#define TX_IDX         0U
#define RX_IDX         1U
#define CAN_OUT_ID     0x333u

static void
handleCanError(status_t err, char * errString)
{
  console_puts(errString);
  switch (err) {
    case STATUS_TIMEOUT:
      console_puts(": Timeout\n\r");
      break;
    case STATUS_CAN_BUFF_OUT_OF_RANGE:
      console_puts(": OutOfRange\n\r");
      break;
    case STATUS_ERROR:
      console_puts(": Error\n\r");
      break;
    default:
      console_puts(": unspecified error\n\r");
  }
}

/*
 * canTask periodically sends out a packet
 */
void
can_task( void *pvParameters )
{
  status_t stat;
  uint16_t err = 0;
  uint32_t loop = 0;
  uint8_t sendMsg[8] = { 0, 1, 2, 3, 4, 5, 6, 0xff};

  /* let things around settle a little bit before starting initialization */
  vTaskDelay( DELAY_MS(50) );

  /* init the SPI bus to communicate with the CAN transceiver */
  DSPI_MasterInit(INST_SPI_CAN, &spi_canState, &spi_can_0);

  /* enable the CAN transceiver */
  sendSpiCanSequence(can_ta_list, can_ta_list_len);

  if ((stat = FLEXCAN_DRV_Init(INST_CAN, &can_State, &can_InitConfig0)) != STATUS_SUCCESS) err++;

  /*
   * end out TESTEE_OUT and MISC_ID telegrams
   */
  if ((stat = FLEXCAN_DRV_ConfigTxMb(INST_CAN, TX_IDX,      &dataInfo, CAN_OUT_ID)) != STATUS_SUCCESS) err++;

  if (err) {
    console_puts("CanTask aborted due to errors in init\n\r");
    vTaskDelete(NULL);
  }

  while (1) {
    sendMsg[1] = loop & 0xff;

    /* demo part: periodically send out a CAN message */
    stat = FLEXCAN_DRV_SendBlocking(INST_CAN, TX_IDX,     &dataInfo, CAN_OUT_ID, sendMsg, 10);
    if (stat != STATUS_SUCCESS) {
      handleCanError(stat, "CanTask - send CAN_OUT_ID");
      err++;
    }
    loop++;

    vTaskDelay(DELAY_MS(1000));
  }
}
