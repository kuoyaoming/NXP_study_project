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

#ifndef SJA1105SMBEVM_INC_SJA1105SMBEVM_GPIO_H_
#define SJA1105SMBEVM_INC_SJA1105SMBEVM_GPIO_H_

/*
 * this file belongs to the adaption layer specific to the target board
 * it maps generic services from the SDK device drivers to services for the application
 */

#include <stdint.h>
#include <stdbool.h>
#include <MPC5748G_features.h>

//      1234567890123456789012345678901234567890____    //!<

/*
 * general remark to port definitions
 * for Calypso there are two access methods to a part:
 * - port configuration (IO cell setup, e.g. pullup, schmitt, etc) via PORTx
 * - port line setting (read/write level) via PTx
 * this results in different #defines for ports which need a runtime fixup of PEx's setup
 */
#define LED_GPIO                PTG
#define LED_PORT				PORTG
#define LED_A1                  (1UL << 2U)     /* pin PG[2] - D2   GP1 */
#define LED_B0                  (1UL << 3U)     /* pin PG[3] - D4   GP2 */
#define LED_B1                  (1UL << 5U)     /* pin PG[5] - D5   GP3 */
#define LED_B2                  (1UL << 6U)     /* pin PG[6] - D15  GP4 */
#define LED_B3                  (1UL << 7U)     /* pin PG[7] - D16  GP5 */
#define LED_ALIVE               (1UL << 4U)     /* pin PG[4] - D3   ALIVE */
#define LED_GP                  (1UL << 8U)     /* pin PG[8] - D17  GP */

#if defined SJA1105P_REFBOARD_REVA
#define BUTTON_GPIO             PTK
#define BUTTON_PORT             PORTK
#define BUTTON_1_NR             (2U)            /* PK[2] */
#define BUTTON_2_NR             (1U)            /* PK[1] */
#define BUTTON_1                (1UL <<  BUTTON_1_NR)
#define BUTTON_2                (1UL <<  BUTTON_2_NR)
#elif defined SJA1105P_REFBOARD_REVB
#define BUTTON_GPIO             PTE
#define BUTTON_1_NR             (7U)            /* "BUTTON 0" = SW3 = GP162 = PE[7] */
#define BUTTON_2_NR             (6U)            /* "BUTTON 1" = SW4 = GP161 = PE[6] */
#define BUTTON_1                (1UL <<  BUTTON_1_NR)
#define BUTTON_2                (1UL <<  BUTTON_2_NR)
#else
#error must define either RevA or RevB
#endif

#define ENCODER_GPIO            PTD
#define ENCODER_MASK            (0xf)
#define ENCODER_SHIFT           (0)

#define PG_GPIO                 PTI
#define PG_2V5_MASK             (1UL <<  0U)    /* PI[0] */
#define PG_1V8_MASK             (1UL <<  1U)    /* PI[1] */
#define PG_1V2_MASK             (1UL <<  2U)    /* PI[2] */
#define PG_3V3_MASK             (1UL <<  3U)    /* PI[3] */
#define PG_5V0_MASK             (1UL <<  4U)    /* PI[4] */

#define RESET_GPIO              PTI
#define RESET_PORT              PORTI
#define RESET_TJA_PHY_0_NR      (9U)            /* PI[9] */
#define RESET_TJA_PHY_S_NR      (10U)           /* PI[10] */
#define RESET_PRI_SJA1105       (1UL <<  5U)    /* PI[5] */
#define RESET_SEC_SJA1105       (1UL <<  6U)    /* PI[6] */
#define RESET_GIG_PHY_1         (1UL <<  7U)    /* PI[7] */
#define RESET_GIG_PHY_2         (1UL <<  8U)    /* PI[8] */
#define RESET_TJA_PHY_0         (1UL << RESET_TJA_PHY_0_NR)
#define RESET_TJA_PHY_S         (1UL << RESET_TJA_PHY_S_NR)
#define RESET_FE_PHY            (1UL << 14U)    /* PI[14] */

#define VREGINH_GPIO            PTA
#define VREGINH_MASK            (1UL << 15U)    /* PA[15] */

#if defined SJA1105P_REFBOARD_REVA
#define ENAB_GPIO               PTD
#define ENAB_PORT               PORTD
#define ENAB_TJAS_PHY_3_NR      (4U)            /* PD[4] MSCR52 */
#define ENAB_TJA_PHY_D1_NR      (5U)            /* PD[5] MSCR53 */
#define ENAB_TJA_PHY_D2_NR      (6U)            /* PD[6] MSCR54 */
#define ENAB_TJAS_PHY_3         (1UL <<  ENAB_TJAS_PHY_3_NR)
#define ENAB_TJA_PHY_D1         (1UL <<  ENAB_TJA_PHY_D1_NR)
#define ENAB_TJA_PHY_D2         (1UL <<  ENAB_TJA_PHY_D2_NR)
#elif defined SJA1105P_REFBOARD_REVB
#define ENAB_GPIO               PTJ
#define ENAB_PORT               PORTJ
#define ENAB_TJAS_PHY_3_NR      (4U)            /* PJ[4] */
#define ENAB_TJA_PHY_D1_NR      (0U)            /* PJ[0] */
#define ENAB_TJA_PHY_D2_NR      (1U)            /* PJ[1] */
#define ENAB_TJAS_PHY_3         (1UL <<  ENAB_TJAS_PHY_3_NR)
#define ENAB_TJA_PHY_D1         (1UL <<  ENAB_TJA_PHY_D1_NR)
#define ENAB_TJA_PHY_D2         (1UL <<  ENAB_TJA_PHY_D2_NR)
#endif

#if defined SJA1105P_REFBOARD_REVA
#define PHYINT_GPIO             PTG
#define PHYINT_PIN              (1UL << 9U)     /* pin PG[9] */
#elif defined SJA1105P_REFBOARD_REVB
#define PHYINT_GPIO             PTC
#define PHYINT_PIN              (1UL << 2U)     /* pin PC[2] */
#else
#error must define either RevA or RevB
#endif

/**
 * \brief Gets the status of a button
 * \return True if button is pressed
 */
#define GPIO_GETBUTTONSTATUS(buttonNo) \
 ((buttonNo == 0) ? \
    ((PINS_DRV_ReadPins(BUTTON_GPIO) & BUTTON_1) == 0) :  \
    ((PINS_DRV_ReadPins(BUTTON_GPIO) & BUTTON_2) == 0))

/**
 * \brief Gets the value of hex encoder
 * \return The value of hex encoder
 */
#define GPIO_GETCONFIGENCODER() \
  ((PINS_DRV_ReadPins(ENCODER_GPIO) & ENCODER_MASK) >> ENCODER_SHIFT)

/**
 * \brief Sets the state of a specific LED.
 */
#define GPIO_SETLED(led, set_to_on) \
  { if (set_to_on) { PINS_DRV_ClearPins(LED_GPIO, led);\
    } else { PINS_DRV_SetPins(LED_GPIO, led); } } while (0)

#define GPIO_GETPGSTATUS(rail) \
  (!!((rail & PINS_DRV_ReadPins(PG_GPIO)) != 0))

/**
 * \brief Controls the product reset pin. Presuming all are active-low
 * \parameter pins_channel_type_t                             line,                               //!< what reset line to operate on
 * \parameter bool                                            reset                               //!< on true, the reset line will be activated - reset polarity is handled
 */
#define GPIO_SETPRODUCTRESET(line,reset) \
  { if (reset) { PINS_DRV_ClearPins(RESET_GPIO, line);\
    } else { PINS_DRV_SetPins(RESET_GPIO, line); } } while (0)


/**
 * \brief Controls the VregInh pin
 * \parameter bool                                            do_assert                          //!< true means to temporarily veto going to sleep
 */
#define GPIO_SETVREGINH(do_assert) \
  { if (do_assert) { PINS_DRV_SetPins(VREGINH_GPIO, VREGINH_MASK); \
    } else { PINS_DRV_ClearPins(VREGINH_GPIO, VREGINH_MASK); } } while (0)

/**
 * \brief Sets the value for the PHYEN pins.
 * \parameter pins_channel_type_t                             line,                             //!< which EN pin is meant
 * \parameter bool                                            do_enable						    //!< true for enable; automatic polarity handling
 */
#if defined SJA1105P_REFBOARD_REVA
#define GPIO_SETPHYENABLES(line, do_enable) \
  { if (do_enable) { PINS_DRV_SetPullSel(ENAB_PORT, line, PORT_INTERNAL_PULL_UP_ENABLED); \
    } else { PINS_DRV_SetPullSel(ENAB_PORT, line, PORT_INTERNAL_PULL_DOWN_ENABLED); } } while (0)
#elif defined SJA1105P_REFBOARD_REVB
#define GPIO_SETPHYENABLES(line, do_enable) \
  { if (do_enable) { PINS_DRV_SetPins(ENAB_GPIO, line); \
    } else { PINS_DRV_ClearPins(ENAB_GPIO, line); } } while (0)
#endif

/**
 * \brief Get input value of the ENET_PHY_INT line - true == low(asserted)
 */
#define GPIO_GETPHYIRQ() \
  (!!((PHYINT_PIN & PINS_DRV_ReadPins(PHYINT_GPIO)) == 0))

#endif // SJA1105SMBEVM_INC_SJA1105SMBEVM_GPIO_H_
