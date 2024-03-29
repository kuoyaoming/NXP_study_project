/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : spi_sw.c
**     Project     : sja1105smbevm_tc10example
**     Processor   : MPC5748G_256
**     Component   : dspi
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
** @file spi_sw.c
** @version 01.00
*/         
/*!
**  @addtogroup spi_sw_module spi_sw module documentation
**  @{
*/         

/*
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.7, External could be made static.
 * This structure is defined for usage by application code.
 *
 */
 
#include "spi_sw.h"

/*Define state structure for current DSPI instance */
dspi_state_t spi_swState; 
/* SPI Master Configurations */
dspi_master_config_t spi_sw_0 =
{
    .bitsPerSec              = 2000000,
    .pcsPolarity             = DSPI_ACTIVE_LOW,
    .bitcount                = 32,
    .clkPhase                = DSPI_CLOCK_PHASE_2ND_EDGE,
    .clkPolarity             = DSPI_ACTIVE_HIGH,
    .lsbFirst                = false,
    .transferType            = DSPI_USING_INTERRUPTS,
    .rxDMAChannel            = 255U,
    .txDMAChannel            = 255U,
    .callback                = NULL,
    .callbackParam           = NULL,
    .continuousPCS           = true,
    .whichPCS                = 0
};
dspi_master_config_t spi_sw_1 =
{
    .bitsPerSec              = 2000000,
    .pcsPolarity             = DSPI_ACTIVE_LOW,
    .bitcount                = 32,
    .clkPhase                = DSPI_CLOCK_PHASE_2ND_EDGE,
    .clkPolarity             = DSPI_ACTIVE_HIGH,
    .lsbFirst                = false,
    .transferType            = DSPI_USING_INTERRUPTS,
    .rxDMAChannel            = 255U,
    .txDMAChannel            = 255U,
    .callback                = NULL,
    .callbackParam           = NULL,
    .continuousPCS           = true,
    .whichPCS                = 1
};
 
/* SPI Slave Configurations */
/* END spi_sw. */

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


