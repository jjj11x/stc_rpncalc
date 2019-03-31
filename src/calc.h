/*
 * calc.h
 *
 *  Created on: Mar 28, 2019
 */

#ifndef SRC_CALC_H_
#define SRC_CALC_H_

#include <stdint.h>
#include "decn/decn.h"

void process_cmd(char cmd);

void push_decn(const char* signif_str, int16_t exponent, uint8_t no_lift);

void clear_x(void);
void set_x(const char* signif_str, int16_t exponent);
__xdata dec80* get_x(void);
__xdata dec80* get_y(void);

#endif /* SRC_CALC_H_ */
