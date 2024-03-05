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

#ifndef SJA1105SMBEVM_INC_SJA1105SMBEVM_ETHSETUP_H_
#define SJA1105SMBEVM_INC_SJA1105SMBEVM_ETHSETUP_H_

#include <stdint.h>

typedef enum {
  port_type_none,
  port_type_switch, 		/* PHY connected to switch port */
  port_type_enet  			/* PHY connected to processor */
} port_e;

typedef struct {
  port_e      t;			/* type of port this PHY is connected to */
  uint8_t     n;            /* number; for switch: log. port number switch driver; for processor: port idx for enet driver */
} port_descr_t;

typedef struct {
  uint32_t                      led;
  uint8_t                       phy_config_idx;	             /* idx in PE generated PHY table */
  port_descr_t                  port;
} ethsetup_portLUT_t;

extern const ethsetup_portLUT_t ethsetup_portLUT[];
extern const int ethsetup_portLUT_size;
extern uint32_t phy_callbacks_getLedByPhyCfgIdx(uint8_t phy_config_idx);

#endif /* SJA1105SMBEVM_INC_SJA1105SMBEVM_ETHSETUP_H_ */
