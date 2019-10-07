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

#define EXP16

#ifdef EXP16
typedef int16_t exp_t;
//allow full range, but reserve -16384 for special numbers
#define DEC80_MIN_EXP (-16383)
#define DEC80_MAX_EXP   16383
#define DEC80_NAN_EXP (-16383 - 1)
#else
typedef int8_t exp_t;
//allow full range, but reserve -128 for special numbers
#define DEC80_MIN_EXP (-63)
#define DEC80_MAX_EXP   63
#define DEC80_NAN_EXP (-63 - 1) //-64
#endif


//decimal80 unpacked into 80 bits
// for computation
typedef struct {
	exp_t exponent; //MSBit is sign of dec80 number, bottom 15 or 7 bits are 2's Compl exponent
	uint8_t lsu[DEC80_NUM_LSU]; //lsu[0] holds most-significant 2 digits (base 100)
	//implicit decimal point between (lsu[0]/10) and (lsu[0]%10)
} dec80;

//1 constant
static const dec80 DECN_1 = {
	0, {10, 0}
};

//ln(10) constant
static const dec80 DECN_LN_10 = {
	0, {23,  2, 58, 50, 92, 99, 40, 45, 68}
};

//remove sign bit, and return 15 bit exponent sign-extended to 16 bits
exp_t get_exponent(const dec80* x);

void copy_decn(dec80* dest, const dec80* src);

extern dec80 AccDecn;
extern __idata dec80 BDecn, Tmp4Decn;

void build_dec80(__xdata const char* signif_str, exp_t exponent);

void set_dec80_zero(dec80* dest);
void set_dec80_NaN(dec80* dest);
uint8_t decn_is_nan(const dec80* x);

void negate_decn(dec80* x);
void add_decn(void);   //calculate AccDecn -= BDecn (BDecn is preserved)
void mult_decn(void);  //calculate AccDecn *= BDecn (BDecn is preserved)
void recip_decn(void);
void div_decn(void);

void ln_decn(void);
void log10_decn(void);

void exp_decn(void);
void exp10_decn(void);

//calculate AccDecn = AccDecn ^ BDecn
#define pow_decn() do {\
	copy_decn(&Tmp4Decn, &BDecn); \
	ln_decn(); \
	copy_decn(&BDecn, &Tmp4Decn); \
	mult_decn(); \
	exp_decn(); \
} while (0);

//Buf should hold at least 18 + 4 + 5 + 1 = 28
#define DECN_BUF_SIZE 28
extern __xdata char Buf[DECN_BUF_SIZE];
int8_t decn_to_str(const dec80* x);

#ifdef DESKTOP
//complete string including exponent
void decn_to_str_complete(const dec80* x);
void build_decn_at(dec80* dest, const char* signif_str, exp_t exponent);
#endif

#ifdef __cplusplus
}
#endif

#endif /* SRC_DEC_DECN_H_ */
