#ifndef LCD_H
#define LCD_H


#define MAX_CHARS_PER_LINE 16
#define MAX_ROWS 2

void LCD_Open(void);
void LCD_Clear(void);
void LCD_GoTo(unsigned int row, unsigned int col);
void LCD_SingleLineGoTo(unsigned int row_to);

void LCD_OutString(const char* string, uint8_t max_chars);
short TERMIO_PutChar(unsigned char letter);
void LCD_OutNibble(uint8_t x);
void LCD_ClearToEnd(uint8_t curr_row);

unsigned char LCD_Timeout_Error(void);

//CGRAM character address
#define CGRAM_EXP 0
#define CGRAM_EXP_NEG 1

#endif

