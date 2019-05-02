/*
 * utils.h
 *
 *  Created on: Mar 10, 2019
 */

#ifndef SRC_UTILS_H_
#define SRC_UTILS_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void _delay_ms(uint8_t ms);

#define ACCURATE_DELAY_US
#ifdef ACCURATE_DELAY_US
void _delay_us(uint8_t us);
#else
#define _delay_us(x) _delay_ms(1)
#endif

void backlight_off(void);



#ifdef __linux__
#define DESKTOP
#elif _WIN32
#define DESKTOP
#elif __APPLE__
#define DESKTOP
#endif

#if defined(DESKTOP) || defined(IS_ECLIPSE)
char* u32str(uint32_t x, char* buf, uint8_t base);
#define __xdata
#define __idata
#define __sfr
#define __at uint8_t*
#define SDCC_ISR(isr, reg)
#define __using(x)
#define BACKLIGHT_ON()
#define TURN_OFF()
#else
#define SDCC_ISR(isr, reg) __interrupt (isr) __using (reg)
#define BACKLIGHT_ON()  P3_4 = 0
#define TURN_OFF() P3_2 = 0
#endif


#ifdef __cplusplus
}
#endif

#endif /* SRC_UTILS_H_ */

