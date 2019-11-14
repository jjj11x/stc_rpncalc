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

#ifndef LCD_H
#define LCD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "utils.h"

#define MAX_CHARS_PER_LINE 16
#define MAX_ROWS 2

void LCD_Open(void);
void LCD_Clear(void);
void LCD_GoTo(uint8_t row, uint8_t col);

void LCD_OutString(__xdata const char* string, uint8_t max_chars);
#ifdef DESKTOP
void LCD_OutString_Initial(__xdata const char* string, uint8_t max_chars);
#else
#define LCD_OutString_Initial(a, b) LCD_OutString(a, b)
#endif
short TERMIO_PutChar(unsigned char letter);
void LCD_OutNibble(uint8_t x);
void LCD_ClearToEnd(uint8_t curr_row);

//CGRAM character address
#define CGRAM_EXP 0
#define CGRAM_EXP_NEG 1

#include "utils.h"
#ifdef DESKTOP
const char* get_lcd_buf(void);
void print_lcd(void);
#endif

#ifdef __cplusplus
}
#endif

#endif

