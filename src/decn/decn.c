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
 * decn.c
 *
 *  Created on: Mar 21, 2019
 *      Author: jeffrey
 */

#include "../utils.h"
#include "../stack_debug.h"

#include "decn.h"


#define EXTRA_CHECKS
// #define DEBUG
// #define DEBUG_COMPARE_MAGN
// #define DEBUG_ADD
// #define DEBUG_MULT
// #define DEBUG_MULT_ALL //even more verbose
// #define DEBUG_DIV
// #define DEBUG_LOG
// #define DEBUG_LOG_ALL //even more verbose
// #define DEBUG_EXP
// #define DEBUG_EXP_ALL //even more verbose
// #define DEBUG_SQRT

#ifndef DESKTOP
//#undef EXTRA_CHECKS
#undef DEBUG
#undef DEBUG_COMPARE_MAGN
#undef DEBUG_ADD
#undef DEBUG_MULT
#undef DEBUG_DIV
#undef DEBUG_LOG
#undef DEBUG_LOG_ALL
#undef DEBUG_EXP
#undef DEBUG_EXP_ALL
#undef DEBUG_SQRT
#endif

#ifdef DESKTOP
#include <stdio.h>
#endif

#ifdef DESKTOP
#include <assert.h>
#else
#define assert(x)
#endif

#ifdef DESKTOP
static const uint8_t num_digits_display = DEC80_NUM_LSU*2;
#else
static const uint8_t num_digits_display = 16;
#endif

dec80 AccDecn;
__idata dec80 BDecn;
__idata dec80 TmpDecn; //used by add_decn() and mult_decn() and sqrt_decn()
__idata dec80 Tmp2Decn; //used by recip_decn(), ln_decn(), exp_decn(), sqrt_decn(), and sincos_decn()
__idata dec80 Tmp3Decn; //used by ln_decn(), exp_decn(), sqrt_decn(), and sincos_decn()
__xdata dec80 Tmp4Decn; //used by sincos_decn()

__xdata dec80 TmpStackDecn[4];
#define TMP_STACK_SIZE  (sizeof TmpStackDecn / sizeof TmpStackDecn[0])
__idata uint8_t TmpStackPtr;

__xdata char Buf[DECN_BUF_SIZE];

//ln(10) constant
const dec80 DECN_LN_10 = {
	0, {23,  2, 58, 50, 92, 99, 40, 45, 68}
};

// pi
const dec80 DECN_PI = {
	0, {31, 41, 59, 26, 53, 58, 97, 93, 24}
};

// pi/2
const dec80 DECN_PI2 = {
	0, {15, 70, 79, 63, 26, 79, 48, 96, 62}
};

// 180/pi = 1rad in degree
const dec80 DECN_1RAD = {
	1, {57, 29, 57, 79, 51, 30, 82, 32,  9}
};

void st_push_decn(const dec80 * const src)
{
	copy_decn(&TmpStackDecn[TmpStackPtr], src);
	TmpStackPtr++;
	assert(TmpStackPtr < TMP_STACK_SIZE);
}

void st_pop_decn(dec80 * const dst)
{
	assert(TmpStackPtr >= 1);
	TmpStackPtr--;
	if (dst) copy_decn(dst, &TmpStackDecn[TmpStackPtr]);
}

void st_load_decn(dec80 * const dst)
{
	assert(TmpStackPtr >= 1);
	copy_decn(dst, &TmpStackDecn[TmpStackPtr - 1]);
}

void copy_decn(dec80* const dest, const dec80* const src){
	uint8_t i;

	stack_debug(0x01);
	dest->exponent = src->exponent;

	//copy nibbles
	for (i = 0; i < DEC80_NUM_LSU; i++){
		dest->lsu[i] = src->lsu[i];
	}
}

exp_t get_exponent(const dec80* const x){
	exp_t exponent = x->exponent;
#ifdef EXP16
	if (exponent & 0x4000){ //negative
		return exponent | 0x8000;
	} else { //positive
		return exponent & 0x7fff;
	}
#else
	if (exponent & 0x40){ //negative
		return exponent | 0x80;
	} else { //positive
		return exponent & 0x7f;
	}
#endif
}

void set_exponent(dec80* acc, exp_t exponent, uint8_t num_is_neg){
#ifdef EXP16
	if (num_is_neg){
		exponent |= 0x8000;
	} else {
		exponent &= 0x7fff;
	}
#else
	if (num_is_neg){
		exponent |= 0x80;
	} else {
		exponent &= 0x7f;
	}
#endif

	acc->exponent = exponent;
}

static void zero_remaining_dec80(dec80* dest, uint8_t digit100){
	for ( ; digit100 < DEC80_NUM_LSU; digit100++){
		dest->lsu[digit100] = 0;
	}
}


static uint8_t shift_high = 0, shift_low = 0, shift_old = 0;
static uint8_t shift_i;
static void shift_right(dec80* x){
	shift_high = shift_low = shift_old = 0;
	for (shift_i = 0; shift_i < DEC80_NUM_LSU; shift_i++){
		shift_high = x->lsu[shift_i] / 10;
		shift_low = x->lsu[shift_i] % 10;
		x->lsu[shift_i] = shift_high + ( shift_old*10);
		shift_old = shift_low;
	}
}

static void shift_left(dec80* x){
	shift_high = shift_low = shift_old = 0;
	for (shift_i = DEC80_NUM_LSU - 1; shift_i < 255; shift_i--){
		shift_high = x->lsu[shift_i] / 10;
		shift_low = x->lsu[shift_i] % 10;
		x->lsu[shift_i] = shift_old + ( shift_low*10);
		shift_old = shift_high;
	}
}

void remove_leading_zeros(dec80* x){
	uint8_t digit100;
	uint8_t is_negative = (x->exponent < 0);
	exp_t exponent = get_exponent(x);

	stack_debug(0x02);
	//find first non-zero digit100
	for (digit100 = 0; digit100 < DEC80_NUM_LSU; digit100++){
		if (x->lsu[digit100] != 0){
			break;
		}
	}
	exponent -= digit100 * 2; //base 100

	//copy digit100s left if needed
	if (digit100 != 0 && digit100 < DEC80_NUM_LSU){ //found non-zero digit100
		uint8_t i;
		//copy digit100s
		for (i = 0; digit100 < DEC80_NUM_LSU; i++, digit100++){
			x->lsu[i] = x->lsu[digit100];
		}
		//zero out remaining digit100s, now that number left-aligned
		zero_remaining_dec80(x, i);
	}

	//ensure MSdigit in MSdigit100 is > 0
	if (x->lsu[0] < 10) {
		shift_left(x);
		exponent--;
	}
	//write back exponent
	set_exponent(x, exponent, is_negative);
}

void build_dec80(__xdata const char* signif_str, __xdata exp_t exponent){
	enum {
		SIGN_ZERO,
		SIGN_ZERO_SEEN_POINT,
		SIGN_NEG_ZERO,
		SIGN_NEG_ZERO_SEEN_POINT,
		SIGN_POS,
		SIGN_POS_SEEN_POINT,
		SIGN_NEG,
		SIGN_NEG_SEEN_POINT
	};
#ifdef DESKTOP
	static_assert(SIGN_ZERO < SIGN_NEG_ZERO, "do not change enum values");
#endif
#define SEEN_POINT(curr_sign) ((curr_sign) & 1)
#define IS_ZERO(curr_sign) ((curr_sign) <= SIGN_NEG_ZERO_SEEN_POINT)
#define IS_NEG(curr_sign) ((curr_sign) >= SIGN_NEG)

	uint8_t i = 0;
	uint8_t nibble_i = 0;
	uint8_t save_nibble = 0;
	int8_t num_lr_points = 0; //number of digits to the right (-) or left of decimal point
	int8_t curr_sign = SIGN_ZERO;

	//check first digit
	if (signif_str[0] == '\0'){
		set_dec80_zero(&AccDecn);
		return;
	} else if (signif_str[0] == '-'){
		curr_sign = SIGN_NEG_ZERO;
		i++;
	}

	//go through digits
	while (1){
		if (signif_str[i] == '.'){
			if (!SEEN_POINT(curr_sign)){
				//begin tracking number of digits to right of decimal point
				curr_sign |= 1; //seen point
			}
#ifdef EXTRA_CHECKS
			else {
				//multiple '.'s in string
#ifdef DEBUG
				printf("  ERROR: multiple '.'s in string\n");
#endif
				set_dec80_NaN(&AccDecn);
				return;
			}
#endif
		} else if (signif_str[i] >= '1' && signif_str[i] <= '9'){
			if (nibble_i < DEC80_NUM_LSU*2){
				if (nibble_i & 1) { //odd
					AccDecn.lsu[nibble_i/2] = save_nibble * 10 + (signif_str[i] - '0');
				} else {
					save_nibble = signif_str[i] - '0';
				}
			}
			nibble_i++;
			//track number of digits R of decimal point
			//must do this before changing is_zero
			if (num_lr_points == 0 &&
			   (curr_sign == SIGN_ZERO_SEEN_POINT || curr_sign == SIGN_NEG_ZERO_SEEN_POINT))
			{
				num_lr_points = -1;
			}
			//track sign
			if (curr_sign == SIGN_ZERO){
				curr_sign = SIGN_POS;
			} else if (curr_sign == SIGN_ZERO_SEEN_POINT){
				curr_sign = SIGN_POS_SEEN_POINT;
			} else if (curr_sign == SIGN_NEG_ZERO){
				curr_sign = SIGN_NEG;
			} else if (curr_sign == SIGN_NEG_ZERO_SEEN_POINT){
				curr_sign = SIGN_NEG_SEEN_POINT;
			}
			//track number digits L of decimal point
			if (!SEEN_POINT(curr_sign)){ //haven't seen decimal point yet
				num_lr_points++; //increase left count
			}
		} else if (signif_str[i] == '0'){
			//make sure not a leading zero
			if (!IS_ZERO(curr_sign)){ //non-zero value
				if (nibble_i < DEC80_NUM_LSU*2){
					if (nibble_i & 1) { //odd
						AccDecn.lsu[nibble_i/2] = save_nibble * 10 + 0;
					} else {
						save_nibble = 0;
					}
				}
				nibble_i++;
			}
			//track number digits L/R of decimal point
			if (SEEN_POINT(curr_sign)){
				if (IS_ZERO(curr_sign)){ //tracking 0s to right of point
					if (num_lr_points == 0){ //no left count exists
						num_lr_points = -2;
					} else if (num_lr_points < 0){ //continue tracking count
						num_lr_points--; //increase right count
					}
				}
			} else { //haven't seen decimal point yet
				if (!IS_ZERO(curr_sign)){ //not a leading zero
					num_lr_points++; //increase left count
				}
			}
		} else if (signif_str[i] == '\0'){ //done
			if (IS_ZERO(curr_sign)){
				//zero
				set_dec80_zero(&AccDecn);
				return;
			} else {
				//not zero
				exp_t new_exponent = exponent;
				//write out saved nibble, if it exists
				// (saved while nibble_i even, nibble_i then incremented to odd)
				if (nibble_i & 1){ //odd
					AccDecn.lsu[nibble_i/2] = save_nibble * 10;
					nibble_i++; //increment for zeroing out
				}
				//zero out any old data
				zero_remaining_dec80(&AccDecn, nibble_i/2);
				// adjust exponent for left-aligned significand input
				// or for number of digits past decimal point
				if (num_lr_points > 0){ //left count exists
					new_exponent = exponent + (num_lr_points - 1); //1 digit left of implicit point
					//overflow is checked later, should be impossible to overflow int16_t:
					assert(new_exponent >= exponent);
				} else if (num_lr_points < 0) { //right count exists
					// (num_lr_points represents exponent shift)
					// (this ends up being a subtraction)
					new_exponent = exponent + num_lr_points;
					//underflow is checked later, should be impossible to overflow int16_t:
					assert(new_exponent <= exponent);
				}

				//check for over/underflow of exponent
				exponent = new_exponent;
#ifdef EXTRA_CHECKS
				if (exponent > DEC80_MAX_EXP || exponent < DEC80_MIN_EXP){
	#ifdef DEBUG
					printf("   over/underflow (new_exp, exp)=(%d,%d)\n",
						   new_exponent, exponent);
	#endif
					set_dec80_NaN(&AccDecn);
					return;
				}
#endif
				//set negative bit
				set_exponent(&AccDecn, exponent, IS_NEG(curr_sign));
				//normalize
				remove_leading_zeros(&AccDecn);
#ifdef DEBUG
				printf("   num_lr_points (%d), new_exp (%d), sign (%d), exp (%d)\n",
				        num_lr_points, new_exponent, curr_sign, exponent);
				printf("   ");
				for (i = 0; i < DEC80_NUM_LSU; i++){
					printf("%02d,", AccDecn.lsu[i]);
				}
				printf("\n");
#endif

				return;
			}
		} else { //invalid character
#ifdef DEBUG
			printf(" invalid character %c at i=%d\n", signif_str[i], i);
#endif
			set_dec80_NaN(&AccDecn);
			return;
		}
		i++;
		assert(i < DECN_BUF_SIZE);
	} // while 1
#undef SEEN_POINT
#undef IS_ZERO
#undef IS_NEG
}



void set_dec80_zero(dec80* dest){
	uint8_t i;

	//clear exponent
	dest->exponent = 0;
	//clear nibbles
	for (i = 0; i < DEC80_NUM_LSU; i++){
		dest->lsu[i] = 0;
	}
}

void set_decn_one(dec80* dest){
	set_dec80_zero(dest);
	dest->lsu[0] = 10;
}

uint8_t decn_is_zero(const dec80* x){
	uint8_t i;
	for (i = 0; i < DEC80_NUM_LSU; i++){
		if (x->lsu[i] != 0){
			return 0;
		}
	}

	return 1;
}

uint8_t decn_is_nan(const dec80* x){
	uint8_t i;
	if (x->exponent != DEC80_NAN_EXP){
		return 0;
	}
	for (i = 0; i < DEC80_NUM_LSU; i++){
		if (x->lsu[i] != 0xff){
			return 0;
		}
	}
	return 1;
}

#ifdef EXTRA_CHECKS
void set_dec80_NaN(dec80* dest){
	uint8_t i;

	//set exponent to special val
	dest->exponent = DEC80_NAN_EXP;
	//set all nibbles
	for (i = 0; i < DEC80_NUM_LSU; i++){
		dest->lsu[i] = 0xff;
	}
}
#endif

void negate_decn(dec80* x){
#ifdef EXP16
	static const exp_t xor_val = -(0x7fff) - 1;
#else
	static const exp_t xor_val = -(0x7f) - 1;
#endif
	if (decn_is_nan(x)){
		return;
	}
	(x->exponent) ^= xor_val;
}

static int8_t compare_magn(void){ //returns a<b: -1, a==b: 0, a>b: 1
	uint8_t a_i, b_i;
	exp_t a_exp=0, b_exp=0;
	int8_t a_signif_b = 0; //a<b: -1, a==b: 0, a>b: 1
	static __xdata dec80 a_tmp, b_tmp;
	//copy
	copy_decn(&a_tmp, &AccDecn);
	copy_decn(&b_tmp, &BDecn);
	//normalize
	remove_leading_zeros(&a_tmp);
	remove_leading_zeros(&b_tmp);
	//compare signifcands while tracking magnitude
	for (
		a_i = 0, b_i = 0;
		a_i < DEC80_NUM_LSU;
		a_i++, b_i++, a_exp+=2, b_exp+=2
		)
	{
		//set signif. inequality if this is first digit that is different
		if (a_signif_b == 0 && (a_tmp.lsu[a_i] < b_tmp.lsu[b_i])){
#ifdef DEBUG_COMPARE_MAGN
			printf("a_signif_b -1: a.lsu[%d]=%d, b.lsu[%d]=%d\n",
			        a_i, a_tmp.lsu[a_i], b_i, b_tmp.lsu[b_i]);
#endif
			a_signif_b = -1;
		} else if (a_signif_b == 0 && (a_tmp.lsu[a_i] > b_tmp.lsu[b_i])){
#ifdef DEBUG_COMPARE_MAGN
			printf("a_signif_b  1: a.lsu[%d]=%d, b.lsu[%d]=%d\n",
			        a_i, a_tmp.lsu[a_i], b_i, b_tmp.lsu[b_i]);
#endif
			a_signif_b = 1;
		}
	}
	//calculate exponents
	a_exp += get_exponent(&a_tmp);
	b_exp += get_exponent(&b_tmp);
	//compare exponents
	if (a_exp > b_exp){
#ifdef DEBUG
		printf("a_exp > b_exp\n");
#endif
		return 1;
	} else if (a_exp < b_exp){
#ifdef DEBUG
		printf("a_exp < b_exp\n");
#endif
		return -1;
	}
	//exponents equal, compare by significand
#ifdef DEBUG
		printf("a_signif_b (%d)\n", a_signif_b);
#endif
	return a_signif_b;
}

#ifdef USE_COMPARE_DECN
static int8_t compare_decn(void){ //returns a<b: -1, a==b: 0, a>b: 1
	int8_t is_neg;

	//handle zero special cases
	if (decn_is_zero(&AccDecn) && decn_is_zero(&BDecn)){
		return 0;
	} else if (decn_is_zero(&AccDecn)){
		if (BDecn.exponent < 0){
			return 1;
		} else {
			return -1;
		}
	} else if (decn_is_zero(&BDecn)){
		if (AccDecn.exponent < 0){
			return -1;
		} else {
			return 1;
		}
	}
	//handle cases where signs differ
	if (AccDecn.exponent < 0 && BDecn.exponent > 0){
		return -1;
	} else if (AccDecn.exponent > 0 && BDecn.exponent < 0){
		return 1;
	}
	//signs must now be the same, either both pos, or both neg
	is_neg = (AccDecn.exponent < 0 ? -1 : 1);

	return is_neg * compare_magn();
}
#endif //USE_COMPARE_DECN

//WARNING: for add_decn() and sub_mag() functions only
//acc and the number from which exponent was taken MUST be stripped of leading 0s first
//rescales acc up to exponent (increase exponent of acc, while shifting right)
//the actual value of acc->exponent remains unchanged
static void _incr_exp(dec80* acc, exp_t exponent){
	exp_t curr_exp = get_exponent(acc);
#ifdef DEBUG_ADD
	uint8_t is_neg = (acc->exponent < 0);
	printf("   (is_neg,curr_exp,exponent)=(%d,%d,%d)\n",
	        is_neg, curr_exp, exponent);
#endif
	assert(exponent > curr_exp);
	while (curr_exp != exponent){
		//shift right
		shift_right(acc);
		curr_exp++;
	}

	//curr_exp does NOT get written back to acc->exponent
}

//for internal use only,
//AccDecn must be larger than BDecn in absolute value
//subtract by equal addition algorithm
static void sub_mag(){
	int8_t i;
	uint8_t carry = 0;
	//normalize
	remove_leading_zeros(&AccDecn);
	remove_leading_zeros(&BDecn);
	if (get_exponent(&AccDecn) != get_exponent(&BDecn)){
		_incr_exp(&BDecn, get_exponent(&AccDecn));
	}
#ifdef DEBUG_ADD
	decn_to_str_complete(&tmp);
	printf("        incr_exp tmp: %s\n", Buf);
#endif
	//do subtraction
	for (i = DEC80_NUM_LSU - 1; i >=0; i--){
		uint8_t digit100;
		if (AccDecn.lsu[i] >= (BDecn.lsu[i] + carry)){
			digit100 = AccDecn.lsu[i] - (BDecn.lsu[i] + carry);
			carry = 0;
		} else {
			digit100 = AccDecn.lsu[i] + 100 - (BDecn.lsu[i] + carry);
			carry = 1;
		}
		assert(digit100 < 100);
		      AccDecn.lsu[i] = digit100;
	}
	assert(carry == 0); //shouldn't be carry out if |acc| > |x|
}

void add_decn(void){
	int8_t rel;
	uint8_t carry = 0;
	int8_t i;

	//check if zero
	if (decn_is_zero(&BDecn)){
		return;
	} else if (decn_is_zero(&AccDecn)){
		copy_decn(&AccDecn, &BDecn);
		return;
	}
	//save b for restoring later
	//n.b. don't use TmpStackDecn here, it is called quite often. So you'd need to increase TMP_STACK_SIZE
	copy_decn(&TmpDecn, &BDecn);
	//handle cases where signs differ
	if (AccDecn.exponent < 0 && BDecn.exponent >= 0){
		// -acc, +x
		rel = compare_magn();
		if (rel == 1){
#ifdef DEBUG_ADD
			printf("|-acc| > |+x|\n");
#endif
			sub_mag();
		} else if (rel == -1){
#ifdef DEBUG_ADD
			printf("|-acc| < |+x|\n");
#endif
			copy_decn(&BDecn, &AccDecn);
			copy_decn(&AccDecn, &TmpDecn);
			sub_mag();
		} else { //equal
#ifdef DEBUG_ADD
			printf("|-acc| == |+x|\n");
#endif
			set_dec80_zero(&AccDecn);
		}
		//restore b
		copy_decn(&BDecn, &TmpDecn);

		return;
	} else if (AccDecn.exponent >= 0 && BDecn.exponent < 0){
		// +acc, -x
		rel = compare_magn();
		if (rel == 1){
#ifdef DEBUG_ADD
			printf("|+acc| > |-x|\n");
#endif
			sub_mag();
		} else if (rel == -1){
#ifdef DEBUG_ADD
			printf("|+acc| < |-x|\n");
#endif
			copy_decn(&BDecn, &AccDecn);
			copy_decn(&AccDecn, &TmpDecn);
			sub_mag();
		} else { //equal
#ifdef DEBUG_ADD
			printf("|+acc| == |-x|\n");
#endif
			set_dec80_zero(&AccDecn);
		}
		//restore b
		copy_decn(&BDecn, &TmpDecn);

		return;
	}
	//signs must now be the same, begin adding
	//normalize
	remove_leading_zeros(&AccDecn);
	remove_leading_zeros(&BDecn);
#ifdef DEBUG_ADD
	decn_to_str_complete(&AccDecn);
	printf("        rem_leading_zeros acc: %s\n", Buf);
	decn_to_str_complete(&BDecn);
	printf("        rem_leading_zeros tmp: %s\n", Buf);
#endif
	if (get_exponent(&AccDecn) > get_exponent(&BDecn)){
		_incr_exp(&BDecn, get_exponent(&AccDecn));
	} else if (get_exponent(&AccDecn) < get_exponent(&BDecn)){
		//shift significand and adjust exponent to match
		for (i = 0; i < get_exponent(&BDecn) - get_exponent(&AccDecn); i++){
			shift_right(&AccDecn);
		}
		set_exponent(&AccDecn, get_exponent(&BDecn), (AccDecn.exponent < 0));
	}
#ifdef DEBUG_ADD
	decn_to_str_complete(&AccDecn);
	printf("        incr_exp acc: %s\n", Buf);
	decn_to_str_complete(&BDecn);
	printf("        incr_exp tmp: %s\n", Buf);
#endif
	//do addition
	for (i = DEC80_NUM_LSU - 1; i >= 0; i--){
		uint8_t digit100 = AccDecn.lsu[i] + BDecn.lsu[i] + carry;
		AccDecn.lsu[i] = digit100 % 100;
		carry = digit100 / 100;
		assert(carry <= 1);
	}
	//may need to rescale number
	if (carry > 0){
		exp_t curr_exp = get_exponent(&AccDecn);
		assert(carry == 1);
		rel = (AccDecn.exponent < 0); //is_neg?
		//shift right
		shift_right(&AccDecn);
		AccDecn.lsu[0] += 10; //carry gets shifted into most significant digit
		curr_exp++;
		//track sign
		set_exponent(&AccDecn, curr_exp, rel); //rel==is_neg?
	}

	//restore b
	copy_decn(&BDecn, &TmpDecn);
}

//AccDecn *= BDecn
// (the BDecn register is preserved)
void mult_decn(void){
	int8_t i, j;
	uint8_t carry = 0;
	uint8_t is_neg;
	exp_t new_exponent;
#ifdef EXTRA_CHECKS
	if (decn_is_nan(&AccDecn) || decn_is_nan(&BDecn)) {
		set_dec80_NaN(&AccDecn);
		return;
	}
#endif
	//initialize values
	set_dec80_zero(&TmpDecn);
	//normalize
	remove_leading_zeros(&AccDecn);
	remove_leading_zeros(&BDecn);
	//store new sign
#ifdef EXP16
	if ((AccDecn.exponent & 0x8000) ^ (BDecn.exponent & 0x8000)){ //signs differ
#else
		if ((AccDecn.exponent & 0x80) ^ (BDecn.exponent & 0x80)){ //signs differ
#endif
		is_neg = 1;
	} else {
		is_neg = 0;
	}
	//calculate new exponent
	new_exponent = get_exponent(&AccDecn) + get_exponent(&BDecn);
#ifdef DEBUG_MULT
	printf("\n a_exp: %d, b_exp: %d", get_exponent(&AccDecn), get_exponent(&BDecn));
	printf("\n new exponent: %d, is_neg: %u", new_exponent, is_neg);
#endif
	//do multiply
	for (i = DEC80_NUM_LSU - 1; i >= 0; i--){
		//partial product
		for (j = DEC80_NUM_LSU - 1; j >= 0; j--){
			uint16_t digit100 = TmpDecn.lsu[j] + (BDecn.lsu[i] * AccDecn.lsu[j]) + carry;
			TmpDecn.lsu[j] = digit100 % 100;
			carry = digit100 / 100;
			assert(carry < 100);
		}
#ifdef DEBUG_MULT_ALL
		printf("\n%d:", i);
		printf("\n      acc:");
		for (j = 0; j < DEC80_NUM_LSU; j++){
			printf(" %3d", AccDecn.lsu[j]);
		}
		printf("\n        x:");
		for (j = 0; j < DEC80_NUM_LSU; j++){
			if (j == i)
				printf(" %3d", BDecn.lsu[j]);
			else
				printf("    ");
		}
#endif
#ifdef DEBUG_MULT
		printf("\n  TmpDecn:");
		for (j = 0; j < DEC80_NUM_LSU; j++){
			printf(" %3d", TmpDecn.lsu[j]);
		}
		printf("\ncarry:%d", carry);
#endif
		if (i != 0){ //handle last carry separately later, no final shift
			//shift
			shift_right(&TmpDecn);
			shift_right(&TmpDecn);
			//add back carry to MSdigit100
			TmpDecn.lsu[0] = carry; //was 0 from shift
		}
	}
	//handle last carry
	if (carry >= 10){
		//shift
		shift_right(&TmpDecn);
		shift_right(&TmpDecn);
		new_exponent += 1;
		//add back carry to MSdigit100
		TmpDecn.lsu[0] = carry; //was 0 from shift
	} else if (carry > 0){
		//shift
		shift_right(&TmpDecn);
		//add back carry to MSdigit in MSdigit100
		TmpDecn.lsu[0] += carry*10;
	}
	//set new exponent, checking for over/underflow
#ifdef DEBUG_MULT
	printf("\n new exponent: %d, is_neg: %u\n", new_exponent, is_neg);
#endif
	if (new_exponent < DEC80_MAX_EXP && new_exponent > DEC80_MIN_EXP){
		set_exponent(&TmpDecn, new_exponent, is_neg);
	} else {
		set_dec80_NaN(&AccDecn);
		return;
	}
	//copy back to acc
	copy_decn(&AccDecn, &TmpDecn);
	//normalize
	remove_leading_zeros(&AccDecn);
}

void recip_decn(void){
#define CURR_RECIP Tmp2Decn //copy of x, holds current 1/x estimate
	uint8_t i;
	exp_t initial_exp;
	//check divide by zero
#ifdef EXTRA_CHECKS
	if (decn_is_zero(&AccDecn)){
		set_dec80_NaN(&AccDecn);
#ifdef DESKTOP
		printf("error division by 0\n");
#endif
		return;
	}
#endif
	//normalize
	remove_leading_zeros(&AccDecn);
	//store copy of x
	st_push_decn(&AccDecn);
	//get initial exponent of estimate for 1/x
	initial_exp = get_exponent(&AccDecn);
#ifdef DEBUG_DIV
	printf("exponent %d", initial_exp);
#endif
	//necessary to subtract 1 for convergence
	initial_exp = -initial_exp - 1;
#ifdef DEBUG_DIV
	printf(" -> %d\n", initial_exp);
#endif
	set_exponent(&CURR_RECIP, initial_exp, (AccDecn.exponent < 0)); //set exponent, copy sign
	//get initial estimate for 1/x
	if        (AccDecn.lsu[0] < 20){       //mantissa between 1 and 2
		      CURR_RECIP.lsu[0] = 50; //0.50 with implicit point and exponent
	} else if (AccDecn.lsu[0] < 33){
		      CURR_RECIP.lsu[0] = 30;
	} else if (AccDecn.lsu[0] < 50){
		      CURR_RECIP.lsu[0] = 20;
	} else {
		      CURR_RECIP.lsu[0] = 10; //0.1 with implicit point and exponent
	}
	zero_remaining_dec80(&CURR_RECIP, 1);
	copy_decn(&AccDecn, &CURR_RECIP);
	//do newton-raphson iterations
	for (i = 0; i < 6; i++){ //just fix number of iterations for now
#ifdef DEBUG_DIV
		decn_to_str_complete(&CURR_RECIP);
		printf("%2d: %s\n", i, Buf);
#endif
		//Accum *= x_copy
		st_load_decn(&BDecn);
		mult_decn();
#ifdef DEBUG_DIV
		decn_to_str_complete(&AccDecn);
		printf("  %20s: %s\n", "recip*x", Buf);
#endif
		//Accum *= -1
		negate_decn(&AccDecn);
		//Accum += 1
		set_decn_one(&BDecn);
		add_decn();
#ifdef DEBUG_DIV
		decn_to_str_complete(&AccDecn);
		printf("  %20s: %s\n", "(1-recip*x)", Buf);
#endif
		//Accum *= curr_recip
		copy_decn(&BDecn, &CURR_RECIP);
		mult_decn();
#ifdef DEBUG_DIV
		decn_to_str_complete(&AccDecn);
		printf("  %20s: %s\n", "recip * (1-recip*x)", Buf);
#endif
		//Accum += curr_recip
		add_decn();
		//new_est(Accum) = recip + (1 - recip*x)*recip, where recip is current recip estimate
		copy_decn(&CURR_RECIP, &AccDecn);
	}
	st_pop_decn(0);

//try not to pollute namespace
#undef CURR_RECIP
}

void div_decn(void){
	//store copy of acc for final multiply by 1/x
	st_push_decn(&AccDecn);
	copy_decn(&AccDecn, &BDecn);
	recip_decn();
	//Accum now holds 1/x, multiply by original acc to complete division
	st_pop_decn(&BDecn);
	mult_decn();
}



//constants used for ln(x) and exp(x)
#define NUM_A_ARR 9
static const dec80 LN_A_ARR[NUM_A_ARR] = {
	{-1 & 0x7fff, {69, 31, 47, 18,  5, 59, 94, 53,  9}},
	{-2 & 0x7fff, {95, 31,  1, 79, 80, 43, 24, 86,  0}},
	{-3 & 0x7fff, {99, 50, 33,  8, 53, 16, 80, 82, 84}},
	{-4 & 0x7fff, {99, 95,  0, 33, 30, 83, 53, 31, 67}},
	{-5 & 0x7fff, {99, 99, 50,  0, 33, 33,  8, 33, 33}},
	{-6 & 0x7fff, {99, 99, 95,  0,  0, 33, 33, 29, 95}},
	{-7 & 0x7fff, {99, 99, 99, 50,  0,  0, 33,  5, 35}},
	{-8 & 0x7fff, {99, 99, 99, 95,  0,  0,  2, 76, 40}},
	{-9 & 0x7fff, {99, 99, 99, 99, 50,  0, 15, 98, 65}},
};

void ln_decn(void){
	uint8_t j, k;
#define B_j Tmp2Decn
#define NUM_TIMES Tmp3Decn

	//check not negative or zero
	if (AccDecn.exponent < 0 || decn_is_zero(&AccDecn)){
		set_dec80_NaN(&AccDecn);
		return;
	}
	//normalize
	remove_leading_zeros(&AccDecn);
	//scale to between 1 and 10
	NUM_TIMES.exponent = get_exponent(&AccDecn) + 1; //store initial exp in NUM_TIMES.exponent
	AccDecn.exponent = 0;
#ifdef DEBUG_LOG
	decn_to_str_complete(&AccDecn);
	printf("ln() accum scaled between 1,10: %s\n", Buf);
#endif
	//get initial estimate (accum = 10 - A)
	set_decn_one(&BDecn);
	BDecn.exponent = 1; //BDecn = 10
	negate_decn(&AccDecn);
	add_decn();
	copy_decn(&B_j, &AccDecn); //b_j = accum = 10 - A
#ifdef DEBUG_LOG
	decn_to_str_complete(&AccDecn);
	printf("ln() initial accum: %s (%d)\n", Buf, NUM_TIMES.exponent);
#endif

	//track number of times multiplied by a_arr[j]
	for (j = 0; j < NUM_A_ARR; j++){
		uint8_t k_j;
		if (j != 0){
			//b_j *= 10.0
			shift_left(&B_j);
		}
		copy_decn(&AccDecn, &B_j); //accum = b_j
		k_j = 0;
		while (!(AccDecn.exponent < 0)){ //while >= 0 (!negative) TODO: should just be >
			uint8_t i;
			copy_decn(&B_j, &AccDecn); //b_j = accum
			//accum *= a_arr[j]
			// since a_arr[j] is of the form 1 + 10^-j, just shift accum and add to self
			copy_decn(&BDecn, &AccDecn);
			for (i = 0; i < j; i++){
				shift_right(&BDecn);
			}
			add_decn();
			//accum -= 10
			// accum.exponent is 1 while needs subtracting
			if (AccDecn.lsu[0] >= 10 && get_exponent(&AccDecn) > 0){
				AccDecn.lsu[0] -= 10;
			} else {
				//set as negative to get out of while(), accum will get overwritten with b_j
				AccDecn.exponent = -1;
			}
#ifdef DEBUG_LOG_ALL
			decn_to_str_complete(&AccDecn);
			printf("    %u: %s\t", k_j, Buf);
			for (int ii = 0; ii < DEC80_NUM_LSU; ii++){
				printf("%2d ", AccDecn.lsu[ii]);
			}
			printf(" (%d)\n", get_exponent(&AccDecn));
#endif
			k_j++;
#ifdef DEBUG_LOG
			assert(k_j != 255);
#endif
		}
		//track num times
		NUM_TIMES.lsu[j] = k_j - 1;
#ifdef DEBUG_LOG
		decn_to_str_complete(&B_j);
		printf("  %u: num_times: %u, %s\n", j, NUM_TIMES.lsu[j], Buf);
#endif
	}

	//build final value
	copy_decn(&AccDecn, &B_j); //remainder
#ifdef DEBUG_LOG
	decn_to_str_complete(&AccDecn);
	printf("ln() remainder: %s\n", Buf);
#endif
	for (j = NUM_A_ARR - 1; j < NUM_A_ARR; j--){ //sum in reverse order, note: (j < NUM_A_ARR) == signed(j >= 0)
		for (k = 0; k < NUM_TIMES.lsu[j]; k++){
			//accum += ln_a_arr[j];
			copy_decn(&BDecn, &LN_A_ARR[j]);
			BDecn.exponent = 0; //tracking exponent through shifts/saved initial exponent
			add_decn();
		}
		shift_right(&AccDecn);
#ifdef DEBUG_LOG
		decn_to_str_complete(&AccDecn);
		printf("%u: ln() remainder: %s\n", j, Buf);
#endif
	}
#ifdef DEBUG_LOG
	decn_to_str_complete(&AccDecn);
	printf("ln() accum after summing: %s\n", Buf);
#endif
	//accum = -accum;
	negate_decn(&AccDecn);

	//add back in initial exponent (stored in NUM_TIMES.exponent)
	copy_decn(&B_j, &AccDecn); //temporarily store accum in B_j
	set_dec80_zero(&AccDecn);
	//check if negative
	if (NUM_TIMES.exponent < 0){
		j = 1; //track is_neg
		NUM_TIMES.exponent = -NUM_TIMES.exponent;
	} else {
		j = 0;
	}
	//check if too big for single lsu
#ifdef EXP16
	if (NUM_TIMES.exponent >= 10000){
		AccDecn.lsu[0] = NUM_TIMES.exponent / 10000;
		NUM_TIMES.exponent    = NUM_TIMES.exponent % 10000;
		AccDecn.lsu[1] = NUM_TIMES.exponent / 100;
		AccDecn.lsu[2] = NUM_TIMES.exponent % 100;
		AccDecn.exponent = 5;
	} else
#endif
	if (NUM_TIMES.exponent >= 100){
		AccDecn.lsu[0] = NUM_TIMES.exponent / 100;
		AccDecn.lsu[1] = NUM_TIMES.exponent % 100;
		AccDecn.exponent = 3;
	} else {
		AccDecn.lsu[0] = NUM_TIMES.exponent;
		AccDecn.exponent = 1;
	}
	//check if need to negate
	if (j) { //was negative
		negate_decn(&AccDecn);
	}
#ifdef DEBUG_LOG
	decn_to_str_complete(&AccDecn);
	printf("ln() exponent from initial: %s\n", Buf);
#endif
	//initial exp * ln(10)
	copy_decn(&BDecn, &DECN_LN_10);
	mult_decn();
	//add back stored accum
	copy_decn(&BDecn, &B_j);
	add_decn();

//try not to pollute namespace
#undef B_j
#undef NUM_TIMES
}

void log10_decn(void){
	ln_decn();
	copy_decn(&BDecn, &DECN_LN_10);
	div_decn();
}



void exp_decn(void){
	uint8_t j, k;
	uint8_t need_recip = 0;
#define SAVED Tmp2Decn
#define NUM_TIMES Tmp3Decn

	//check not error
	if (decn_is_nan(&AccDecn)){
		set_dec80_NaN(&AccDecn);
		return;
	}
	//check if negative
	if (AccDecn.exponent < 0){
		negate_decn(&AccDecn);
		need_recip = 1;
	}

	//check if in range
	copy_decn(&SAVED, &AccDecn); //save = accum
	set_dec80_zero(&BDecn);
	BDecn.lsu[0] = 29;
	BDecn.lsu[1] = 47;
	BDecn.exponent = 2; //b = 294.7
	negate_decn(&BDecn);
	add_decn(); //accum = x - 294.7 (should be negative if in range)
	if (!(AccDecn.exponent < 0)){ //if not negative
		set_dec80_NaN(&AccDecn);
		return;
	}
	copy_decn(&AccDecn, &SAVED); //restore

// 	//normalize
// 	remove_leading_zeros(&AccDecn);

	//initial b = -10*ln(10)
	copy_decn(&BDecn, &DECN_LN_10); //b=ln(10)
	copy_decn(&SAVED, &AccDecn); //save = accum
	set_decn_one(&AccDecn);
	AccDecn.exponent = 1; //accum = 10
	mult_decn();             //accum =  10*ln(10)
	copy_decn(&BDecn, &AccDecn); //b =  10*ln(10)
	negate_decn(&BDecn);         //b = -10*ln(10)
	copy_decn(&AccDecn, &SAVED); //restore
	//track number of times 10*ln(10) can be subtracted
	k = 0;
	while (!(AccDecn.exponent < 0)){ //while not negative
		copy_decn(&SAVED, &AccDecn); //save = accum
		//accum -= 10*ln10
		add_decn();
		k++;
	}
	//subtracted 1 time too many
	NUM_TIMES.exponent = (k - 1) * 10; //use exp to store number of times ln(10) subtracted
	copy_decn(&AccDecn, &SAVED); //restore
#ifdef DEBUG_EXP
	decn_to_str_complete(&AccDecn);
	printf("exp() num_times for 10*ln(10): %s (%d)\n", Buf, NUM_TIMES.exponent);
#endif

	//load b with -ln(10)
	copy_decn(&BDecn, &DECN_LN_10); //b =  ln(10)
	negate_decn(&BDecn);            //b = -ln(10)
	//track number of times ln(10) and then (1 + 10^-j) can be subtracted
	j = UINT8_MAX; //becomes 0 after incrementing to start (1 + 10^-j) series
	do {
		k = 0;
		while (!(AccDecn.exponent < 0)){ //while not negative
			copy_decn(&SAVED, &AccDecn); //save = accum
			//accum -= b (ln10, then ln(1 + 10^-j) series)
			add_decn();
#ifdef DEBUG_EXP_ALL
			decn_to_str_complete(&AccDecn);
			printf("    %u: %s\n", k, Buf);
// 			decn_to_str_complete(&BDecn);
// 			printf("(%s)\n", Buf);
#endif
			k++;
#ifdef DEBUG_EXP
			assert(k != 255);
#endif
		}
		//subtracted 1 time too many:
		if (j == UINT8_MAX){
			NUM_TIMES.exponent += k - 1;
		} else {
			NUM_TIMES.lsu[j] = k - 1;
		}
		copy_decn(&AccDecn, &SAVED); //restore
#ifdef DEBUG_EXP
		decn_to_str_complete(&AccDecn);
		printf("exp() num_times for %d: %s (%d)\n", j, Buf, k-1);
#endif
		//next j
		j++;
		if (j < NUM_A_ARR){
			//get next ln(1 + 10^-j) for subtraction
			copy_decn(&BDecn, &LN_A_ARR[j]);
			negate_decn(&BDecn);
		} else {
			break;
		}
	} while (1);

	//build final value
	// (currently accum = save = remainder)
	// calculate 1+remainder
	set_decn_one(&BDecn);
	add_decn();
	//get initial multiplier (10) for ln(10)
	set_decn_one(&BDecn);
	BDecn.exponent = 1; //BDecn = 10
	//do multiplies
	j = UINT8_MAX; //becomes 0 after incrementing to start (1 + 10^-j) series
	do {
		for (k = 0; k < (j==UINT8_MAX ? NUM_TIMES.exponent : NUM_TIMES.lsu[j]); k++){
			mult_decn();
		}
#ifdef DEBUG_EXP
		decn_to_str_complete(&AccDecn);
		printf("exp() current val for %d: %s\n", j, Buf);
#endif
		//next j
		j++;
		if (j < NUM_A_ARR){
			//get next multiplier (1 + 10^-j) for ln(1 + 10^-j)
			if (j == 0){
				//set to 2
				BDecn.lsu[0] = 20;
				BDecn.exponent = 0;
			} else if (j == 1) {
				//set to 1.1
				BDecn.lsu[0] = 11;
				//exponent is already 0
			} else {
				//get next (1 + 10^-j)
				shift_right(&BDecn);
				BDecn.lsu[0] = 10;
			}
		} else {
			break;
		}
	} while (1);

#ifdef DEBUG_EXP
	decn_to_str_complete(&AccDecn);
	printf("exp() before recip: %s\n", Buf);
#endif
	//take reciprocal if exp was negative
	if (need_recip){
		recip_decn();
	}

#ifdef DEBUG_EXP
	decn_to_str_complete(&AccDecn);
	printf("exp() final val: %s\n", Buf);
#endif

//try not to pollute namespace
#undef SAVED
#undef NUM_TIMES
}

void exp10_decn(void){
	//exp10_decn() = exp_decn(AccDecn * ln(10))
	copy_decn(&BDecn, &DECN_LN_10);
	mult_decn();
	exp_decn();
}

void pow_decn(void) {
	if (decn_is_zero(&BDecn)) {
		set_decn_one(&AccDecn);
		return;
	}
	if (decn_is_zero(&AccDecn)) {
		set_dec80_zero(&AccDecn);
		return;
	}
	//calculate AccDecn = AccDecn ^ BDecn
	st_push_decn(&BDecn);
	ln_decn();
	st_pop_decn(&BDecn);
	mult_decn(); //accum = b*ln(accum)
	exp_decn();
}

#ifdef USE_POW_SQRT_IMPL
void sqrt_decn(void) {
	if (decn_is_zero(&AccDecn)) {
		return;
	}
	if (decn_is_nan(&AccDecn)) {
		return;
	}
	if (AccDecn.exponent < 0){ //negative
		set_dec80_NaN(&AccDecn);
		return;
	}
	st_push_decn(&BDecn); // sqrt should behave like an unary operation
	//b = 0.5
	set_dec80_zero(&BDecn);
	BDecn.lsu[0] = 5;
	pow_decn();
	st_pop_decn(&BDecn);
}
#else
void sqrt_decn(void){
#define CURR_EST Tmp2Decn //holds current 1/sqrt(x) estimate
#define X_2      Tmp3Decn //holds copy of original x / 2
	uint8_t i;
	exp_t initial_exp;
	if (decn_is_nan(&AccDecn)) {
		return;
	}
	if (AccDecn.exponent < 0){ //negative
		set_dec80_NaN(&AccDecn);
		return;
	}
	//normalize
	remove_leading_zeros(&AccDecn);
#ifdef DEBUG_SQRT
	decn_to_str_complete(&AccDecn);
	printf("sqrt in: %s\n", Buf);
#endif
	//store copy of x
	st_push_decn(&AccDecn);
	//calculate x_orig / 2
	set_dec80_zero(&BDecn);
	BDecn.lsu[0] = 5;
	mult_decn();
	copy_decn(&X_2, &AccDecn);
	//restore x
	st_load_decn(&AccDecn);
	//get initial estimate for 1/sqrt(x) == 10^(-0.5 * log(x)):
	// approximate significand == 10^(-0.5 * log(x_signif))
	//  with linear approximation: -0.18 * x_signif + 2.5
	// new exponent part is (10^(-0.5 * log(10^x_exp)))
	//                    == 10^(-0.5 * x^exp)
	initial_exp = get_exponent(&AccDecn);
	set_exponent(&AccDecn, 0, 0); //clear exponent (Acc is not negative)
#ifdef DEBUG_SQRT
	printf("sqrt exponent %d ", initial_exp);
#endif
	if (initial_exp & 0x1){ //odd
#ifdef DEBUG_SQRT
		printf("(odd) ");
#endif
		//increment x_exp and
		initial_exp++;
		//approximate estimated significand as (-0.056*x_signif + 0.79) * 10^0.5
		//                                  == -0.18 * x_signif + 2.5
		//b = -0.18
		BDecn.lsu[0] = 18;
		BDecn.exponent = -1; //negative, and exponent = -1
		//a = -0.18 * x_signif
		mult_decn();
		//b = 2.5
		BDecn.lsu[0] = 25;
		BDecn.exponent = 0;
		//a = -0.18 * x_signif + 2.5
		add_decn();
	} else { //even
		//keep x_exp as is and approximate estimated significand as
		//                   -0.056*x_signif + 0.79
		//b = -0.056
		BDecn.lsu[0] = 56;
		set_exponent(&BDecn, -2, 1);
		//a = -0.056 * x_signif
		mult_decn();
		//b = 0.79
		BDecn.lsu[0] = 7;
		BDecn.lsu[1] = 90;
		BDecn.exponent = 0;
		//a = -0.056*x_signif + 0.79
		add_decn();
	}
	//est_exp = -x_exp / 2;
	initial_exp = -initial_exp / 2;
	//est_exp-- if AccDecn exponent is negative
	// (AccDecn exponent is either 0 or -1, and AccDecn is positive)
	if (AccDecn.exponent != 0){
		initial_exp--;
	}
	set_exponent(&AccDecn, initial_exp, 0); //(initial estimate is never negative)
	copy_decn(&CURR_EST, &AccDecn);
#ifdef DEBUG_SQRT
	printf(" -> %d\n", initial_exp);
#endif
	//do newton-raphson iterations
	for (i = 0; i < 6; i++){ //just fix number of iterations for now
#ifdef DEBUG_SQRT
		decn_to_str_complete(&CURR_EST);
		printf("sqrt %2d: %s\n", i, Buf);
#endif
		//accum = est * est;
		copy_decn(&BDecn, &AccDecn);
		mult_decn();
		//accum *= x_orig_2;     //accum = x/2 * est * est
		copy_decn(&BDecn, &X_2);
		mult_decn();
		//accum = - x/2 * est * est
		negate_decn(&AccDecn);
		//b = 3/2
		set_dec80_zero(&BDecn);
		BDecn.lsu[0] = 15;
		//accum = 3/2 - x/2 * est * est
		add_decn();
		//accum *= est;          //accum = 0.5 * est * (3 - x * est * est)
		copy_decn(&BDecn, &CURR_EST);
		mult_decn();
		//est = accum;
		copy_decn(&CURR_EST, &AccDecn);
	}

	//calc sqrt from recip_sqrt
	st_pop_decn(&BDecn);
	mult_decn();

#undef CURR_EST
#undef X_COPY
}
#endif //USE_POW_SQRT_IMPL


// normal angle to between 0 and 360 degrees
void normalize_0_360(void) {
	const uint8_t is_negative = (AccDecn.exponent < 0);
	exp_t exponent;

	remove_leading_zeros(&AccDecn);
	if (is_negative) {
		negate_decn(&AccDecn);
	}
	exponent = get_exponent(&AccDecn);
	//B = 360
	set_dec80_zero(&BDecn);
	BDecn.lsu[0] = 36;
	BDecn.exponent = 2;
	if (compare_magn() > 0) {
		do {
			do {
				//B = 3.6e...
				BDecn.exponent = exponent;
				if (compare_magn() >= 0) {
					negate_decn(&BDecn);
					add_decn();
				} else {
					break;
				}
			} while (1);
			exponent--;
		} while (exponent >= 2);
	}

	if (is_negative) {
		negate_decn(&AccDecn);
		//B = 360
		BDecn.exponent = 2;
		add_decn();
	}
}

// K. Shirriff, "Reversing Sinclair's amazing 1974 calculator hack - half the ROM of the HP-35"
// http://files.righto.com/calculator/sinclair_scientific_simulator.html
#define SIN Tmp2Decn
#define COS Tmp3Decn
#define THETA Tmp4Decn
void sincos_decn(const uint8_t sincos_arctan) {
	const uint8_t is_negative = AccDecn.exponent < 0;
	if (sincos_arctan) { //calculate arctan
		set_dec80_zero(&THETA);
		if (is_negative) negate_decn(&AccDecn);
		copy_decn(&COS, &AccDecn);
		set_decn_one(&SIN);
	} else { //calculate sin/cos
		normalize_0_360();
		to_radian_decn();
		copy_decn(&THETA, &AccDecn);
		set_decn_one(&COS);
		set_dec80_zero(&SIN);
		// 0.0 00 5
		SIN.lsu[2] = 50;
		negate_decn(&SIN);
	}
	do {
		if (sincos_arctan) { //calculate arctan
			// THETA is in AccDecn from previous iteration
			if (COS.exponent < 0) {
				if (is_negative) negate_decn(&AccDecn);
				break;
			}
		} else { //calculate sin/cos
			if (THETA.exponent < 0) {
				break;
			}
		}
		// COS = COS - SIN / 1000
		copy_decn(&AccDecn, &COS);
		copy_decn(&BDecn, &SIN);
		shift_right(&BDecn);
		shift_right(&BDecn);
		shift_right(&BDecn);
		negate_decn(&BDecn);
		add_decn();
		copy_decn(&COS, &AccDecn);
		// SIN = SIN + COS / 1000
		copy_decn(&AccDecn, &SIN);
		copy_decn(&BDecn, &COS);
		shift_right(&BDecn);
		shift_right(&BDecn);
		shift_right(&BDecn);
		add_decn();
		copy_decn(&SIN, &AccDecn);
		// THETA = THETA -/+ 0.0 01
		copy_decn(&AccDecn, &THETA);
		set_dec80_zero(&BDecn);
		BDecn.lsu[1] = 1;
		if (!sincos_arctan) negate_decn(&BDecn);
		add_decn();
		copy_decn(&THETA, &AccDecn);
	} while (1);
}

void sin_decn(void) {
	sincos_decn(0);
	copy_decn(&AccDecn, &SIN);
}

void cos_decn(void) {
	sincos_decn(0);
	copy_decn(&AccDecn, &COS);
}

void tan_decn(void) {
	sincos_decn(0);
	copy_decn(&AccDecn, &SIN);
	copy_decn(&BDecn, &COS);
	div_decn();
}

void arctan_decn(void) {
	sincos_decn(1);
	to_degree_decn();
}

// see W.E. Egbert, "Personal Calculator Algorithms III: Inverse Trigonometric Functions"
void arcsin_decn_rad(void) {
	st_push_decn(&AccDecn);
	copy_decn(&BDecn, &AccDecn);
	mult_decn();
	negate_decn(&AccDecn);
	set_decn_one(&BDecn);
	add_decn();
	sqrt_decn();
	recip_decn();
	st_pop_decn(&BDecn);
	mult_decn();
	sincos_decn(1);
}

void arcsin_decn(void) {
	arcsin_decn_rad();
	to_degree_decn();
}

void arccos_decn(void) {
	arcsin_decn_rad();
	negate_decn(&AccDecn);
	copy_decn(&BDecn, &DECN_PI2);
	add_decn();
	to_degree_decn();
}
#undef SIN
#undef COS
#undef THETA

void to_degree_decn(void) {
	copy_decn(&BDecn, &DECN_1RAD);
	mult_decn();
}

void to_radian_decn(void) {
	copy_decn(&BDecn, &DECN_1RAD);
	div_decn();
}

void pi_decn(void) {
	set_dec80_zero(&BDecn);
	copy_decn(&AccDecn, &DECN_PI);
	// mult_decn();  
}

static void set_str_error(void){
	Buf[0] = 'E';
	Buf[1] = 'r';
	Buf[2] = 'r';
	Buf[3] = 'o';
	Buf[4] = 'r';
	Buf[5] = '\0';
}

#ifdef DESKTOP
int
#else
int8_t
#endif
decn_to_str(const dec80* x){
#define INSERT_DOT() Buf[i++]='.'
	uint8_t i = 0;
	uint8_t digit100;
	exp_t exponent = 0;
	uint8_t trailing_zeros = 0;
	uint8_t use_sci = 0;
	static __xdata dec80 tmp;

	//handle corner case of NaN
	if (decn_is_nan(x)){
		set_str_error();
#ifdef DEBUG
		printf ("  corner case NaN ");
#endif
		return 0;
	}

	//copy and normalize
	copy_decn(&tmp, x);
	remove_leading_zeros(&tmp);
	//handle corner case of 0
	if (tmp.lsu[0] == 0){
#ifdef DEBUG
		printf ("  corner case, set to 0  ");
#endif
		Buf[0] = '0';
		Buf[1] = '\0';
		return 0;
	}
	//check sign of number
	if (tmp.exponent < 0){
#ifdef DEBUG
		printf ("  negative  ");
#endif
		Buf[i] = '-';
		i++;
	}
	//check if we should use scientific notation
	exponent = get_exponent(&tmp);
	if (exponent > (num_digits_display - 1) || exponent < -3){
		use_sci = 1;
	}
	//pad zeros right of decimal point if needed
	if (!use_sci && exponent < 0){
		exp_t j;
		Buf[i] = '0';
		i++;
		INSERT_DOT();
		//pad zeros right of decimal point
//		for (j = exponent + 1; j < 0; j++){ <--- results in undefined behavior (signed overflow), and causes crash
		for (j = -exponent -1; j > 0; --j){
			Buf[i] = '0';
			i++;
		}
	}
	//print 1st digit
	Buf[i] = (tmp.lsu[0] / 10) + '0';
	i++;
	if (use_sci) {
		INSERT_DOT();
	} else {
		if (exponent == 0){
			INSERT_DOT();
		}
		exponent--;
	}
	//print 2nd digit
	Buf[i] = (tmp.lsu[0] % 10) + '0';
	if (tmp.lsu[0] % 10 == 0 && (use_sci || exponent < 0)){

		trailing_zeros = 1;
	}
	i++;
	if (!use_sci){
		if (exponent == 0){
			INSERT_DOT();
		}
		exponent--;
	}
	//print rest of significand
	for (digit100 = 1 ; digit100 < num_digits_display/2; digit100++){
		//print 1st digit
		Buf[i] = (tmp.lsu[digit100] / 10) + '0';
		i++;
		if (!use_sci){
			if (exponent == 0){
				INSERT_DOT();
			}
			exponent--;
		}
		//print 2nd digit
		Buf[i] = (tmp.lsu[digit100] % 10) + '0';
		i++;
		if (!use_sci){
			if (exponent == 0){
				INSERT_DOT();
			}
			exponent--;
		}
		//track trailing 0s
		if (tmp.lsu[digit100] == 0 && (use_sci || exponent < 0)){
			if (use_sci || exponent < -2){ //xx.00
				trailing_zeros += 2;
			} else if (exponent == -2){ //xx.0
				trailing_zeros += 1;
			}
		} else if (tmp.lsu[digit100]%10 == 0 && (use_sci || exponent < 0)){
			trailing_zeros = 1;
		} else {
			trailing_zeros = 0;
		}
	}

	//remove trailing zeros
	if (use_sci || exponent <= 0){
		i -= trailing_zeros;
	}
	Buf[i] = '\0';

	//calculate exponent
	exponent = get_exponent(&tmp); //base 100
#ifdef DEBUG
	printf ("  exponent (%d)", exponent);
#endif
	//print exponent
	if (use_sci){
		//check for overflow
#ifdef DESKTOP
		if (exponent > DEC80_MAX_EXP || exponent < DEC80_MIN_EXP){
#else
		if (exponent > DECN_MAX_PRINT_EXP || exponent < DECN_MIN_PRINT_EXP){
#endif
			set_str_error();
			return 0;
		}
		return exponent;
	} else {
		return 0;
	}

#ifdef DEBUG
	printf ("  final i (%d)  ", i);
	for (int jjj = 0; jjj < DEC80_NUM_LSU; jjj++){
		printf(" %02d", tmp.lsu[jjj]);
	}
	printf("\n");
#endif
}

#ifdef DESKTOP
//complete string including exponent
void decn_to_str_complete(const dec80* x){
	int exponent = decn_to_str(x);
	int i;
	//find end of string
	for (i = 0; Buf[i] != '\0'; i++);
	//add exponent
	if (exponent != 0){
		Buf[i++] = 'E';
		if (exponent < 0){
			Buf[i++] = '-';
			u32str(-exponent, &Buf[i], 10); //adds null terminator automatically
		} else {
			u32str(exponent, &Buf[i], 10); //adds null terminator automatically
		}
	}
}

void build_decn_at(dec80* dest, const char* signif_str, exp_t exponent){
	dec80 tmp;
	copy_decn(&tmp, &AccDecn); //save
	build_dec80(signif_str, exponent);
	copy_decn(dest, &AccDecn);
	copy_decn(&AccDecn, &tmp); //restore
}

#endif //DESKTOP

