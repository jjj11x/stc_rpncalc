/*
 * decn.c
 *
 *  Created on: Mar 21, 2019
 *      Author: jeffrey
 */

#include "../utils.h"

#include "decn.h"


#define EXTRA_CHECKS
// #define DEBUG
// #define DEBUG_COMPARE_MAGN
// #define DEBUG_ADD
// #define DEBUG_MULT
// #define DEBUG_MULT_ALL //even more verbose
// #define DEBUG_DIV

#ifndef DESKTOP
//#undef EXTRA_CHECKS
#undef DEBUG
#undef DEBUG_COMPARE_MAGN
#undef DEBUG_ADD
#undef DEBUG_MULT
#undef DEBUG_DIV
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

__idata dec80 AccDecn, BDecn;
__idata dec80 TmpDecn;

__xdata char Buf[DECN_BUF_SIZE];


void copy_decn(dec80* dest, const dec80* src){
	uint8_t i;
	dest->exponent = src->exponent;

	//copy nibbles
	for (i = 0; i < DEC80_NUM_LSU; i++){
		dest->lsu[i] = src->lsu[i];
	}
}

exp_t get_exponent(const dec80* x){
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

static void set_exponent(dec80* acc, exp_t exponent, uint8_t num_is_neg){
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

static void shift_right(dec80* x){
	uint8_t high = 0, low = 0, old_low = 0;
	uint8_t i;
	for (i = 0; i < DEC80_NUM_LSU; i++){
		high = x->lsu[i] / 10;
		low = x->lsu[i] % 10;
		x->lsu[i] = high + (old_low*10);
		old_low = low;
	}
}

static void shift_left(dec80* x){
	uint8_t high = 0, low = 0, old_high = 0;
	int8_t i;
	for (i = DEC80_NUM_LSU - 1; i >= 0 ; i--){
		high = x->lsu[i] / 10;
		low = x->lsu[i] % 10;
		x->lsu[i] = old_high + (low*10);
		old_high = high;
	}
}

static void remove_leading_zeros(dec80* x){
	uint8_t digit100;
	uint8_t is_negative = (x->exponent < 0);
	exp_t exponent = get_exponent(x);

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

void build_dec80(__xdata const char* signif_str, exp_t exponent){
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
				int8_t new_exponent;
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
					assert(DEC80_NUM_LSU*2 > num_lr_points);
					new_exponent = exponent + (num_lr_points - 1); //1 digit left of implicit point
					//check for overflow
#ifdef EXTRA_CHECKS
					if (new_exponent < exponent || exponent > DEC80_MAX_EXP){
	#ifdef DEBUG
						printf("   overflow (new_exp, exp)=(%d,%d)\n",
						         new_exponent, exponent);
	#endif
						set_dec80_NaN(&AccDecn);
						return;
					}
#endif
				} else if (num_lr_points < 0) { //right count exists
					// (num_lr_points represents exponent shift)
					// (this ends up being a subtraction)
					new_exponent = exponent + num_lr_points;
					//check for underflow
#ifdef EXTRA_CHECKS
					if (new_exponent > exponent || exponent < DEC80_MIN_EXP){
	#ifdef DEBUG
						printf("   underflow (new_exp, exp)=(%d,%d)\n",
						         new_exponent, exponent);
	#endif
						set_dec80_NaN(&AccDecn);
						return;
					}
#endif
				} else {
					//no change
					new_exponent = exponent;
				}
				exponent = new_exponent;
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

static uint8_t decn_is_zero(const dec80* x){
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
		a_i < DEC80_NUM_LSU && b_i < DEC80_NUM_LSU;
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
//acc must be larger than x in absolute value
//subtract by equal addition algorithm
static void sub_mag(dec80* acc, const dec80* x){
	int8_t i;
	uint8_t carry = 0;
	static __xdata dec80 tmp;
	copy_decn(&tmp, x);
	//normalize
	remove_leading_zeros(acc);
	remove_leading_zeros(&tmp);
	if (get_exponent(acc) != get_exponent(&tmp)){
		_incr_exp(&tmp, get_exponent(acc));
	}
#ifdef DEBUG_ADD
	decn_to_str_complete(&tmp);
	printf("        incr_exp tmp: %s\n", Buf);
#endif
	//do subtraction
	for (i = DEC80_NUM_LSU - 1; i >=0; i--){
		uint8_t digit100;
		if (acc->lsu[i] >= (tmp.lsu[i] + carry)){
			digit100 = acc->lsu[i] - (tmp.lsu[i] + carry);
			carry = 0;
		} else {
			digit100 = acc->lsu[i] + 100 - (tmp.lsu[i] + carry);
			carry = 1;
		}
		assert(digit100 < 100);
		acc->lsu[i] = digit100;
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
	//handle cases where signs differ
	if (AccDecn.exponent < 0 && BDecn.exponent >= 0){
		// -acc, +x
		rel = compare_magn();
		if (rel == 1){
#ifdef DEBUG_ADD
			printf("|-acc| > |+x|\n");
#endif
			sub_mag(&AccDecn, &BDecn);
			return;
		} else if (rel == -1){
#ifdef DEBUG_ADD
			printf("|-acc| < |+x|\n");
#endif
			copy_decn(&TmpDecn, &BDecn);
			sub_mag(&TmpDecn, &AccDecn);
			copy_decn(&AccDecn, &TmpDecn);
			return;
		} else { //equal
#ifdef DEBUG_ADD
			printf("|-acc| == |+x|\n");
#endif
			set_dec80_zero(&AccDecn);
			return;
		}
	} else if (AccDecn.exponent >= 0 && BDecn.exponent < 0){
		// +acc, -x
		rel = compare_magn();
		if (rel == 1){
#ifdef DEBUG_ADD
			printf("|+acc| > |-x|\n");
#endif
			sub_mag(&AccDecn, &BDecn);
			return;
		} else if (rel == -1){
#ifdef DEBUG_ADD
			printf("|+acc| < |-x|\n");
#endif
			copy_decn(&TmpDecn, &BDecn);
			sub_mag(&TmpDecn, &AccDecn);
			copy_decn(&AccDecn, &TmpDecn);
			return;
		} else { //equal
#ifdef DEBUG_ADD
			printf("|+acc| == |-x|\n");
#endif
			set_dec80_zero(&AccDecn);
			return;
		}
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
		assert(carry < 100);
	}
	//may need to rescale number
	if (carry > 0){
		exp_t curr_exp = get_exponent(&AccDecn);
		rel = (AccDecn.exponent < 0); //is_neg?
#ifdef DEBUG_ADD
		printf("        carry out: %d", carry);
#endif
		//shift right
		if (carry < 10){
			shift_right(&AccDecn);
			AccDecn.lsu[0] += carry*10; //carry gets shifted into most significant digit
			curr_exp++;
		} else {
			shift_right(&AccDecn);
			shift_right(&AccDecn);
			AccDecn.lsu[0] = carry;
			curr_exp+=2;
		}
		//track sign
		set_exponent(&AccDecn, curr_exp, rel); //rel==is_neg?
	}
}

void mult_decn(void){
	int8_t i, j;
	uint8_t carry = 0;
	uint8_t is_neg;
	exp_t new_exponent;
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
		printf("\n new exponent: %d:", new_exponent);
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

void div_decn(void){
	static __xdata dec80 curr_recip; //copy of x, holds current 1/x estimate
	static __xdata dec80 x_copy; //holds copy of original x
	static __xdata dec80 acc_copy; //holds copy of original acc
	uint8_t i;
	exp_t initial_exp;
	//check divide by zero
#ifdef EXTRA_CHECKS
	if (decn_is_zero(&BDecn)){
		set_dec80_NaN(&AccDecn);
#ifdef DESKTOP
		printf("error division by 0\n");
#endif
		return;
	}
#endif
	//normalize
	remove_leading_zeros(&AccDecn);
	remove_leading_zeros(&BDecn);
	//store copy of acc for final multiply by 1/x
	copy_decn(&acc_copy, &AccDecn);
	//store copy of x
	copy_decn(&x_copy, &BDecn);
	//get initial exponent of estimate for 1/x
	initial_exp = get_exponent(&BDecn);
#ifdef DEBUG_DIV
	printf("exponent %d", initial_exp);
#endif
	//necessary to subtract 1 for convergence
	initial_exp = -initial_exp - 1;
#ifdef DEBUG_DIV
	printf(" -> %d\n", initial_exp);
#endif
	set_exponent(&curr_recip, initial_exp, (BDecn.exponent < 0)); //set exponent, copy sign
	//get initial estimate for 1/x
	if        (BDecn.lsu[0] < 20){       //mantissa between 1 and 2
		curr_recip.lsu[0] = 50; //0.50 with implicit point and exponent
	} else if (BDecn.lsu[0] < 33){
		curr_recip.lsu[0] = 30;
	} else if (BDecn.lsu[0] < 50){
		curr_recip.lsu[0] = 20;
	} else {
		curr_recip.lsu[0] = 10; //0.1 with implicit point and exponent
	}
	for (i = 1; i < DEC80_NUM_LSU; i++){
		curr_recip.lsu[i] = 0;
	}
	copy_decn(&AccDecn, &curr_recip);
	//do newton raphson iterations
	for (i = 0; i < 6; i++){ //just fix number of iterations for now
#ifdef DEBUG_DIV
		decn_to_str_complete(&curr_recip);
		printf("%2d: %s\n", i, Buf);
#endif
		copy_decn(&BDecn, &x_copy);
		mult_decn();
#ifdef DEBUG_DIV
		decn_to_str_complete(&AccDecn);
		printf("  %20s: %s\n", "recip*x", Buf);
#endif
		negate_decn(&AccDecn);
		copy_decn(&BDecn, &DECN_1);
		add_decn();
#ifdef DEBUG_DIV
		decn_to_str_complete(&AccDecn);
		printf("  %20s: %s\n", "(1-recip*x)", Buf);
#endif
		copy_decn(&BDecn, &curr_recip);
		mult_decn();
#ifdef DEBUG_DIV
		decn_to_str_complete(&AccDecn);
		printf("  %20s: %s\n", "recip * (1-recip*x)", Buf);
#endif
		add_decn();
		//new_est(acc) = recip + (1 - recip*x)*recip, where tmp is current recip estimate
		copy_decn(&curr_recip, &AccDecn);
	}
	//acc now holds 1/x, multiply by original acc to complete division
	copy_decn(&BDecn, &acc_copy);
	mult_decn();
}

static void set_str_error(){
	Buf[0] = 'E';
	Buf[1] = 'r';
	Buf[2] = 'r';
	Buf[3] = 'o';
	Buf[4] = 'r';
	Buf[5] = '\0';
}

int8_t decn_to_str(const dec80* x){
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
	for (digit100 = 1 ; digit100 < DEC80_NUM_LSU; digit100++){
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
		if (exponent > DEC80_MAX_EXP || exponent < DEC80_MIN_EXP){
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
	int8_t exponent = decn_to_str(x);
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


