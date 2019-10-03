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
void push_decn(__xdata const char* signif_str, exp_t exponent);
extern uint8_t NoLift;
extern uint8_t IsShifted;

void clear_x(void);
__xdata dec80* get_x(void);
__xdata dec80* get_y(void);

#ifdef __cplusplus
}
#endif

#endif /* SRC_CALC_H_ */
