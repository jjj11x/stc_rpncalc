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
#ifndef SRC_STACK_DEBUG_H_
#define SRC_STACK_DEBUG_H_

#include <stdint.h>

#if !defined(DESKTOP)
#include "stc15.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

// P3_4 is connected to the cathode and the only pin that is not toggled normally
// unfortunately, the hardware uart cannot be mapped to TX = P3_4

#if defined(STACK_DEBUG)
void stack_debug_init(void);
void stack_debug(uint8_t marker);
void stack_debug_write(uint8_t value) __naked;
#else
#define stack_debug_init()
#define stack_debug(marker)
#endif

#if defined(STACK_DEBUG) && defined(SHOW_STACK)
extern __xdata uint8_t stack_max;
#endif

#if defined(DESKTOP) || defined(STACK_DEBUG)
#define backlight_on()
#define backlight_off()
#else
inline void backlight_on(void) {
	P3_4 = 0;
}
inline void backlight_off(void) {
	P3_4 = 1;
}
#endif

#ifdef __cplusplus
}
#endif

#endif
