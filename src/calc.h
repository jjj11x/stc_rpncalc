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
 * calc.h
 *
 *  Created on: Mar 28, 2019
 */

#ifndef SRC_CALC_H_
#define SRC_CALC_H_

#include <stdint.h>
#include "decn/decn.h"

#ifdef __cplusplus
extern "C" {
#endif

void process_cmd(char cmd);

//push_decn is equivalent to "set_x()" if no_lift is true
void push_decn(__xdata const char* signif_str, __xdata exp_t exponent);
extern uint8_t NoLift;
extern __bit IsShiftedUp;
extern __bit IsShiftedDown;

void clear_x(void);
__xdata dec80* get_x(void);
__xdata dec80* get_y(void);

#ifdef __cplusplus
}
#endif

#endif /* SRC_CALC_H_ */
