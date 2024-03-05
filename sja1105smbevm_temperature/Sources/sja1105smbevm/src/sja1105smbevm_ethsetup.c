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

#include "sja1105smbevm_ethsetup.h"
#include "sja1105smbevm_gpio.h"

/*
 * board specific table of associations between
 *   link LED
 *   phy driver's table idx
 *   MAC description (here: switch/processor) and for switch logical number of switch driver
 *
 */
const ethsetup_portLUT_t ethsetup_portLUT[] = {
  { LED_A1,     3U, { port_type_switch, 1U }  },    //!< connector A1 (TJA1102S)
  { 0,          1U, { port_type_switch, 2U }  },    //!< connector A2 (KSZ9031)
  { 0,          2U, { port_type_switch, 3U }  },    //!< connector A2 (KSZ9031)
  { LED_B0,     4U, { port_type_switch, 4U }  },    //!< connector B0 (TJA1102)
  { LED_B1,     5U, { port_type_switch, 5U }  },    //!< connector B1 (TJA1102)
  { LED_B2,     6U, { port_type_switch, 6U }  },    //!< connector B2 (TJA1102)
  { LED_B3,     7U, { port_type_switch, 7U }  },    //!< connector B3 (TJA1102)
  { 0,          0U, { port_type_enet, 1U }  },      //!< connector DBG (DP83848)
  { 0,          0U, { port_type_none, 255U } },     //!< no connector
};
const int ethsetup_portLUT_size = sizeof(ethsetup_portLUT)/sizeof(*ethsetup_portLUT);
