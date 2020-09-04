// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

/*
 * utils.h
 *
 *  Created on: Mar 10, 2019
 */

#ifndef SRC_UTILS_H_
#define SRC_UTILS_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void _delay_ms(uint8_t ms);

#define ACCURATE_DELAY_US
#ifdef ACCURATE_DELAY_US
void _delay_us(uint8_t us);
#else
#define _delay_us(x) _delay_ms(1)
#endif

void backlight_off(void);



#ifdef __linux__
#define DESKTOP
#elif _WIN32
#define DESKTOP
#elif __APPLE__
#define DESKTOP
#endif

#if defined(DESKTOP) || defined(IS_ECLIPSE)
#include <stdbool.h>
char* u32str(uint32_t x, char* buf, uint8_t base);
#define __bit bool
#define __code
#define __xdata
#define __idata
#define __sfr
#define __at uint8_t*
#define SDCC_ISR(isr, reg)
#define __using(x)
#define BACKLIGHT_ON()
#define TURN_OFF()
#else
#define SDCC_ISR(isr, reg) __interrupt (isr) __using (reg)
#define BACKLIGHT_ON()  P3_4 = 0
#define TURN_OFF() P3_2 = 0
#endif


#ifdef __cplusplus
}
#endif

#endif /* SRC_UTILS_H_ */

