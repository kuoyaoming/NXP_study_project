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

#ifndef SJA1105SMBEVM_INC_SJA1105SMBEVM_CONSOLE_H_
#define SJA1105SMBEVM_INC_SJA1105SMBEVM_CONSOLE_H_

#include <stdint.h>

//      1234567890123456789012345678901234567890____    //!<

#define CONSOLE_EOF    ((int) -1)


/**
 * \brief Initializes the console layer and allocates all resources
 */
void
console_init(void);


/*
 * \brief Output a single character
 */
int
console_putchar(int c);

/*
 * \brief Output a NULL-terminated string
 */
int
console_puts(const char *str);

/*
 * \brief Output a integer in decimal form; for pretty printing fill with leading spaces for right justified columns
 */
int
console_putint(const int num, uint8_t digitsToUse);

/**
 * \brief Output a 32bit unsigned word in hex
 */
int
console_puthex(uint32_t number);

/**
 * \brief fetch a char from input; if nothing available or error CONSOLE_EOF is returned
 */
int
console_getchar(void);


#endif /* SJA1105SMBEVM_INC_SJA1105SMBEVM_CONSOLE_H_ */
