/*
 * decn.h
 *
 *  Created on: Mar 21, 2019
 */

#ifndef SRC_DEC_DECN_H_
#define SRC_DEC_DECN_H_

#include <stdint.h>
#include "../utils.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEC80_NUM_LSU 9

//allow full range, but reserve -16384 for special numbers
#define DEC80_MIN_EXP (-16383)
#define DEC80_MAX_EXP   16383

//decimal80 unpacked into 80 bits
// for computation
typedef struct {
	int16_t exponent; //MSBit is sign of dec80 number, bottom 15 bits are 2's Compl exponent
	uint8_t lsu[DEC80_NUM_LSU]; //lsu[0] holds most-significant 2 digits (base 100)
	//implicit decimal point between (lsu[0]/10) and (lsu[0]%10)
} dec80;

//1 constant
static const dec80 DECN_1 = {
	0,
	{10, 0}
};

//remove sign bit, and return 15 bit exponent sign-extended to 16 bits
int16_t get_exponent(const dec80* x);

//void dec64to80(dec80* dest, const dec64* src);

void copy_decn(dec80* dest, const dec80* src);

void build_dec80(dec80* dest, const char* signif_str, int16_t exponent);

void set_dec80_zero(dec80* dest);
void set_dec80_NaN(dec80* dest);

void negate_decn(dec80* x);
int8_t compare_decn(const dec80* a, const dec80* b); //a<b: -1, a==b: 0, a>b: 1
void add_decn(dec80* acc, const dec80* x);
void mult_decn(dec80* acc, const dec80* x);
void div_decn(dec80* acc, const dec80* x);

//buf should hold at least 18 + 4 + 5 + 1 = 28
#define DECN_BUF_SIZE 28
int8_t decn_to_str(char* buf, const dec80* x);

#ifdef DESKTOP
//complete string including exponent
void decn_to_str_complete(char* buf, const dec80* x);
#endif

#ifdef __cplusplus
}
#endif

#endif /* SRC_DEC_DECN_H_ */
