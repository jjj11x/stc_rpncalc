//header file for main.c for gui

#ifndef QT_GUI_CALC_MAIN_H
#define QT_GUI_CALC_MAIN_H


#include <QSemaphore>

extern const char KEY_MAP[20];

extern int8_t NewKeyBuf[4];

extern QSemaphore KeysAvailable;
extern QSemaphore LcdAvailable;

extern volatile uint8_t new_key_write_i;
extern volatile uint8_t new_key_read_i;
extern volatile uint8_t NewKeyEmpty;

extern uint8_t ExitCalcMain;

int calc_main(void);

#endif //include guard
