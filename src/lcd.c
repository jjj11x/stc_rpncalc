//#define DEBUG

#include "stc15.h"
#include "utils.h"
#include "lcd.h"


#define LCDINC 2
#define LCDDEC 0
#define LCDSHIFT 1
#define LCDNOSHIFT 0
#define LCDCURSOR 2
#define LCDNOCURSOR 0
#define LCDBLINK 1
#define LCDNOBLINK 0
#define LCDSCROLL 8
#define LCDNOSCROLL 0
#define LCDLEFT 0
#define LCDRIGHT 4
#define LCD2LINE 8
#define LCD1LINE 0
#define LCD10DOT 4
#define LCD7DOT 0

#define CR 13 // \r
#define TAB 9 // \n
#define LF 10 // \r



static int row, col;

#define CLEAR_BIT(port, bit) (port &= ~(_BV(bit)))
#define CLEAR_BITS(port, bits) (port &= ~(bits))
#define SET_BIT(port, bit) (port |= _BV(bit))

//#define DISABLE_INTERRUPTS() __critical{
//#define ENABLE_INTERRUPTS()  }
#define DISABLE_INTERRUPTS() {
#define ENABLE_INTERRUPTS()  }

#define LCD_E P3_5
#define LCD_RW P3_6
#define LCD_RS P3_7
#define LCD_BUSY P2_7 //LCD D7

static void outCsrBlindNibble(unsigned char command);
static void outCsr(unsigned char command);
static char readBusy(void);
static void wait_busy(void);
static void to_row(unsigned char row_to);
static void LCD_OutChar(unsigned char c);

static void outCsrBlindNibble(unsigned char command) {
//	CLEAR_BITS(PORTC, _BV(LCD_E) | _BV(LCD_RS) | _BV(LCD_RW));
	P3 &= ~(0x7 << 5); //clear LCD_E, LCD_RS, LCD_RW
	_delay_us(50);
	P2 = ((command & 0xf) << 4);
	//CLEAR_BIT(PORTC, LCD_E); //E = 0
	//CLEAR_BIT(PORTC, LCD_RS); //control
	//CLEAR_BIT(PORTC, LCD_RW); //write
	_delay_us(100);
	LCD_E = 1;
	_delay_us(100);
	LCD_E = 0;
}

static void outCsr(unsigned char command) {
	DISABLE_INTERRUPTS();
	outCsrBlindNibble(command >> 4); // ms nibble, E=0, RS=0
	outCsrBlindNibble(command); // ls nibble, E=0, RS=0
	ENABLE_INTERRUPTS();
	wait_busy();
}

#define SET_BUSY_IN()  do {P2M1 |= (1<<7);  P2M0 &= ~(1<<7);} while(0)
#define SET_BUSY_OUT() do {P2M1 &= ~(1<<7); P2M0 |= (1<<7); } while(0)

//returns 1 if busy, 0 otherwise
static char readBusy() {
	unsigned char oldP2 = P2;
	__bit busy;

	LCD_RS = 0; //control
	LCD_RW = 1; //read
	LCD_E = 1;
	SET_BUSY_IN();

	//wait
	_delay_us(100); // blind cycle 100us wait
	//read busy flag
	busy = LCD_BUSY;
	SET_BUSY_OUT();
	LCD_E = 0;
	LCD_E = 1;
	//wait
	_delay_us(100); // blind cycle 100us wait
	LCD_E = 0;
	P2 = oldP2;

	return busy;
}

static void wait_busy() {
	unsigned int i;
	for (i = 0; i < 100; i++){
		if (!readBusy()){
			return;
		}
		_delay_ms(1);
	}

	return;
}

static void LCD_OutChar(unsigned char c) {
	unsigned char lower = (c & 0x0f);
	DISABLE_INTERRUPTS();
	wait_busy();
	//output upper 4 bits:
	LCD_E = 0;
	LCD_RS = 1; //data
	LCD_RW = 0; //write
	P2 = ((c & 0xf0));
	LCD_E = 1;
	_delay_us(100);
	LCD_E = 0;
	//output lower 4 bits:
	P2 = ((c & 0xf) << 4);
	LCD_E = 1;
	_delay_us(100);
	LCD_E = 0;
	ENABLE_INTERRUPTS();
	wait_busy();
}

void LCD_Open(void) {
	uint8_t i;
	//set ports to push-pull output M[1:0] = b01
	//P2 entire port
	P2M1 = 0;
	P2M0 = 0xff;
	//P3 pins 7:4
	P3M1 &= ~(0xf0);
	P3M0 |= (0xf0);

	_delay_ms(30); // to allow LCD powerup
	outCsrBlindNibble(0x03); // (DL=1 8-bit mode)
	_delay_ms(5); //  blind cycle 5ms wait
	outCsrBlindNibble(0x03); // (DL=1 8-bit mode)
	_delay_us(100); // blind cycle 100us wait
	outCsrBlindNibble(0x03); // (DL=1 8-bit mode)
	_delay_us(100); //  blind cycle 100us wait (not called for, but do it anyway)
	outCsrBlindNibble(0x02); // DL=1 switch to 4 bit mode (only high 4 bits are sent)
	_delay_us(100); // blind cycle 100 us wait
	//set increment, no shift
	outCsr(0x4 + LCDINC + LCDNOSHIFT);
#ifdef DEBUG
	//set display on, cursor on, blink on:
	outCsr(0x0c + LCDCURSOR + LCDBLINK);
#else
	//set display on, cursor and blink off:
	outCsr(0x0c + LCDNOCURSOR + LCDNOBLINK);
#endif
	//set display shift on and to the right
	outCsr(0x10 + LCDNOSCROLL + LCDRIGHT);
	//set 4-bit mode, 2 line, 5x7 display:
	outCsr(0x20 + LCD2LINE + LCD7DOT);

	outCsr(0x40); //set cgram address to 0
	//program CGRAM
	for (i = 0; i < 2; i++){
		//define exponent and negative exponent sign
		LCD_OutChar(0x0);
		LCD_OutChar(i ? 0x0f : 0);
		LCD_OutChar(0x0);
		LCD_OutChar(0x1c);
		LCD_OutChar(0x10);
		LCD_OutChar(0x18);
		LCD_OutChar(0x10);
		LCD_OutChar(0x1c);
	}

	//clear display
	LCD_Clear();
}

//row and columns indexed from 0
void LCD_GoTo(unsigned int row_to, unsigned int col_to) {
	if (row_to < MAX_ROWS && col_to < MAX_CHARS_PER_LINE) {
		outCsr(0x80 + 0x40 * row_to + col_to); //set ddram address to position
		row = row_to;
		col = col_to;
	}
}

static void to_row(unsigned char row_to) {
	if (row_to == 0) {
		outCsr(0x80);//set address to start of row 0
		row = 0;
	} else {
		outCsr(0xc0);// set address to row 1
		row = 1;
	}
	col = 0;
}

void LCD_OutString(const char *string, uint8_t max_chars) {
	const char *s;
	for (s = string; *s && max_chars > 0; s++, max_chars--) {
		TERMIO_PutChar(*s);
	}
}

short TERMIO_PutChar(unsigned char letter) {
	if (letter == CR || letter == '\n') {
		LCD_Clear();
	} else if (letter == TAB || letter == '\t') {
		if (row == 0) {
			to_row(1);
		} else {
			to_row(0);
		}
	} else {
		LCD_OutChar(letter);
		col++;
		if (col > MAX_CHARS_PER_LINE) {
			if (row == 0) {
				to_row(1);
			} else {
				to_row(0);
			}
		}
	}

	return 1;
}

void LCD_ClearToEnd(uint8_t curr_row){
	while (col != 0 && row == curr_row){
		TERMIO_PutChar(' ');
	}
}

void LCD_OutNibble(uint8_t x){
	x &= 0xf; //ensure only bottom nibble
	if (x <= 9){
		TERMIO_PutChar(x + '0');
	} else {
		TERMIO_PutChar(x - 10 + 'a');
	}
}

void LCD_Clear() {
	outCsr(0x01);
	row = 0;
	col = 0;
}

