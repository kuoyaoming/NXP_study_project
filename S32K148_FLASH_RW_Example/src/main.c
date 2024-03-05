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

#define EVB

#define PCC_CLOCK PCC_PORTE_CLOCK


#define uart_instance uart_instance_lu0
#define UART_PAL_CONFIG &LPUART_0_uart_pal_config0

#include "sdk_project_config.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "interrupt_manager.h"
/*Fix logging index address*/

bool sw3_f = false;
bool sw4_f = false;
unsigned int temp[8];
status_t ret; /* Store the driver APIs return code */
flash_ssd_config_t flashSSDConfig;

/*
 * FTFC Flash command callback, it will be called when
 * Flash command is executing
 */
void __attribute__((section(".code_ram"))) Flash_Command_Callback(void)
{
    /* refresh the WDOG watchdog to avoid timeout reset */
    WDOG->CNT = 0xB480A602U;
}

void delay(volatile int cycles)
{
    /* Delay function - do nothing for a number of cycles */
    while (cycles--)
        ;
}

/*!
  \brief Interrupt handler for Flash Command Complete event
*/
void CCIF_Handler(void)
{
    /* Disable Flash Command Complete interrupt */
    FTFx_FCNFG &= (~FTFx_FCNFG_CCIE_MASK);

    return;
}



void PORTC_irq_handler(void)
{
    uint32_t Port_IntFlag = 0;

    Port_IntFlag = PINS_DRV_GetPortIntFlag(PORTC);

    if (Port_IntFlag & (1 << SW3_PIN))
    {
        sw3_f = true;
        PINS_DRV_ClearPinIntFlagCmd(SW3_PORT, SW3_PIN);
    }

    if (Port_IntFlag & (1 << SW4_PIN))
    {
        sw4_f = true;
        PINS_DRV_ClearPinIntFlagCmd(SW4_PORT, SW4_PIN);
    }
    PINS_DRV_ClearPortIntFlagCmd(PORTC);
}

int main(void)
{


	uint32_t address, size;
	int data_n = 0, data_r = 0;

    status_t error;
    /* Configure clocks for PORT */
    error = CLOCK_DRV_Init(&clockMan1_InitConfig0);
    DEV_ASSERT(error == STATUS_SUCCESS);
    /* Set pins as GPIO */
    error = PINS_DRV_Init(NUM_OF_CONFIGURED_PINS0, g_pin_mux_InitConfigArr0);
    DEV_ASSERT(error == STATUS_SUCCESS);
    PINS_DRV_Init(NUM_OF_CONFIGURED_PINS0, g_pin_mux_InitConfigArr0);

    UART_Init(&uart_instance, UART_PAL_CONFIG);

    /* Send a welcome message */

    INT_SYS_InstallHandler(FTFC_IRQn, CCIF_Handler, (isr_t *)0);
    INT_SYS_EnableIRQ(FTFC_IRQn);

    INT_SYS_InstallHandler(PORTC_IRQn, PORTC_irq_handler, (isr_t *)NULL);
    INT_SYS_EnableIRQ(PORTC_IRQn);

    /* Enable global interrupt */
    INT_SYS_EnableIRQGlobal();

    ret = FLASH_DRV_Init(&Flash_InitConfig0, &flashSSDConfig);
    DEV_ASSERT(STATUS_SUCCESS == ret);


    for(int i =0; i<8;i++){
    	temp[i]=i;
    }

    address = flashSSDConfig.DFlashBase;
	size = FEATURE_FLS_DF_BLOCK_SECTOR_SIZE;

    for (;;)
    {

        if (sw3_f)
        {
            sw3_f = false;
            PINS_DRV_TogglePins(LED_R_PORT, 1 << LED_R_PIN);
            printf("button sw3 pressed, write data to flash\n");

            // Flash erase and write operations
            INT_SYS_DisableIRQGlobal(); /*disable CPU global interrupt*/
			ret = FLASH_DRV_EraseSector(&flashSSDConfig, address, size);
			DEV_ASSERT(STATUS_SUCCESS == ret);
			INT_SYS_EnableIRQGlobal(); /*enable CPU global interrupt*/
			printf("Flash Erase\n");
			temp[0]=data_n;
			INT_SYS_DisableIRQGlobal();
			ret = FLASH_DRV_Program(&flashSSDConfig,address,8u,temp);
			DEV_ASSERT(STATUS_SUCCESS == ret);
			INT_SYS_EnableIRQGlobal();
			printf("Flash write = %d\n",data_n);
			data_n++;
        }
        if (sw4_f)
        {
            sw4_f = false;
            PINS_DRV_TogglePins(LED_G_PORT, 1 << LED_G_PIN);
            printf("button sw4 pressed, read data from flash\n");

            // Flash read operation
            data_r = *(unsigned int *)address;
            printf("Flash read = %d\n",data_r);
        }
    }
}
