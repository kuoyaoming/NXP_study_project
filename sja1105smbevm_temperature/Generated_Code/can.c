/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : can.c
**     Project     : sja1105smbevm_tc10example
**     Processor   : MPC5748G_256
**     Component   : flexcan
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
** @file can.c
** @version 01.00
*/         
/*!
**  @addtogroup can_module can module documentation
**  @{
*/         

/* MODULE can.
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 8.1, types should be explicitly specified  
 * This rule applies only for C90 standard. Defining other integer types that are 
 * permitted in bit-field declarations is allowed in C99 standard.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.7, External variable could be made static.
 * The external variables will be used in other source files that user initialize
 * to use this module.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 8.4, external symbol defined without a prior declaration  
 * This symbols are declared in the generated configuration structures  
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 1.3, occurrence of undefined symbol
 * This module is used only with defined values based on can interface features.
 * 
 * @section [global]
 * Violates MISRA 2012 Required Rule 10.3, Expression assigned to a narrower or different essential type.
 * The assignation of boolean elements is the same type as declared variable that use them.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 13.1, Expression not contain persistent side effects.
 * The assignation does not have side effects, code based on can interface features.
 */

#include "can.h"

flexcan_state_t can_State; 

const flexcan_user_config_t can_InitConfig0 = {
    .fd_enable = false,
    .pe_clock = FLEXCAN_CLK_SOURCE_OSC,
    .max_num_mb = 16,
    .num_id_filters = FLEXCAN_RX_FIFO_ID_FILTERS_8,
    .is_rx_fifo_needed = false,
    .flexcanMode = FLEXCAN_NORMAL_MODE,
    .payload = FLEXCAN_PAYLOAD_SIZE_8,
    .bitrate = {
        .propSeg = 7,
        .phaseSeg1 = 4,
        .phaseSeg2 = 1,
        .preDivider = 4,
        .rJumpwidth = 1
    },
    .bitrate_cbt = {
        .propSeg = 7,
        .phaseSeg1 = 4,
        .phaseSeg2 = 1,
        .preDivider = 4,
        .rJumpwidth = 1
    },
    .transfer_type = FLEXCAN_RXFIFO_USING_INTERRUPTS,
    .rxFifoDMAChannel = 0U
};


/* END can. */

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


