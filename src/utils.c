#include <stdint.h>
#include "utils.h"
#ifndef DESKTOP
#include "stc15.h"
#endif

#ifdef DESKTOP
void _delay_ms(uint8_t ms){
	//TODO:
	(void) ms;
}
#ifdef ACCURATE_DELAY_US
void _delay_us(uint8_t us)
{
	//TODO:
	(void) us;
}
#endif
void backlight_off(void){ }
#else //!DESKTOP
void _delay_ms(uint8_t ms)
{
	// delay function, tuned for 11.583 MHz clock
	// optimized to assembler
	ms; // keep compiler from complaining
	__asm;
		; dpl contains ms param value
	ms_delay$:
		mov	b, #10   ; i, 2 clocks
	ms_outer$:
		mov	a, #230    ; j, 2 clocks
	ms_inner$:
		djnz acc, ms_inner$  ;4 clocks
		djnz b, ms_outer$    ;4 clocks
		djnz dpl, ms_delay$  ;4 clocks
	__endasm;
}

#ifdef ACCURATE_DELAY_US
void _delay_us(uint8_t us)
{
	// delay function, tuned for 11.583 MHz clock
	// optimized to assembler
	us; // keep compiler from complaining
	__asm;
		; dpl contains us param value
	us_delay$:
		nop
		nop
		nop
		nop
		nop
		nop
		djnz dpl, us_delay$
	__endasm;
}
#endif

void backlight_off(void){
	P3_4 = 1;
}

#endif //ifdef desktop

#ifdef DESKTOP
char* u32str(uint32_t x, char* buf, uint8_t base)
{
	int i = 0, j;
	//corner case
	if (x == 0){
		buf[i] = '0';
		i++;
	}
	//get reversed string
	while(x > 0){
		buf[i] = (x % base) + '0';
		x /= base;
		i++;
	}
	//add terminator
	buf[i] = '\0';
	i--; //back to last digit
	//reverse string
	for (j = 0; j < i; j++, i--){
		char tmp;
		tmp = buf[j];
		buf[j] = buf[i];
		buf[i] = tmp;
	}

	return buf;
}
#endif

