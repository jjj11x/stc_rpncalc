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

//for printing: exponent gets returned as int8_t
#define DECN_MIN_PRINT_EXP (-128)
#define DECN_MAX_PRINT_EXP   128

//decimal80 unpacked into 80 bits
// for computation
typedef struct {
	exp_t exponent; //MSBit is sign of dec80 number, bottom 15 or 7 bits are 2's Compl exponent
	uint8_t lsu[DEC80_NUM_LSU]; //lsu[0] holds most-significant 2 digits (base 100)
	//implicit decimal point between (lsu[0]/10) and (lsu[0]%10)
} dec80;

//remove sign bit, and return 15 bit exponent sign-extended to 16 bits
exp_t get_exponent(const dec80* const x);

void set_exponent(dec80* acc, exp_t exponent, uint8_t num_is_neg);

void remove_leading_zeros(dec80* x);


void copy_decn(dec80* const dest, const dec80* const src);

extern dec80 AccDecn;
extern __idata dec80 BDecn;
extern __idata uint8_t TmpStackPtr;

void build_dec80(__xdata const char* signif_str, __xdata exp_t exponent);

void set_dec80_zero(dec80* dest);
void set_decn_one(dec80* dest);
void set_dec80_NaN(dec80* dest);
uint8_t decn_is_zero(const dec80* x);
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
void pow_decn(void);
void sqrt_decn(void);

void sin_decn(void);
void cos_decn(void);
void tan_decn(void);
void arctan_decn(void);
void arcsin_decn(void);
void arccos_decn(void);
void to_degree_decn(void);
void to_radian_decn(void);
void pi_decn(void);

//Buf should hold at least 18 + 4 + 5 + 1 = 28
#define DECN_BUF_SIZE 28
extern __xdata char Buf[DECN_BUF_SIZE];

#ifdef DESKTOP
int
#else
int8_t
#endif
decn_to_str(const dec80* x);

#ifdef DESKTOP
//complete string including exponent
void decn_to_str_complete(const dec80* x);
void build_decn_at(dec80* dest, const char* signif_str, exp_t exponent);
#endif

#ifdef DESKTOP
#define PRINT_DEC80(n, v) \
	printf(n " %d %5d: ", v.exponent < 0, get_exponent(&v)); \
	for (int i = 0; i < DEC80_NUM_LSU; i++) { \
		printf("%02d ", v.lsu[i]); \
	} \
	fputc('\n', stdout);
#else
#define PRINT_DEC80(n, v)
#endif

#ifdef __cplusplus
}
#endif

#endif /* SRC_DEC_DECN_H_ */
