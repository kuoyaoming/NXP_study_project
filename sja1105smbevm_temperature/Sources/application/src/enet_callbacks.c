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
#include "enet_callbacks.h"
#include "sja1105smbevm_gpio.h"

#include "Cpu.h"

#include "FreeRTOS.h"
#include "task.h"

#include "enet_sw.h"
#include "enet_dbg.h"

/*
 * ethernet receive callbacks for processor's switch eth port and dbg eth port
 * essentials of received packet are copied into a transfer buffer for
 * visualisation in main task;
 * statistic counters are handled for debugging
 */

typedef struct {
  uint32_t  txPkt;
  uint32_t  rxPkt;
  uint32_t  errEvent;
  uint32_t  wkupEvent;
} ETH_STAT;

static ETH_STAT sw_stat, dbg_stat;

enet_callbacks_mirrlog_t enet_sw_rcvlog, enet_dbg_rcvlog;

void
enet_dbg_rxCB(uint8_t instance, enet_event_t event, uint8_t ring)
{
enet_buffer_t rxBuff;
status_t stat;

  switch (event) {
    case ENET_RX_EVENT:

      stat = ENET_DRV_ReadFrame(instance, ring, &rxBuff, NULL);
      if (stat == STATUS_SUCCESS) {
        /* testee seemed to have received back the sent packet after having been mirrored on tester */
        memcpy((uint8_t *) &enet_dbg_rcvlog.dst, &rxBuff.data[2], 4);	/* only LSB */
        memcpy((uint8_t *) &enet_dbg_rcvlog.src, &rxBuff.data[8], 4);
        memcpy((uint8_t *) &enet_dbg_rcvlog.type, &rxBuff.data[12], 2);
        enet_dbg_rcvlog.pktnumber = rxBuff.data[15];
        ENET_DRV_ProvideRxBuff(instance, ring, &rxBuff);
      }
      dbg_stat.rxPkt++;
      break;
    case ENET_TX_EVENT:
      dbg_stat.txPkt++;
      break;
    case ENET_ERR_EVENT:
      dbg_stat.errEvent++;
      break;
    case ENET_WAKE_UP_EVENT:
      dbg_stat.wkupEvent++;
      break;
    default:
      ;
  }
}

static uint32_t rcvdEthPkt;
void
enet_sw_rxCB(uint8_t instance, enet_event_t event, uint8_t ring)
{
enet_buffer_t rxBuff;
status_t stat;

  switch (event) {
    case ENET_RX_EVENT:

      stat = ENET_DRV_ReadFrame(instance, ring, &rxBuff, NULL);

      if (stat == STATUS_SUCCESS) {

        /* testee seemed to have received the mirrored send packet */
        memcpy((uint8_t *) &enet_sw_rcvlog.dst, &rxBuff.data[2], 4);	/* only LSB */
        memcpy((uint8_t *) &enet_sw_rcvlog.src, &rxBuff.data[8], 4);
        memcpy((uint8_t *) &enet_sw_rcvlog.type, &rxBuff.data[12], 2);
        enet_sw_rcvlog.pktnumber = rxBuff.data[15];

        /* only print a debug log if test is not terminally succeeded */
        enet_sw_rcvlog.ctr++;

        rcvdEthPkt++;

        ENET_DRV_ProvideRxBuff(instance, ring, &rxBuff);
      }
      sw_stat.rxPkt++;
      break;
    case ENET_TX_EVENT:
      sw_stat.txPkt++;
      break;
    case ENET_ERR_EVENT:
      sw_stat.errEvent++;
      break;
    case ENET_WAKE_UP_EVENT:
      sw_stat.wkupEvent++;
      break;
    default:
      ;
  }

}

