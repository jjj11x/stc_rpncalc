//
// STC15 RPN calculator
//

#include "stc15.h"
#include <stdint.h>
#include "lcd.h"
#include "key.h"
#include "utils.h"

#define FOSC 11059200

// clear wdt
#define WDT_CLEAR() (WDT_CONTR |= 1 << 4)
void timer0_isr() __interrupt 1 __using 1
{
//	P3_1 ^= 1;
}


// Call timer0_isr() 10000/sec: 0.0001 sec
// Initialize the timer count so that it overflows after 0.0001 sec
// THTL = 0x10000 - FOSC / 12 / 10000 = 0x10000 - 92.16 = 65444 = 0xFFA4
void Timer0Init(void)		//100us @ 11.0592MHz
{
	// TMOD = 0;	// default: 16-bit auto-reload
	// AUXR = 0;	// default: traditional 8051 timer frequency of FOSC / 12
	// Initial values of TL0 and TH0 are stored in hidden reload registers: RL_TL0 and RL_TH0
	TL0 = 0xA4;		// Initial timer value
	TH0 = 0xFF;		// Initial timer value
	TF0 = 0;		// Clear overflow flag
	TR0 = 1;		// Timer0 start run
	ET0 = 1;		// Enable timer0 interrupt
	EA  = 1;		// Enable global interrupt
}


char buf[17];

/*********************************************/
int main()
{
	uint32_t i;
	uint8_t* keys;
	uint8_t key_i;
	Timer0Init(); // display refresh & switch read
	LCD_Open();
	KeyInit();
	P3_4 = 0; //turn on led backlight
	LCD_OutString("Hello world !!!!");
	LCD_GoTo(1,0);
	LCD_OutString(".......");

	i = 0;
	// LOOP
	while (1)
	{
		LCD_GoTo(0,0);
		//scan keyboard
		KeyScan();
		keys = GetKeys();
		for (key_i = 0; key_i < 5; key_i++){
			LCD_OutNibble(keys[key_i]);
		}

		//counter
		LCD_GoTo(1,7);
		LCD_OutString(u32str(i, buf, 10));
		i++;

		WDT_CLEAR();
	}
}
/* ------------------------------------------------------------------------- */

