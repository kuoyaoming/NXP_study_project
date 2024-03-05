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
  UUID: 8f74fc8a-8989-4964-a5f1-43bb6786778f
  called_from_default_init: true
  selectedCore: core0
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/*******************************************************************************
 * Included files 
 ******************************************************************************/
#include "peripherals_flexio_i2c.h"

/*******************************************************************************
 * flexio_i2c initialization code
 ******************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'flexio_i2c'
- type: 'flexio_i2c_config'
- mode: 'general'
- custom_name_enabled: 'true'
- type_id: 'flexio_i2c'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'FLEXIO'
- config_sets:
  - flexio_i2c_driver:
    - flexio_i2c_Configuration:
      - 0:
        - name: 'flexio_i2c_MasterConfig0'
        - readonly: 'true'
        - slaveAddress: '37'
        - driverType: 'FLEXIO_DRIVER_TYPE_INTERRUPTS'
        - baudRate: '400000'
        - sdaPin: '2'
        - sclPin: '3'
        - callback: 'NULL'
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

/* FlexioI2C configuration structure */
const flexio_i2c_master_user_config_t flexio_i2c_MasterConfig0 = {
  .slaveAddress = 37U,
  .driverType = FLEXIO_DRIVER_TYPE_INTERRUPTS,
  .baudRate = 400000UL,
  .sdaPin = 2U,
  .sclPin = 3U,
  .callback = NULL,
  .callbackParam = NULL
};


