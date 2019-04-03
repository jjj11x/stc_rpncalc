/*
 * lcd_emulator.c
 *
 *  Created on: Apr 1, 2019
 */

#include <stdio.h>
#include <stdint.h>
#include "lcd.h"

#define CR 13 // \r
#define TAB 9 // \n

static int lcd_row, lcd_col;
static char lcd_buf[MAX_ROWS][MAX_CHARS_PER_LINE];

void LCD_Open(void){

}

void LCD_Clear(void){
	lcd_row=0;
	lcd_col=0;
	for (int i = 0; i < MAX_ROWS; i++){
		for (int j = 0; j < MAX_CHARS_PER_LINE; j++){
			lcd_buf[i][j] = 0;
		}
	}
}

void LCD_GoTo(unsigned int row, unsigned int col){
	if (row < MAX_ROWS && col < MAX_CHARS_PER_LINE){
		lcd_row = row;
		lcd_col = col;
	} else {
		printf("LCD_GoTo(%u, %u) out of range\n", lcd_row, lcd_col);
	}
}

static void to_row(unsigned char row_to){
	if (row_to == 0){
		lcd_row = 0;
	} else {
		lcd_row = 1;
	}
	lcd_col = 0;
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
		if (lcd_row == 0) {
			to_row(1);
		} else {
			to_row(0);
		}
	} else {
		lcd_buf[lcd_row][lcd_col] = letter;
		lcd_col++;
		if (lcd_col > MAX_CHARS_PER_LINE) {
			if (lcd_row == 0) {
				to_row(1);
			} else {
				to_row(0);
			}
		}
	}

	return 1;
}

void LCD_ClearToEnd(uint8_t curr_row){
	while (lcd_col != 0 && lcd_row == curr_row){
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
