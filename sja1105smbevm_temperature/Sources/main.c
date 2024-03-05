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

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "Cpu.h"
#include "FreeRTOS.h"
#include "clockman.h"
#include "dmacontroller1.h"
#include "uart.h"
#include "pin_mux.h"

#include "sja1105smbevm_console.h"
#include "sja1105smbevm_gpio.h"

#include "task.h"
#include "app.h"

#include "enet_app.h"
#include "enet_callbacks.h"
#include "can_app.h"

/* SW version number definitions */
/*
 * version number scheme
 * 31..24 - reserved; currently 0x80 for legacy purposes
 * 23..16 - product type
 * 15..12 - 4bit major version
 *  8..11 - 4bit mid version
 *  0.. 7 - 8bit minor version
 */
#define VERSION_PRODUCTTYPE_SHIFTWIDTH      16
#define VERSION_PRODUCTTYPE_MASK            0xff
#define VERSION_MAJOR_SHIFTWIDTH            12
#define VERSION_MAJOR_MASK                  0xf
#define VERSION_MID_SHIFTWIDTH              8
#define VERSION_MID_MASK                    0xf
#define VERSION_MINOR_SHIFTWIDTH            0
#define VERSION_MINOR_MASK                  0xff

#define SJA1105SMBEVM                    0x10

#define productType     SJA1105SMBEVM
#define majorVersion    0
#define midVersion      3
#define minorVersion    0x10

#define VERSION_WORD ((1UL << 31) |\
          ((((uint32_t) productType) & VERSION_PRODUCTTYPE_MASK) << VERSION_PRODUCTTYPE_SHIFTWIDTH) |\
          ((((uint32_t) majorVersion) & VERSION_MAJOR_MASK) << VERSION_MAJOR_SHIFTWIDTH) |\
          ((((uint32_t) midVersion) & VERSION_MID_MASK) << VERSION_MID_SHIFTWIDTH) |\
          ((((uint32_t) minorVersion) & VERSION_MINOR_MASK) << VERSION_MINOR_SHIFTWIDTH))

/* SW version number definitions end */

/* definitions for the tasks created by main() */
/* maxPriority in PEX is 16 */
#define main_TASK_PRIORITY          ( 14 )
#define enet_TASK_PRIORITY           ( 12 )
#define can_TASK_PRIORITY           (  5 )
#define button_TASK_PRIORITY           ( 9 )

/* TaskHandles for different tasks */
static TaskHandle_t canTaskHandle, enetTaskHandle, mainTaskHandle, buttonTaskHandle;

static uint8_t rotary_position;	/* local copy of the rotary position at startup - for detecting changes during runtime */
uint8_t verboseFlag = 0;	    /* may be changed by debugger */
uint16_t aliveLedTime = 600U;	/* time in msec for aliveLed blink frequency */

volatile int exit_code = 0;

/* operation mode definitions of the Firmware, depending on the rotary setting */
#define TASKFLAGS_ENET_TASK	    (1<<0)
#define TASKFLAGS_CAN_TASK      (1<<1)

typedef struct {
  uint8_t                       taskFlags;		/* which task to start */
  SJA1105SMBEVM_ENETTASK_DEFS	enetTaskDefs;		/* which static config, which ENET task properties */
  const char                   *descString;		/* verbal description of this rotary mode */
} SJA1105SMBEVM_CONFIG_DEFS;

static const
SJA1105SMBEVM_CONFIG_DEFS rotary_defs[] = {
   /*  0 = silent;                            */
   [0] = {(0),
    {255, (0), 0},
    "(silent)"
   },
   /*  1 = CAN + ENET periodic traffic;       */
   [1] = {(TASKFLAGS_ENET_TASK | TASKFLAGS_CAN_TASK),
    {0, ENETTASK_FEATURE_SENDPKT, 0},
    "(CAN + ENET periodic traffic)"
   },
   /*  2 = ENET only;                         */
   [2] = {(TASKFLAGS_ENET_TASK),
    {0, ENETTASK_FEATURE_SENDPKT, 0},
    "(ENET only - no CAN)"
   },
   /*  3 = CAN only.                          */
   [3] = {(TASKFLAGS_CAN_TASK),
    {255, 0, 0},
    "(CAN only - no ENET)"
   },
   /*  4 = ENET + TC10 + simple wakeup forwarding matrix;             */
   [4] = {(TASKFLAGS_ENET_TASK),
   {0, (ENETTASK_FEATURE_TC10), 0},
    "(ENET + TC10 support + simple wup forw matrix)"
   },
   /*  5 = ENET + TC10 + complex wakeup forwarding matrix;            */
   [5] = {(TASKFLAGS_ENET_TASK),
   {0, (ENETTASK_FEATURE_TC10), 1},
    "(ENET + TC10 support + complex wup forw matrix)"
   },
   /*  6 = spare */
   [6] = {(0),
    {255, (0), 0},
	"(#6 default - no special meaning)"
   },
   /*  7 = spare */
   [7] = {(0),
    {255, (0), 0},
	"(#7 default - no special meaning)"
   },
   /*  8 = spare */
   [8] = {(0),
    {255, (0), 0},
	"(#8 default - no special meaning)"
   },
   /*  9 = spare */
   [9] = {(0),
    {255, (0), 0},
	"(#9 default - no special meaning)"
   },
   /*  10 = ENET + TC10 + simple wakeup forwarding matrix + SEND_PACKETS */
   [10] = {(TASKFLAGS_ENET_TASK),
   {0, (ENETTASK_FEATURE_TC10 | ENETTASK_FEATURE_SENDPKT), 0},
    "(ENET + TC10 support + simple wup forw matrix + SEND_PKTs )"
   },
#if 0
   /* 10 = spare */
   [10] = {(0),
    {255, (0), 0},
   "(#10 default - no special meaning)"
   },
#endif
   /* 11 = spare */
   [11] = {(0),
    {255, (0), 0},
	"(#11 default - no special meaning)"
   },
   /* 12 = spare */
   [12] = {(0),
    {255, (0), 0},
	"(#12 default - no special meaning)"
   },
   /* 13 = spare */
   [13] = {(0),
    {255, (0), 0},
	"(#13 default - no special meaning)"
   },
   /* 14 = spare */
   [14] = {(0),
    {255, (0), 0},
	"(#14 default - no special meaning)"
   },
   /* 15 = spare */
   [15] = {(0),
    {255, (0), 0},
	"(#15 default - no special meaning)"
   },
};

static const size_t rotary_defs_len = sizeof(rotary_defs)/sizeof(*rotary_defs);
static const SJA1105SMBEVM_CONFIG_DEFS * rotary_defsPtr;

/*
 * emergency callback in case the scheduler detects a stack overflow
 * for production code this should be removed and unconfigured in PEx
 * (FreeRTOS -> Component Inspector -> All -> Hook function ->
 *    Check for stack overflow -> Disabled)
 */
void
vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
  console_puts("Stack overflow in task \"");
  console_puts(pcTaskName);
  console_puts("\n\r");

  for (;;) {};
}

/**
 * Creates LED start-up blink pattern when board is powered
 */
static void
main_startupBlinkPattern(void)
{
  int i;

#define LED_DELAY   50
  /* blink the LEDs */
  for (i = 0; i < 3; i++) {
    GPIO_SETLED(LED_A1, true);
    vTaskDelay( DELAY_MS(LED_DELAY) );
    GPIO_SETLED(LED_B2, true);
    vTaskDelay( DELAY_MS(LED_DELAY) );
    GPIO_SETLED(LED_B3, true);
    vTaskDelay( DELAY_MS(LED_DELAY) );
    GPIO_SETLED(LED_B0, true);
    vTaskDelay( DELAY_MS(LED_DELAY) );
    GPIO_SETLED(LED_B1, true);
    vTaskDelay( DELAY_MS(LED_DELAY) );

    GPIO_SETLED(LED_A1, false);
    vTaskDelay( DELAY_MS(LED_DELAY) );
    GPIO_SETLED(LED_B2, false);
    vTaskDelay( DELAY_MS(LED_DELAY) );
    GPIO_SETLED(LED_B3, false);
    vTaskDelay( DELAY_MS(LED_DELAY) );
    GPIO_SETLED(LED_B0, false);
    vTaskDelay( DELAY_MS(LED_DELAY) );
    GPIO_SETLED(LED_B1, false);
    vTaskDelay( DELAY_MS(LED_DELAY) );
  }
}

static void
check_for_rcvd_enet_pkts(void)
{
static uint8_t dbglog_ctr_copy;
static uint8_t swlog_ctr_copy;

  if (swlog_ctr_copy != enet_sw_rcvlog.ctr) {
    console_puts("sw  rcv from "); console_puthex(enet_sw_rcvlog.src);
    console_puts(" to "); console_puthex(enet_sw_rcvlog.dst);
    console_puts(" type "); console_puthex(enet_sw_rcvlog.type);
    console_puts(" pkt "); console_puthex(enet_sw_rcvlog.pktnumber);
    console_puts("\n\r");

    swlog_ctr_copy = enet_sw_rcvlog.ctr;
  }

  if (dbglog_ctr_copy != enet_dbg_rcvlog.ctr) {
    console_puts("dbg rcv from "); console_puthex(enet_dbg_rcvlog.src);
    console_puts(" to "); console_puthex(enet_dbg_rcvlog.dst);
    console_puts(" type "); console_puthex(enet_dbg_rcvlog.type);
    console_puts(" pkt "); console_puthex(enet_dbg_rcvlog.pktnumber);
    console_puts("\n\r");

    dbglog_ctr_copy = enet_dbg_rcvlog.ctr;
  }
}

static void
button_task( void *pvParameters )
{
  uint32_t mainLoopCtr = 0;
  bool button0;
  bool button1;
  bool t_button0;
  bool t_button1;

  button0 = GPIO_GETBUTTONSTATUS(0);
  button1 = GPIO_GETBUTTONSTATUS(1);

  for (;;) {

    mainLoopCtr++;

    t_button0 = GPIO_GETBUTTONSTATUS(0);
    t_button1 = GPIO_GETBUTTONSTATUS(1);
    if (t_button0 != button0) {
      enet_sleep_button_handler(t_button0);
      button0 = t_button0;
    }

    if (t_button1 != button1) {
      enet_diag_button_handler(t_button1);
      button1 = t_button1;
    }

    vTaskDelay( DELAY_MS(5) );
  }
}

/*
 * main task jobs:
 * - startup greeter and startup blink pattern,
 * - finalize BSP initialisation where scheduling is necessary
 * - blink alive LED
 * - check for printing key items of received ethernet packets
 */
static void
main_task ( void *pvParameters )
{
  uint32_t mainLoopCtr = 0;
  SJA1105SMBEVM_CONFIG_DEFS *boardConfigDefsPtr = (SJA1105SMBEVM_CONFIG_DEFS *) pvParameters;

  console_puts("\r\n\n\nWelcome to SJA1105SMBEVM\n\rFW Version ");
  console_puthex(VERSION_WORD);

#if defined SJA1105P_REFBOARD_REVA
  console_puts(" for RevA boards");
#elif defined SJA1105P_REFBOARD_REVB
  console_puts(" for RevB boards");
#endif

  console_puts("\n\rRotary position ");
  console_puthex(rotary_position);
  console_puts(" ");
  console_puts(boardConfigDefsPtr->descString);
  console_puts("\n\n\r");

  main_startupBlinkPattern();

  for (;;) {

    mainLoopCtr++;

    GPIO_SETLED(LED_ALIVE, true);
    vTaskDelay( DELAY_MS(aliveLedTime) );
    GPIO_SETLED(LED_ALIVE, false);
    vTaskDelay( DELAY_MS(aliveLedTime) );

    if (boardConfigDefsPtr->enetTaskDefs.enettask_featureFlags & ENETTASK_FEATURE_SENDPKT) {
      check_for_rcvd_enet_pkts();
    }

    if (rotary_position != GPIO_GETCONFIGENCODER()) {
      /* reset the board */
      SystemSoftwareReset();
    }
  }
}

int
main(void)
{
  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  #ifdef PEX_RTOS_INIT
    PEX_RTOS_INIT();                   /* Initialization of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of Processor Expert internal initialization.                    ***/

  CLOCK_SYS_Init(g_clockManConfigsArr,   CLOCK_MANAGER_CONFIG_CNT,
  		         g_clockManCallbacksArr, CLOCK_MANAGER_CALLBACK_CNT);
  CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_AGREEMENT);

  PINS_DRV_Init(NUM_OF_CONFIGURED_PINS, g_pin_mux_InitConfigArr);

  console_init();

  rotary_position = GPIO_GETCONFIGENCODER();
  if (rotary_position >= rotary_defs_len) {
    rotary_defsPtr = &rotary_defs[0];
  } else {
    rotary_defsPtr = &rotary_defs[rotary_position];
  }

  /* Depending on the settings of the rotary encoder create different tasks */
  if (rotary_defsPtr->taskFlags & TASKFLAGS_ENET_TASK) {
    /* parameter for enet_task is part of the rotary definition */
    xTaskCreate( enet_task, ( const char * const ) "EthTask",  4*configMINIMAL_STACK_SIZE,
                 (void*) &rotary_defsPtr->enetTaskDefs, enet_TASK_PRIORITY, &enetTaskHandle);
  }

  if (rotary_defsPtr->taskFlags & TASKFLAGS_CAN_TASK) {
    xTaskCreate( can_task, ( const char * const ) "CanTask", 5*configMINIMAL_STACK_SIZE,
                 (void*)0, can_TASK_PRIORITY, &canTaskHandle);
  }

  xTaskCreate( main_task, ( const char * const ) "MainTask", 8*configMINIMAL_STACK_SIZE,
		       (void*) rotary_defsPtr, main_TASK_PRIORITY, &mainTaskHandle);
  xTaskCreate( button_task, ( const char * const ) "ButtonTask", configMINIMAL_STACK_SIZE,
		       (void*)0, button_TASK_PRIORITY, &buttonTaskHandle);

  /* Start the scheduler. */
  vTaskStartScheduler();

  /* cannot place any code here, because it is not executed, since the tasks have higher priority */
  /* therefore, main_task is indispensable */

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;) {
    if(exit_code != 0) {
      break;
    }
  }
  return exit_code;
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
}
