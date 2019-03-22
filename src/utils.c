#include <stdint.h>

void _delay_ms(uint8_t ms)
{
    // delay function, tuned for 11.092 MHz clock
    // optimized to assembler
    ms; // keep compiler from complaining?
    __asm;
        ; dpl contains ms param value
    delay$:
        mov	b, #8   ; i
    outer$:
        mov	a, #243    ; j
    inner$:
        djnz acc, inner$
        djnz b, outer$
        djnz dpl, delay$
    __endasm;
}

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

