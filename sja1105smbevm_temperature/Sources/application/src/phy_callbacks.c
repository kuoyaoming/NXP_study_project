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
#include <string.h>

#include "Cpu.h"

#include "sja1105smbevm_console.h"
#include "sja1105smbevm_gpio.h"
#include "sja1105smbevm_ethsetup.h"

#include "FreeRTOS.h"
#include "task.h"

#include "NXP_SJA1105P_config.h"
#include "NXP_SJA1105P_switchCore.h"
#include "NXP_SJA1105P_portConfig.h"

#define DEBUG_MSG 1

uint32_t
phy_callbacks_getLedByPhyCfgIdx(uint8_t phy_config_idx) {
  int i;

  for (i = 0; i < ethsetup_portLUT_size; i++) {
    if (ethsetup_portLUT[i].phy_config_idx == phy_config_idx) {
      return ethsetup_portLUT[i].led;
    }
  }
  return 0;
}

/* phyIdx is used by PHY driver and is generated by PE */
static uint8_t
phy_callbacks_getPortDescrByPhyIdx(const uint8_t phyIdx, port_descr_t *p_port_descr)
{
  int i;
  uint8_t res = 1;	/* error */

  for (i = 0; i < ethsetup_portLUT_size; i++) {
    if (ethsetup_portLUT[i].phy_config_idx == phyIdx) {
      *p_port_descr = ethsetup_portLUT[i].port;
      res = 0;
      break;
    }
  }
  return res;
}

static void
switch_setDelayLine(uint8_t idx, SJA1105P_speed_t sp, bool verbose)
{
  if ((ethsetup_portLUT[idx].port.n == 2) ||
      (ethsetup_portLUT[idx].port.n == 3)) {
    /* these are the only RGMII connections on the board - both use KSZ9031 */

    /* KSZ9031 has a 1.2ns delay on the RX side - even with default delay settings of 0 */
    /* therefore set TX to default delay of 81.0deg, but RX to minimal delay of 73.8deg */
    /* port speed                          1000Mbit      100Mbit       10Mbit
     * clock frequency                     125MHz        25MHz         2.5MHz
     * clock cycle                         8.0ns         40.0ns        400ns
     * skew calculation
     * 810U = 81.0deg                      1.8ns         9.0ns         90.0ns
     * 738U = 73.8deg                      1.64ns        8.2ns         82.0ns
     *
     * overall clock skew 1.0..2.6ns;
     * with 738U RX clock skew is 1.64ns+1.2ns = 2.84ns which is a little over 2.6ns
     * We cannot get it better, if we don't want to change the PHY's default delay line setting
     *
     * Empirical evidence: this does not work for RX at 100Mbps.
     * Therefore, disable RX delay line for 100Mbps and 10Mbps
     *
	 */
    if (sp == SJA1105P_e_speed_1_GBPS) {
	  SJA1105P_setupClockDelay(810U, ethsetup_portLUT[idx].port.n, 0, SJA1105P_e_direction_TX);
	  SJA1105P_setupClockDelay(738U, ethsetup_portLUT[idx].port.n, 0, SJA1105P_e_direction_RX);
#if DEBUG_MSG
	  console_puts(" + delay lines (RX=738+TX=810)");
#endif
    } else if (sp == SJA1105P_e_speed_100_MBPS) {
	  SJA1105P_setupClockDelay(810U, ethsetup_portLUT[idx].port.n, 0, SJA1105P_e_direction_TX);
	  SJA1105P_setupClockDelay(0, ethsetup_portLUT[idx].port.n, 0, SJA1105P_e_direction_RX);
#if DEBUG_MSG
	  console_puts(" + delay lines (RX=0+TX=810)");
#endif
    } else {
	  SJA1105P_setupClockDelay(810U, ethsetup_portLUT[idx].port.n, 0, SJA1105P_e_direction_TX);
	  SJA1105P_setupClockDelay(0, ethsetup_portLUT[idx].port.n, 0, SJA1105P_e_direction_RX);

#if DEBUG_MSG
	  console_puts(" + delay lines (RX=0+TX=810)");
#endif
    }
  }
}

void
phy_callbacks_linkUpEventCB(uint8_t idx)
{
  GPIO_SETLED(phy_callbacks_getLedByPhyCfgIdx(idx), true);

#if DEBUG_MSG
  console_puts("phy ");
  console_putint(idx, 3);
  console_puts("  link up\n\r");
#endif

  switch_setDelayLine(idx, SJA1105P_e_speed_10_MBPS, true);
}

void
phy_callbacks_linkDownEventCB(uint8_t idx)
{
  GPIO_SETLED(phy_callbacks_getLedByPhyCfgIdx(idx), false);

#if DEBUG_MSG
  console_puts("phy ");
  console_putint(idx, 3);
  console_puts("  link down\n\r");
#endif
}

static status_t
setEnetMacSpeed(uint8_t portNr, phy_speed_t sp)
{
  /* portNr not used because there is only one port for EnetMac */
  switch (sp) {
    case PHY_SPEED_100_MBPS:
      ENET_DRV_SetSpeed(INST_ENET_DBG, ENET_MII_SPEED_100M);
      break;
    case PHY_SPEED_10_MBPS:
      ENET_DRV_SetSpeed(INST_ENET_DBG, ENET_MII_SPEED_10M);
      break;
    case PHY_SPEED_1_GBPS:
    case PHY_SPEED_RESERVED:
    default:
      return STATUS_ERROR;
  }
  return STATUS_SUCCESS;
}


/* these data structures really should be local variables of the
 * setSwitchMacSpeed() function, and hence to the stack.
 * However, due to the limited stack space, they are now
 * on the heap, despite this wastes memory, as the memory is
 * permanently used.
 */
static SJA1105P_macCfgTableEntryArgument_t macCfgTableEntry;
static SJA1105P_macCfgTableControlArgument_t macCfgTableControl;
/*
 * port: logical port as in NXP_SJA1105P driver (config)
 * sp: speed
 * idx: PHY table
 */
static status_t
setSwitchMacSpeed(uint8_t port, phy_speed_t sp, uint8_t idx)
{
  SJA1105P_port_t portDescr;
  SJA1105P_speed_t xmiiSpeed;
  SJA1105P_portStatusMiixArgument_t portStatus;

  uint8_t ret;

  if (SJA1105P_getPhysicalPort(port, &portDescr)) {
    return STATUS_ERROR;
  } else {
    macCfgTableControl.port = portDescr.physicalPort;
    macCfgTableControl.valid = 1;
    macCfgTableControl.rdwrset = 0;	/* read */

    ret  = SJA1105P_setMacCfgTableControl(&macCfgTableControl, portDescr.switchId);
    ret += SJA1105P_getMacCfgTableEntry(&macCfgTableEntry, portDescr.switchId);

    if (ret) return STATUS_ERROR;

    switch (sp) {
      case PHY_SPEED_1_GBPS:
        macCfgTableEntry.speed = 1;
        xmiiSpeed = SJA1105P_e_speed_1_GBPS;
        break;
      case PHY_SPEED_100_MBPS:
        macCfgTableEntry.speed = 2;
        xmiiSpeed = SJA1105P_e_speed_100_MBPS;
        break;
      case PHY_SPEED_10_MBPS:
        macCfgTableEntry.speed = 3;
        xmiiSpeed = SJA1105P_e_speed_10_MBPS;
        break;
      case PHY_SPEED_RESERVED:
      default:
        macCfgTableEntry.speed = 0;
        xmiiSpeed = SJA1105P_e_speed_10_MBPS;
    }

    macCfgTableControl.port = portDescr.physicalPort;
    macCfgTableControl.valid = 1;
    macCfgTableControl.rdwrset = 1;	/* write */

    ret  = SJA1105P_setMacCfgTableEntry(&macCfgTableEntry, portDescr.switchId);
    ret += SJA1105P_setMacCfgTableControl(&macCfgTableControl, portDescr.switchId);

#if DEBUG_MSG
      console_puts("switch speed sw ");
      console_putint(portDescr.switchId, 2);
      console_puts(" port ");
      console_putint(portDescr.physicalPort, 3);
      console_puts(" speed ");
      console_putint(sp, 3);
      console_puts(" set MAC Table ");
      console_putint(ret, 2);
      console_puts("\n\r");
#endif
    if (ret) {
      return STATUS_ERROR;
    } else if (sp == PHY_SPEED_RESERVED) {
      return STATUS_SUCCESS;
    }


    /* reconfigure port only if ANEG has succeeded and returned a valid speed */
    ret  = SJA1105P_getPortStatusMiix(&portStatus, portDescr.physicalPort, portDescr.switchId);
    ret += SJA1105P_configPort(portDescr.physicalPort, portDescr.switchId, xmiiSpeed, portStatus.xmiiMode, portStatus.phyMode);

    vTaskDelay( (TickType_t) 2 / portTICK_PERIOD_MS);        /* give time until clock change has propagated to the PLL */

    switch_setDelayLine(idx, xmiiSpeed, true);	/* reset the delay lines */

    if (ret) {
      return STATUS_ERROR;
    } else {
      return STATUS_SUCCESS;
    }
  }
}

void
phy_callbacks_autonegCompleteCB(uint8_t idx)
{
  phy_speed_t sp;
  port_descr_t port_descr;
  status_t status;

  if ((phy_callbacks_getPortDescrByPhyIdx(idx, &port_descr) == 0) &&
      ((status = PHY_GetLinkSpeed(idx, &sp)) == STATUS_SUCCESS)) {

    /* find out which switch and which port to change the MAC config table */
    switch (sp) {
    case PHY_SPEED_1_GBPS:
    case PHY_SPEED_100_MBPS:
    case PHY_SPEED_10_MBPS:
      switch (port_descr.t) {
      case port_type_switch:
        status = setSwitchMacSpeed(port_descr.n, sp, idx);
        break;
      case port_type_enet:
        status = setEnetMacSpeed(port_descr.n, sp);
        break;
      default:
        status = STATUS_ERROR;
      }
      break;
    case PHY_SPEED_RESERVED:
    default:
      phy_callbacks_autonegFailedCB(idx);
    }
  }

  /*
   * should set set FD/HD here
   * FD/HD is not supported
   */

}


void
phy_callbacks_autonegFailedCB(uint8_t idx)
{
  port_descr_t port_descr;

  if (phy_callbacks_getPortDescrByPhyIdx(idx, &port_descr) == 0) {

    switch (port_descr.t) {
      case port_type_switch:
    	setSwitchMacSpeed(port_descr.n, PHY_SPEED_RESERVED, idx);
    	break;
      case port_type_enet:
        setEnetMacSpeed(port_descr.n, PHY_SPEED_RESERVED);
        break;
      case port_type_none:
    	/* do nothing */
    	break;
    }
  }

  return;
}
