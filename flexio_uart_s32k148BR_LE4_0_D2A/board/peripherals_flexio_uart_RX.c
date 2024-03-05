/***********************************************************************************************************************
 * This file was generated by the S32 Configuration Tools. Any manual edits made to this file
 * will be overwritten if the respective S32 Configuration Tools is used to update this file.
 **********************************************************************************************************************/

/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Peripherals v10.0
processor: S32K148
package_id: S32K148_LQFP144
mcu_data: s32sdk_s32k1xx_rtm_402
processor_version: 0.0.0
functionalGroups:
- name: BOARD_InitPeripherals
  UUID: 6e315fab-77ad-46ec-85ed-f33981ad8492
  called_from_default_init: true
  selectedCore: core0
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/*******************************************************************************
 * Included files 
 ******************************************************************************/
#include "peripherals_flexio_uart_RX.h"

/*******************************************************************************
 * flexio_uart_RX initialization code
 ******************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'flexio_uart_RX'
- type: 'flexio_uart_config'
- mode: 'general'
- custom_name_enabled: 'true'
- type_id: 'flexio_uart'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'FLEXIO'
- config_sets:
  - flexio_uart_driver:
    - flexio_uart_Configuration:
      - 0:
        - name: 'flexio_uart_RX_Config0'
        - readonly: 'true'
        - driverType: 'FLEXIO_DRIVER_TYPE_INTERRUPTS'
        - baudRate: '115200'
        - bitCount: '8'
        - direction: 'FLEXIO_UART_DIRECTION_RX'
        - dataPin: '1'
        - callback: 'flexio_uart_RX_Callback0'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/**
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.7, External variable could be made static.
 * The external variables will be used in other source files in application code.
 *
 */

/* FlexioUART configuration structure */
const flexio_uart_user_config_t flexio_uart_RX_Config0 = {
  .driverType = FLEXIO_DRIVER_TYPE_INTERRUPTS,
  .baudRate = 115200UL,
  .bitCount = 8U,
  .direction = FLEXIO_UART_DIRECTION_RX,
  .dataPin = 1U,
  .callback = flexio_uart_RX_Callback0,
  .callbackParam = NULL
};


