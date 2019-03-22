//
// STC15 RPN calculator
//

#include "stc15.h"
#include <stdint.h>
#include "lcd.h"
#include "key.h"
#include "utils.h"

#define FOSC 11583000


static const char KEY_MAP[20] = {
	'c', '<', 'r', 'm',
	'/', '9', '8', '7',
	'*', '6', '5', '4',
	'-', '3', '2', '1',
	'+', '=', '.', '0'
};


uint32_t NewKeyBuf[4];
volatile uint8_t new_key_write_i;
volatile uint8_t new_key_read_i;
volatile uint8_t new_key_empty;
#define INCR_NEW_KEY_I(i) i = (i + 1) & 3

volatile uint8_t SecCount;
void timer0_isr() __interrupt 1 __using 1
{
	static uint8_t count = 0;
	static uint8_t min_count = 0, hour_count = 0;

	uint32_t new_keys;

	//scan keyboard
	KeyScan();
	new_keys = GetNewKeys();
	if (new_keys != 0){
		if (!new_key_empty && (new_key_write_i == new_key_read_i)){
			//do not overwrite keymap currently being processed
			INCR_NEW_KEY_I(new_key_write_i);
		}
		NewKeyBuf[new_key_write_i] = new_keys;
		INCR_NEW_KEY_I(new_key_write_i);
		new_key_empty = 0;
	}

	//track time
	count++;
	if (count == 200){
		count = 0;
		SecCount++;
		if (SecCount == 60){
			SecCount = 0;
			min_count++;
			if (min_count == 60){
				min_count = 0;
				hour_count++;
			}
		}
	}
}


// Call timer0_isr() 200/sec: 5 ms period
// Initialize the timer count so that it overflows after 0.01 sec
// THTL = 0x10000 - FOSC / 200 = 0x10000 - 115830 = 7621 = 0x1DC5
void Timer0Init(void)
{
	// TMOD = 0;    // default: 16-bit auto-reload
	AUXR |= 0x80;   // use undivided SYSclk for timer0
	// Initial values of TL0 and TH0 are stored in hidden reload registers: RL_TL0 and RL_TH0
	TL0 = 0xC5;		// Initial timer value
	TH0 = 0x1D;		// Initial timer value
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
	uint8_t j;
	const uint8_t* keys;
	uint8_t key_i;
	Timer0Init(); // display refresh & switch read
	LCD_Open();
	KeyInit();
	P3_4 = 0; //turn on led backlight

	//set (P3_2) as push pull output
	P3_2 = 1; //latch on
	P3M1 &= ~(0x4);
	P3M0 |= (0x4);

	i = 0;
	j = 0;
	// LOOP
	while (1)
	{
		LCD_GoTo(0,0);
		//keyboard debug
		keys = DebugGetKeys();
		for (key_i = 0; key_i < 5; key_i++){
			LCD_OutNibble(keys[key_i]);
		}
		//turn off?
		if (keys[0] == 8 && keys[4] == 8){
			P3_2 = 0;
		}

		TERMIO_PutChar(',');
		//counter
		if (SecCount == 0){
			LCD_OutString("  ");
		} else if (SecCount < 10){
			TERMIO_PutChar(' ');
			LCD_OutString(u32str(SecCount, buf, 10));
		} else {
			LCD_OutString(u32str(SecCount, buf, 10));
		}

		///new keys
		if (!new_key_empty){
			uint8_t i_key;
			uint32_t new_keys = NewKeyBuf[new_key_read_i];
			INCR_NEW_KEY_I(new_key_read_i);
			if (new_key_read_i == new_key_write_i){
				new_key_empty = 1;
			}

			LCD_GoTo(1,j);
			for (i_key = 0; i_key < 20; i_key++){
				if (new_keys & ((uint32_t) 1 << i_key)){
					TERMIO_PutChar(KEY_MAP[i_key]);
					j++;
					j &= 0x0f;
					break;
				}
			}
		}
	}
}
/* ------------------------------------------------------------------------- */

