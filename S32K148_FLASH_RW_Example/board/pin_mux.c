/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Pins v10.0
processor: S32K148
package_id: S32K148_LQFP176
mcu_data: s32sdk_s32k1xx_rtm_403
processor_version: 0.0.0
pin_labels:
- {pin_num: '156', pin_signal: PTE21, label: LED_R, identifier: LED_R}
- {pin_num: '157', pin_signal: PTE22, label: LED_G, identifier: LED_G}
- {pin_num: '84', pin_signal: PTC12, label: SW3, identifier: SW2;SW3}
- {pin_num: '81', pin_signal: PTC13, label: SW4, identifier: SW4}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/**
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.7, External variable could be made static.
 * The external variables will be used in other source files in application code.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 11.4, Conversion between a pointer and integer type.
 * The cast is required to initialize a pointer with an unsigned long define, representing an address.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 11.6, Cast from unsigned int to pointer.
 * The cast is required to initialize a pointer with an unsigned long define, representing an address.
 *
 */

#include "pin_mux.h"

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitPins:
- options: {callFromInitBoot: 'true', coreID: core0}
- pin_list:
  - {pin_num: '156', peripheral: PORTE, signal: 'port, 21', pin_signal: PTE21, direction: OUTPUT, ISF: state_1}
  - {pin_num: '157', peripheral: PORTE, signal: 'port, 22', pin_signal: PTE22, direction: OUTPUT, ISF: state_1}
  - {pin_num: '84', peripheral: PORTC, signal: 'port, 12', pin_signal: PTC12, identifier: SW3, direction: INPUT, ISF: state_1, IRQC: state_1001, DFE: state_1}
  - {pin_num: '81', peripheral: PORTC, signal: 'port, 13', pin_signal: PTC13, direction: INPUT, ISF: state_1, IRQC: state_1001, LK: state_0, DFE: state_1}
  - {pin_num: '144', peripheral: LPUART1, signal: rxd, pin_signal: PTC6}
  - {pin_num: '143', peripheral: LPUART1, signal: txd, pin_signal: PTC7, direction: OUTPUT}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* Generate array of configured pin structures */
pin_settings_config_t g_pin_mux_InitConfigArr0[NUM_OF_CONFIGURED_PINS0] = {
    {
        .base            = PORTC,
        .pinPortIdx      = 12U,
        .pullConfig      = PORT_INTERNAL_PULL_NOT_ENABLED,
        .driveSelect     = PORT_LOW_DRIVE_STRENGTH,
        .passiveFilter   = false,
        .mux             = PORT_MUX_AS_GPIO,
        .pinLock         = false,
        .intConfig       = PORT_INT_RISING_EDGE,
        .clearIntFlag    = true,
        .gpioBase        = PTC,
        .direction       = GPIO_INPUT_DIRECTION,
        .digitalFilter   = true,
        .initValue       = 0U,
    },
    {
        .base            = PORTC,
        .pinPortIdx      = 13U,
        .pullConfig      = PORT_INTERNAL_PULL_NOT_ENABLED,
        .driveSelect     = PORT_LOW_DRIVE_STRENGTH,
        .passiveFilter   = false,
        .mux             = PORT_MUX_AS_GPIO,
        .pinLock         = false,
        .intConfig       = PORT_INT_RISING_EDGE,
        .clearIntFlag    = true,
        .gpioBase        = PTC,
        .direction       = GPIO_INPUT_DIRECTION,
        .digitalFilter   = true,
        .initValue       = 0U,
    },
    {
        .base            = PORTC,
        .pinPortIdx      = 6U,
        .pullConfig      = PORT_INTERNAL_PULL_NOT_ENABLED,
        .driveSelect     = PORT_LOW_DRIVE_STRENGTH,
        .passiveFilter   = false,
        .mux             = PORT_MUX_ALT2,
        .pinLock         = false,
        .intConfig       = PORT_DMA_INT_DISABLED,
        .clearIntFlag    = false,
        .gpioBase        = NULL,
        .digitalFilter   = false,
    },
    {
        .base            = PORTC,
        .pinPortIdx      = 7U,
        .pullConfig      = PORT_INTERNAL_PULL_NOT_ENABLED,
        .driveSelect     = PORT_LOW_DRIVE_STRENGTH,
        .passiveFilter   = false,
        .mux             = PORT_MUX_ALT2,
        .pinLock         = false,
        .intConfig       = PORT_DMA_INT_DISABLED,
        .clearIntFlag    = false,
        .gpioBase        = NULL,
        .digitalFilter   = false,
    },
    {
        .base            = PORTE,
        .pinPortIdx      = 21U,
        .pullConfig      = PORT_INTERNAL_PULL_NOT_ENABLED,
        .driveSelect     = PORT_LOW_DRIVE_STRENGTH,
        .passiveFilter   = false,
        .mux             = PORT_MUX_AS_GPIO,
        .pinLock         = false,
        .intConfig       = PORT_DMA_INT_DISABLED,
        .clearIntFlag    = true,
        .gpioBase        = PTE,
        .direction       = GPIO_OUTPUT_DIRECTION,
        .digitalFilter   = false,
        .initValue       = 0U,
    },
    {
        .base            = PORTE,
        .pinPortIdx      = 22U,
        .pullConfig      = PORT_INTERNAL_PULL_NOT_ENABLED,
        .driveSelect     = PORT_LOW_DRIVE_STRENGTH,
        .passiveFilter   = false,
        .mux             = PORT_MUX_AS_GPIO,
        .pinLock         = false,
        .intConfig       = PORT_DMA_INT_DISABLED,
        .clearIntFlag    = true,
        .gpioBase        = PTE,
        .direction       = GPIO_OUTPUT_DIRECTION,
        .digitalFilter   = false,
        .initValue       = 0U,
    },
};
/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
