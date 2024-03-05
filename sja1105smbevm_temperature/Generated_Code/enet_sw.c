/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : enet_sw.c
**     Project     : sja1105smbevm_tc10example
**     Processor   : MPC5748G_256
**     Component   : enet
**     Version     : Component SDK_S32_PA_11, Driver 01.00, CPU db: 3.00.000
**     Repository  : SDK_S32_PA_11
**     Compiler    : GNU C Compiler
**     Date/Time   : 2023-07-11, 10:21, # CodeGen: 0
**
**     Copyright 1997 - 2015 Freescale Semiconductor, Inc. 
**     Copyright 2016-2017 NXP 
**     All Rights Reserved.
**     
**     THIS SOFTWARE IS PROVIDED BY NXP "AS IS" AND ANY EXPRESSED OR
**     IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
**     OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
**     IN NO EVENT SHALL NXP OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
**     INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
**     SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
**     HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
**     STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
**     IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
**     THE POSSIBILITY OF SUCH DAMAGE.
** ###################################################################*/
/*!
** @file enet_sw.c
** @version 01.00
*/         
/*!
**  @addtogroup enet_sw_module enet_sw module documentation
**  @{
*/         

#include "enet_sw.h"

/**
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.7, External variable could be made static.
 * The external variables will be used in other source files, with the same initialized values.
 */

/*! @brief Number of receive buffer descriptors for ring 0 */
#define ENET_RXBD_NUM0_0 (4U)
/*! @brief Number of transmit buffer descriptors for ring 0 */
#define ENET_TXBD_NUM0_0 (4U)

enet_state_t enet_sw_State; 

/*! @brief The MAC address of the module */
uint8_t enet_sw_MacAddr[6] = {0x08, 0x00, 0x27, 0x02, 0x12, 0x01};

/*! @brief Module configuration structures */
const enet_config_t enet_sw_InitConfig0 =
{
    .interrupts = (uint32_t)ENET_RX_FRAME_INTERRUPT | (uint32_t)ENET_TX_FRAME_INTERRUPT | (uint32_t)ENET_BABR_INTERRUPT | ENET_BABT_INTERRUPT | ENET_EBERR_INTERRUPT | (uint32_t)ENET_LATE_COLLISION_INTERRUPT | ENET_RETRY_LIMIT_INTERRUPT | ENET_UNDERRUN_INTERRUPT | (uint32_t)ENET_PAYLOAD_RX_INTERRUPT,
    .maxFrameLen = 1518U,
    .miiMode = ENET_MII_MODE,
    .miiSpeed = ENET_MII_SPEED_100M,
    .miiDuplex = ENET_MII_FULL_DUPLEX,
    .rxAccelerConfig = 0,
    .txAccelerConfig = 0,
    .rxConfig = 0,
    .txConfig = 0,
    .ringCount = ENET_SW_NUM_RINGS0,
    .callback = enet_sw_rxCB
};

/*! @brief Receive buffer descriptors for ring 0 */
ALIGNED(FEATURE_ENET_BUFFDESCR_ALIGNMENT) static enet_buffer_descriptor_t enet_sw_rxBuffDescrip0_0[ENET_RXBD_NUM0_0];
/*! @brief Transmit buffer descriptors for ring 0 */
ALIGNED(FEATURE_ENET_BUFFDESCR_ALIGNMENT) static enet_buffer_descriptor_t enet_sw_txBuffDescrip0_0[ENET_TXBD_NUM0_0];

/*! @brief Receive data buffers for ring 0 */
ALIGNED(FEATURE_ENET_BUFF_ALIGNMENT) static uint8_t enet_sw_rxDataBuff0_0[ENET_RXBD_NUM0_0 * ENET_BUFF_ALIGN(1518U)];

/*! @brief Buffer configuration structures */
const enet_buffer_config_t enet_sw_buffConfigArr0[] = {
    {
        ENET_RXBD_NUM0_0,
        ENET_TXBD_NUM0_0,
        &enet_sw_rxBuffDescrip0_0[0],
        &enet_sw_txBuffDescrip0_0[0],
        &enet_sw_rxDataBuff0_0[0]
    },
};
/* END enet_sw. */

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.1 [05.21]
**     for the NXP C55 series of microcontrollers.
**
** ###################################################################
*/

