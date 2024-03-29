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
  UUID: c8c315ab-21c4-45ed-bf54-eea7465b9ab6
  called_from_default_init: true
  selectedCore: core0
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/*******************************************************************************
 * Included files 
 ******************************************************************************/
#include "peripherals_flexio_spi1.h"

/*******************************************************************************
 * flexio_spi1 initialization code
 ******************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'flexio_spi1'
- type: 'flexio_spi_config'
- mode: 'general'
- custom_name_enabled: 'true'
- type_id: 'flexio_spi'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'FLEXIO'
- config_sets:
  - flexio_spi_driver:
    - flexio_spi_master_Configuration:
      - 0:
        - name: 'flexio_spi1_MasterConfig0'
        - readonly: 'false'
        - baudRate: '100000'
        - driverType: 'FLEXIO_DRIVER_TYPE_INTERRUPTS'
        - bitOrder: 'FLEXIO_SPI_TRANSFER_MSB_FIRST'
        - transferSize: 'FLEXIO_SPI_TRANSFER_1BYTE'
        - clockPolarity: '0'
        - clockPhase: '0'
        - mosiPin: '3'
        - misoPin: '2'
        - sckPin: '4'
        - ssPin: '5'
        - callback: 'NULL'
    - flexio_spi_slave_Configuration: []
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

/* FlexioSPI master configuration structure */
flexio_spi_master_user_config_t flexio_spi1_MasterConfig0 = {
  .baudRate = 100000UL,
  .driverType = FLEXIO_DRIVER_TYPE_INTERRUPTS,
  .bitOrder = FLEXIO_SPI_TRANSFER_MSB_FIRST,
  .transferSize = FLEXIO_SPI_TRANSFER_1BYTE,
  .clockPolarity = 0U,
  .clockPhase = 0U,
  .mosiPin = 3U,
  .misoPin = 2U,
  .sckPin = 4U,
  .ssPin = 5U,
  .callback = NULL,
  .callbackParam = NULL
};


