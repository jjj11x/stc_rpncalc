#ifndef LCD_H
#define LCD_H


#define MAX_CHARS_PER_LINE 15
#define MAX_ROWS 2

void LCD_Open(void);
void LCD_Clear(void);
void LCD_GoTo(unsigned int row, unsigned int col);
void LCD_SingleLineGoTo(unsigned int row_to);

void LCD_OutString(const char* string);
short TERMIO_PutChar(unsigned char letter);
void LCD_OutNibble(uint8_t x);

unsigned char LCD_Timeout_Error(void);


#endif

