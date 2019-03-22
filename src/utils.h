/*
 * utils.h
 *
 *  Created on: Mar 10, 2019
 */

#ifndef SRC_UTILS_H_
#define SRC_UTILS_H_

#include <stdint.h>

void _delay_ms(uint8_t ms);

//TODO
#define _delay_us(x) _delay_ms(1)

char* u32str(uint32_t x, char* buf, uint8_t base);


#ifdef __linux__
#define DESKTOP
#elif _WIN32
#define DESKTOP
#elif __APPLE__
#define DESKTOP
#endif

#endif /* SRC_UTILS_H_ */

