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
