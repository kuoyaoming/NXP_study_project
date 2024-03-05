/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * NXP Confidential. This software is owned or controlled by NXP and may only be
 * used strictly in accordance with the applicable license terms. By expressly
 * accepting such terms or by downloading, installing, activating and/or otherwise
 * using the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms. If you do not agree to be
 * bound by the applicable license terms, then you may not retain, install,
 * activate or otherwise use the software. The production use license in
 * Section 2.3 is expressly granted for this software.
 */
/* ###################################################################
**     Filename    : main.c
**     Project     : flexio_i2c_s32k144
**     Processor   : S32K148_LQFP144
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2018-01-17, 16:55, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.00
** @brief
**         Main module.
**         This module contains user's application code.
*/
/*!
**  @addtogroup main_module main module documentation
**  @{
*/
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "sdk_project_config.h"

volatile int exit_code = 0;

#include <stdint.h>
#include <stdbool.h>

#define LED_PORT  (PTE)
#define LED_RED   (21u)
#define LED_GREEN (22u)
#define MPL3115A2_DEV_ADDR 0x60
#define MPL3115A2_REG_WHO_AM_I 0x0C
#define MPL3115A2_REG_WHO_AM_VALUE 0xC4
/* Declare transfer size */
#define TRANSFER_SIZE 8U

/* Define master and slave buffers */
//uint8_t slaveBuffer[TRANSFER_SIZE];
uint8_t masterBuffer[TRANSFER_SIZE];
bool CheckResult;
/*!
 *  @brief LPI2C Slave Callback
 *
 *  @param [in] instance   LPI2C instance number
 *  @param [in] slaveEvent Event received on the I2C bus
 *  @param [in] userData   User defined data that is passed to the callback
 *  @return None
 *
 *  @details This function will be called by LPI2C interrupt handler and it
 *  will assign the buffer for TX or RX events.
 *  If an error event occurs, it will abort the current transfer.
 */
//void lpi2c1_SlaveCallback0(i2c_slave_event_t slaveEvent,void *userData)
//{
//    /* Casting userData to void in order to avoid warning as the parameter is not used */
//    uint32_t instance;
//    instance = (uint32_t)userData;
//
//    /* Depending on the event received, set the buffers or abort the transfer */
//    switch(slaveEvent)
//    {
//        case I2C_SLAVE_EVENT_RX_REQ:
//            /*
//             * If the bus master requests data, then set the destination RX buffer
//             * and accepted transfer size
//             */
//            LPI2C_DRV_SlaveSetRxBuffer(instance, slaveBuffer, TRANSFER_SIZE);
//            break;
//        case I2C_SLAVE_EVENT_TX_REQ:
//            /*
//             * If the bus master sends data, then set the source TX buffer
//             * and accepted transfer size
//             */
//            LPI2C_DRV_SlaveSetTxBuffer(instance, slaveBuffer, TRANSFER_SIZE);
//            break;
//        case I2C_SLAVE_EVENT_TX_EMPTY:
//        case I2C_SLAVE_EVENT_RX_FULL:
//            /*
//             * If the TX buffer is empty or the RX buffer is full, abort the current
//             * transfer.
//             */
//            LPI2C_DRV_SlaveAbortTransferData(instance);
//            break;
//        case I2C_SLAVE_EVENT_STOP:
//            /*
//             * This case is used when a stop condition is on the bus. Because
//             * the example does not handle this case there is no action taken.
//             */
//            break;
//    }
//}

/*!
  \brief The main function for the project.
  \details The startup initialization sequence is the following:
 * - __start (startup asm routine)
 * - __init_hardware()
 * - main()
 *   - PE_low_level_init()
 *     - Common_Init()
 *     - Peripherals_Init()
*/
int main(void)
{
    /* Allocate memory for LPI2C driver internal structure */
//    lpi2c_slave_state_t lpi2cSlaveState;
    /* Allocate the memory necessary for the FlexIO state structures */
    flexio_i2c_master_state_t i2cMasterState;
    flexio_device_state_t flexIODeviceState;
    /* Variable that is used to initialize the buffers */
//    uint8_t cnt;
//    volatile bool isTransferOk[2] = {true, true};

    /* Initialize and configure clocks
     *  -   Setup system clocks, dividers
     *  -   Configure FlexIO clock, LPI2C
     *  -   see clock manager component for more details
     */
    CLOCK_SYS_Init(g_clockManConfigsArr, CLOCK_MANAGER_CONFIG_CNT,
                        g_clockManCallbacksArr, CLOCK_MANAGER_CALLBACK_CNT);
    CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_AGREEMENT);

    /* Initialize pins
     *  -   Init FlexIO, LPI2C pins
     *  -   See PinSettings component for more info
     */
    PINS_DRV_Init(NUM_OF_CONFIGURED_PINS0, g_pin_mux_InitConfigArr0);

    /* Use as callback parameter the LPI2C instance number */
//    lpi2c1_SlaveConfig0.callbackParam = (void *)INST_LPI2C1;
    /* Initialize LPI2C instance as bus slave */
//    LPI2C_DRV_SlaveInit(INST_LPI2C1, &lpi2c1_SlaveConfig0, &lpi2cSlaveState);

    /* Init FlexIO device */
    FLEXIO_DRV_InitDevice(INST_FLEXIO_I2C, &flexIODeviceState);

    /* Initialize FlexIO I2C driver as bus master */
    FLEXIO_I2C_DRV_MasterInit(INST_FLEXIO_I2C, &flexio_i2c_MasterConfig0, &i2cMasterState);
    FLEXIO_I2C_DRV_MasterSetSlaveAddr(&i2cMasterState,MPL3115A2_DEV_ADDR);
    masterBuffer[0]=MPL3115A2_REG_WHO_AM_I;
    FLEXIO_I2C_DRV_MasterSendDataBlocking(&i2cMasterState, masterBuffer, 1u, false, 10UL);
    FLEXIO_I2C_DRV_MasterReceiveDataBlocking(&i2cMasterState, masterBuffer, 1u, true, 10UL);
	CheckResult = true;

	if (masterBuffer[0] != MPL3115A2_REG_WHO_AM_VALUE)
		CheckResult = false;

	/* Turn on Red or Green LED depending on the check result */
	PINS_DRV_WritePin(LED_PORT, LED_RED, (1u - CheckResult));
	PINS_DRV_WritePin(LED_PORT, LED_GREEN, CheckResult);

  for(;;) {
    if(exit_code != 0) {
      break;
    }
  }
  return exit_code;
}
