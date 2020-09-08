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

#include <stdint.h>
#include "stack_debug.h"

#if defined(STACK_DEBUG)
extern uint16_t _SP;

inline static void HIGH() {
	P3_4 = 1;
}

inline static void LOW() {
	P3_4 = 1;
}

void stack_debug_init() {
	P3M1 &= ~(0x10);
	P3M0 |= 0x10;
	LOW();
	stack_debug_write(0x55);
	stack_debug_write(0xAA);
	stack_debug_write(0x55);
	stack_debug_write(0xAA);
	stack_debug_write(0x80);
	stack_debug_write(0x10);
	stack_debug_write(0x08);
	stack_debug_write(0x01);
}

/*
  for frequency of 12.000 MHz (12.005 adjusted) this gives a rate of 1'190'000 Baud
  negative polarity, MSB
 */

void stack_debug_write(uint8_t value) __naked {
	value;
__asm
	.macro HIGH
	setb _P3_4
	.endm

	.macro LOW
	clr _P3_4
	.endm

	mov   dph,r0
	mov   a,dpl
	mov   r0,#8
	; START
	HIGH                    ; 1
	nop                     ; 1
	nop                     ; 1
	nop                     ; 1
stack_debug_1$:
	rlc   a                 ; 1
	jc    stack_debug_2$    ; 3
	HIGH                    ; 1
	djnz  r0,stack_debug_1$ ; 4
	sjmp  stack_debug_3$    ; 3
stack_debug_2$:
	LOW                     ; 1
	djnz  r0,stack_debug_1$ ; 4
	sjmp  stack_debug_3$    ; 3
stack_debug_3$:
	nop                     ; 1
        ; STOP
	LOW                     ; 1
	nop                     ; 1
	nop                     ; 1

	mov   r0,dph            ; 3
	ret                     ; 4
__endasm;
}

void stack_debug(uint8_t marker) {
	stack_debug_write(marker);
	stack_debug_write(SP);
}
#endif // !defined(DESKTOP)
