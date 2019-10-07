/*
 * lcd_emulator.c
 *
 *  Created on: Apr 1, 2019
 */

#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include "lcd.h"

#define CR 13 // \r
#define TAB 9 // \n

static uint8_t lcd_row, lcd_col;
static char lcd_buf[MAX_ROWS][MAX_CHARS_PER_LINE];

static char enable_checks = 1;

const char* get_lcd_buf(void){
	return &lcd_buf[0][0];
}

void print_lcd(void){
	printf("(row,col)=(%d,%d)\n", lcd_row, lcd_col);
	printf("|---|---|---|---|\n");
	for (int i = 0; i < MAX_ROWS; i++){
		printf("|");
		for (int j = 0; j < MAX_CHARS_PER_LINE; j++){
			printf("%c", lcd_buf[i][j]);
		}
		printf("\n");
	}
	printf("|---|---|---|---|\n");
}

void LCD_Open(void){
	LCD_Clear();
}

void LCD_Clear(void){
	for (int i = 0; i < MAX_ROWS; i++){
		for (int j = 0; j < MAX_CHARS_PER_LINE; j++){
			lcd_buf[i][j] = ' ';
		}
	}
	lcd_row=0;
	lcd_col=0;
}

void LCD_GoTo(uint8_t row, uint8_t col){
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

void LCD_OutString_Initial(const char *string, uint8_t max_chars) {
	enable_checks = 0;
	LCD_OutString(string, max_chars);
	enable_checks = 1;
}

static int is_valid_character(char letter){
	if (isdigit(letter)){
		return 1;
	} else if(letter == CGRAM_EXP || letter == CGRAM_EXP_NEG){
		return 1;
	} else if(letter == '.' || letter == ' ' || letter == '-'){
		return 1;
	} else if(letter == '^'){
		return 1;
	}

	return 0;
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
	}
	//warn if unknown character
	if (!is_valid_character(letter) && enable_checks) {
		printf("\nerror @%d,%d, invalid character %d\n",
				lcd_row, lcd_col, letter);
	}
	//add character to buf
	if (letter == CGRAM_EXP){
		lcd_buf[lcd_row][lcd_col] = 'E';
	} else if (letter == CGRAM_EXP_NEG) {
		lcd_buf[lcd_row][lcd_col] = '-';
	} else {
		lcd_buf[lcd_row][lcd_col] = letter;
	}
	lcd_col++;
	//check if new line
	if (lcd_col >= MAX_CHARS_PER_LINE) {
		if (lcd_row == 0) {
			to_row(1);
		} else {
			to_row(0);
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
