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

#include <string.h>

#include <FreeRTOS.h>
#include <semphr.h>

#include "sja1105smbevm_console.h"

#include "uart.h"		/* access to PEx generated settings */

#define INST_UART		&uart_instance

/* local data structures for management */
volatile uint32_t console_outctr;
static SemaphoreHandle_t uartMutex; /* map to osif1 layer */



void
console_init(void)
{
    UART_Init(INST_UART, &uart0);

    /* allocate semaphore */
    uartMutex = xSemaphoreCreateMutex();

    console_outctr = 0;
}

int
console_putchar(int c)
{
	uint8_t send_c = c;
	status_t res;

  if (uartMutex != NULL) {
    /* FIXME: use OSIF_MutexLock() and OSIF_MutexUnlock() */
    if ( xSemaphoreTake( uartMutex, ( TickType_t ) 10 ) == pdTRUE ) {
      res = UART_SendDataBlocking(INST_UART, &send_c, 1, 500);
      xSemaphoreGive( uartMutex );
    } else {
      res = STATUS_TIMEOUT;
    }
  } else {
    res = STATUS_ERROR;
  }

  if (res != STATUS_SUCCESS) {
    return -1;  /* EOF is missing or mutex error */
  } else {
	console_outctr++;
    return 1;	/* number of chars written */
  }
}

int
console_puts(const char *str)
{
  status_t res;
  uint32_t len = strlen(str);

  if (uartMutex != NULL) {
    /* FIXME: use OSIF_MutexLock() and OSIF_MutexUnlock() */
    if ( xSemaphoreTake( uartMutex, ( TickType_t ) 10 ) == pdTRUE ) {
      res = UART_SendDataBlocking(INST_UART, (const uint8_t*) str, len, 500);
      xSemaphoreGive( uartMutex );
    } else {
      res = STATUS_TIMEOUT;
    }
  } else {
    res = STATUS_ERROR;
  }

  if (res != STATUS_SUCCESS) {
    return -1;  /* EOF is missing */
  } else {
    console_outctr += len;
    return len;	/* number of chars written */
  }
}

int
console_putint(int num, uint8_t digitsToUse)
{
  uint32_t ref;
  uint32_t rest;
  uint8_t digitsNeeded;
  uint8_t digitToPrint;
  bool isNegative = false;
  uint8_t charsSent = 0;

  if (num < 0) {
    isNegative = true;
    digitsToUse--;
    rest = -num;
  } else {
    rest = num;
  }

  ref = 1;
  digitsNeeded = 0;
  if (rest != 0) while (ref <= rest) {
    ref *= 10;
    digitsNeeded++;
  } else {
    digitsNeeded++;
	}


  while (digitsToUse > digitsNeeded) {
    console_putchar(' ');
    charsSent++;
    digitsToUse--;
  }

  if (isNegative) {
    console_putchar('-');
    charsSent++;
  }

  if (rest != 0) {
    while (ref > 1) {
      ref /= 10;
      digitToPrint = rest/ref;		/* always 0..9 */
      console_putchar('0'+ digitToPrint);
      charsSent++;
      rest -= (digitToPrint * ref);
    }
  } else {
    console_putchar('0'); charsSent++;
  }

  return charsSent;
}

static const uint8_t hexlut[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

int
console_puthex(uint32_t number)
{
uint8_t localBuf[24];
uint8_t *bPtr = &localBuf[sizeof(localBuf)/sizeof(*localBuf)-1];
uint8_t len = 8;

   console_puts("0x");
   *bPtr-- = '\0';

   while (len-- > 0) {
	   *bPtr-- = hexlut[number & 0xf];
	   number >>= 4;
   }

   console_puts((char *) (bPtr+1));

   return strlen((char *) (bPtr+1)) + 2;
}

int
console_getchar(void)
{
uint8_t c;
status_t status;

  if (( status = UART_ReceiveDataBlocking(INST_UART, &c, 1, 2)) == STATUS_SUCCESS) {
    return (int) c;
  } else {
    return CONSOLE_EOF;
  }
}

