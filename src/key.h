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

/*
 * key.h
 *
 *  Created on: Mar 20, 2019
 */

#ifndef SRC_KEY_H_
#define SRC_KEY_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "utils.h"

//#define DEBUG_KEYS

void KeyInit(void);

#ifndef DESKTOP
#pragma nooverlay
#endif
void raw_scan(void) __using(1);

#ifndef DESKTOP
#pragma nooverlay
#endif
void debounce(void) __using(1);


#define KeyScan() do { \
	raw_scan(); \
	debounce(); \
} while(0);

#ifdef DEBUG_KEYS
const uint8_t* DebugGetKeys(void);
#endif

//definition included for determining if multiple keys are pressed
//prefer using GetNewKeys();
#define TOTAL_ROWS 5
extern __idata uint8_t Keys[TOTAL_ROWS]; //only bottom nibbles get set
extern int8_t NewKeyPressed;


#ifdef __cplusplus
}
#endif

#endif /* SRC_KEY_H_ */
