/*
 * decn.c
 *
 *  Created on: Mar 21, 2019
 *      Author: jeffrey
 */

#include "../utils.h"

//#define DEBUG
#define DEBUG_ADD

#ifndef DESKTOP
#undef DEBUG
#undef DEBUG_ADD
#endif

#ifdef DESKTOP
#include <stdio.h>
#endif

#ifdef DESKTOP
#include <assert.h>
#else
#define assert(x)
#endif

#include "decn.h"


void dec64to80(dec80* dest, const dec64* src){
	uint8_t i;
	dest->exponent = src->exponent;

	//clear extra nibbles
	for (i = DEC80_NUM_LSU - 1; i >= DEC64_NUM_LSU; i--){
		dest->lsu[i] = 0;
	}

	//copy nibbles
	for (i = 0; i < DEC64_NUM_LSU; i++){
		dest->lsu[i] = src->lsu[i];
	}
}

void dec80to64(dec64* dest, const dec80* src){
	//TODO:
	assert(0);
}

void copy_decn(dec80* dest, const dec80* src){
	uint8_t i;
	dest->exponent = src->exponent;

	//copy nibbles
	for (i = 0; i < DEC80_NUM_LSU; i++){
		dest->lsu[i] = src->lsu[i];
	}
}

int16_t get_exponent(const dec80* x){
	int16_t exponent = x->exponent;
	if (exponent & 0x4000){ //negative
		return (uint16_t) exponent | 0x8000;
	} else { //positive
		return (uint16_t) exponent & 0x7fff;
	}
}

#define get_nibble(lsu, nibble) \
	(((nibble) & 1) ? (((lsu)[(nibble)/2] >> 4) & 0x0f) : ((lsu)[(nibble)/2] & 0x0f))

#define set_nibble(lsu, nibble, val) do { \
	if ((nibble) & 1){ \
		(lsu)[(nibble)/2] = ((lsu)[(nibble)/2] & 0x0f) | (((val) & 0xf) << 4); \
	} else { \
		(lsu)[(nibble)/2] = ((lsu)[(nibble)/2] & 0xf0) | ((val) & 0xf); \
	} \
} while (0)

static void _zero_remaining_decn(dec80* dest, uint8_t nibble, uint8_t NUM_LSU){
	while (nibble < NUM_LSU*2){
		if (nibble & 1){ //odd
			set_nibble(dest->lsu, nibble, 0);
			nibble++;
		} else {
			dest->lsu[nibble/2] = 0;
			nibble += 2;
		}
	}
}

static void zero_remaining_64(dec64* dest, uint8_t nibble){
	_zero_remaining_decn((dec80*) dest, nibble, DEC64_NUM_LSU);
}

static void zero_remaining_80(dec80* dest, uint8_t nibble){
	_zero_remaining_decn(dest, nibble, DEC80_NUM_LSU);
}

static void remove_leading_zeros(dec80* x){
	uint8_t nibble;

	//find first non-zero nibble
	for (nibble = 0; nibble < DEC80_NUM_LSU*2; nibble++){
		if (get_nibble(x->lsu, nibble) != 0){
			break;
		}
	}

	if (nibble < DEC80_NUM_LSU*2){ //found non-zero nibble
		uint8_t i;
		//copy nibbles
		for (i = 0; nibble < DEC80_NUM_LSU*2; i++, nibble++){
			set_nibble(x->lsu, i, get_nibble(x->lsu, nibble));
		}
		//zero out remaining nibbles, now that number left-aligned
		zero_remaining_80(x, i);
	}
}


void build_dec64(dec64* dest, const char* signif_str, int16_t exponent){
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
	uint8_t nibble = 0;
	int8_t num_lr_points = 0; //number of digits to the right (-) or left of decimal point
	int8_t curr_sign = SIGN_ZERO;

	//check first digit
	if (signif_str[0] == '\0'){
		set_dec64_zero(dest);
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
			} else {
				//multiple '.'s in string
#ifdef DEBUG
				printf("  ERROR: multiple '.'s in string\n");
#endif
				set_dec64_NaN(dest);
				return;
			}
		} else if (signif_str[i] >= '1' && signif_str[i] <= '9'){
			if (nibble < DEC64_NUM_LSU*2){
				set_nibble(dest->lsu, nibble, signif_str[i] - '0');
			}
			nibble++;
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
			//track number digits L/R of decimal point
			if (SEEN_POINT(curr_sign)){
				if (num_lr_points <= 0){
					//seen decimal point, and no left count (or right count already exists)
					num_lr_points--; //increase right count
				}
			} else { //haven't seen decimal point yet
				num_lr_points++; //increase left count
			}
		} else if (signif_str[i] == '0'){
			//make sure not a leading zero
			if (!IS_ZERO(curr_sign)){ //non-zero value
				if (nibble < DEC64_NUM_LSU*2){
					set_nibble(dest->lsu, nibble, 0);
				}
				nibble++;
			}
			//track number digits L/R of decimal point
			if (SEEN_POINT(curr_sign)){
				if (num_lr_points == 0){ //no left count exists
					num_lr_points--; //increase right count
				}
			} else { //haven't seen decimal point yet
				if (!IS_ZERO(curr_sign)){ //not a leading zero
					num_lr_points++; //increase left count
				}
			}
		} else if (signif_str[i] == '\0'){ //done
			if (IS_ZERO(curr_sign)){
				//zero
				set_dec64_zero(dest);
				return;
			} else {
				//not zero, adjust exponent for left-aligned significand input
				//          or for number of digits past decimal point
				int8_t new_exponent;
				if (num_lr_points > 0){ //left count exists
					assert(DEC64_NUM_LSU*2 > num_lr_points);
					new_exponent = exponent - ((DEC64_NUM_LSU*2) - num_lr_points);
				} else if (num_lr_points < 0) { //right count exists
					// (-num_past_point represents #digits right of decimal)
					// (this ends up being a subtraction)
					new_exponent = exponent + num_lr_points;
				} else {
					//no change
					new_exponent = exponent;
				}
				//check for underflow
				if (new_exponent > exponent || exponent < DEC64_MIN_EXP){
#ifdef DEBUG
					printf("   underflow (new_exp, exp)=(%d,%d)\n",
					         new_exponent, exponent);
#endif
					set_dec64_NaN(dest);
					return;
				}
				//check for overflow
				if (exponent > DEC64_MAX_EXP){
#ifdef DEBUG
					printf("   overflow (new_exp, exp)=(%d,%d)\n",
					         new_exponent, exponent);
#endif
					set_dec64_NaN(dest);
					return;
				}
				exponent = new_exponent;
				if (IS_NEG(curr_sign)){
					exponent |= 0x8000;
				} else {
					exponent &= 0x7fff;
				}
				dest->exponent = exponent;
				zero_remaining_64(dest, i);
#ifdef DEBUG
				printf("   num_lr_points (%d), new_exp (%d), sign (%d), exp (%d)\n",
				        num_lr_points, new_exponent, curr_sign, exponent);
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


void set_dec64_zero(dec64* dest){
	uint8_t i;

	//clear exponent
	dest->exponent = 0;
	//clear nibbles
	for (i = 0; i < DEC64_NUM_LSU; i++){
		dest->lsu[i] = 0;
	}
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

void set_dec64_NaN(dec64* dest){
	uint8_t i;

	//clear exponent
	dest->exponent = 0xff;
	//clear nibbles
	for (i = 0; i < DEC64_NUM_LSU; i++){
		dest->lsu[i] = 0;
	}
}

void set_dec80_NaN(dec80* dest){
	uint8_t i;

	//clear exponent
	dest->exponent = 0xff;
	//clear nibbles
	for (i = 0; i < DEC80_NUM_LSU; i++){
		dest->lsu[i] = 0;
	}
}

void negate_decn(dec80* x){
	static const int16_t xor_val = 0x8000;
	(x->exponent) ^= xor_val;
}

int8_t compare_magn(const dec80* a, const dec80* b){ //a<b: -1, a==b: 0, a>b: 1
	uint8_t a_i, b_i;
	int16_t a_exp=0, b_exp=0;
	uint8_t a_trailing_zeros=0, b_trailing_zeros=0;
	int8_t a_signif_b = 0; //a<b: -1, a==b: 0, a>b: 1
	//discard leading zeros
	for (a_i = 0; a_i < DEC80_NUM_LSU*2; a_i++){
		if (get_nibble(a->lsu, a_i) != 0){
			break;
		}
	}
	for (b_i = 0; b_i < DEC80_NUM_LSU*2; b_i++){
		if (get_nibble(b->lsu, b_i) != 0){
			break;
		}
	}
	//compare signifcands while tracking magnitude
	for ( ; a_i < DEC80_NUM_LSU*2 && b_i < DEC80_NUM_LSU*2; a_i++, b_i++, a_exp++, b_exp++){
		//set signif. inequality if this is first digit that is different
		if (a_signif_b == 0 && (get_nibble(a->lsu, a_i) < get_nibble(b->lsu, b_i))){
			a_signif_b = -1;
		} else if (a_signif_b == 0 && (get_nibble(a->lsu, a_i) > get_nibble(b->lsu, b_i))){
			a_signif_b = 1;
		}
		//track trailing zeros (a)
		if (get_nibble(a->lsu, a_i) == 0){
			a_trailing_zeros++;
		} else {
			a_trailing_zeros = 0;
		}
		//                     (b)
		if (get_nibble(b->lsu, b_i) == 0){
			b_trailing_zeros++;
		} else {
			b_trailing_zeros = 0;
		}
	}
	//done with at least one, make sure both are done (a)
	for ( ; a_i < DEC80_NUM_LSU*2; a_i++, a_exp++){
		//track trailing zeros
		if (get_nibble(a->lsu, a_i) == 0){
			a_trailing_zeros++;
		} else {
			a_trailing_zeros = 0;
		}
	}
	//                                                (b)
	for ( ; b_i < DEC80_NUM_LSU*2; b_i++, b_exp++){
		//track trailing zeros
		if (get_nibble(b->lsu, b_i) == 0){
			b_trailing_zeros++;
		} else {
			b_trailing_zeros = 0;
		}
	}
	//calculate exponents
	a_exp += get_exponent(a);
	b_exp += get_exponent(b);
	a_exp -= a_trailing_zeros;
	b_exp -= b_trailing_zeros;
	//compare exponents
	if (a_exp > b_exp){
		return 1;
	} else if (a_exp < b_exp){
		return -1;
	}
	//exponents equal, compare by significand
	return a_signif_b;
}

int8_t compare_decn(const dec80* a, const dec80* b){ //a<b: -1, a==b: 0, a>b: 1
	int8_t is_neg;

	//handle zero special cases
	if (decn_is_zero(a) && decn_is_zero(b)){
		return 0;
	} else if (decn_is_zero(a)){
		if (b->exponent < 0){
			return 1;
		} else {
			return -1;
		}
	} else if (decn_is_zero(b)){
		if (a->exponent < 0){
			return -1;
		} else {
			return 1;
		}
	}
	//handle cases where signs differ
	if (a->exponent < 0 && b->exponent > 0){
		return -1;
	} else if (a->exponent > 0 && b->exponent < 0){
		return 1;
	}
	//signs must now be the same, either both pos, or both neg
	is_neg = (a->exponent < 0 ? -1 : 1);

	return is_neg * compare_magn(a, b);
}

static void sub_mag(dec80* acc, const dec80* x){

}

//WARNING: for add_decn() function only
//rescales acc up to exponent (increase exponent of acc in abs value, while doing shifts)
//the actual value of acc->exponent remains unchanged
//both acc, and the number from which exponent was taking must be stripped of leading 0s first
static void _incr_exp(dec80* acc, int16_t exponent){
	int16_t curr_exp = get_exponent(acc);
	uint8_t is_neg = (acc->exponent < 0 ? 1 : 0);
#ifdef DEBUG_ADD
	printf("   (is_neg,curr_exp,exponent)=(%d,%d,%d)\n",
	        is_neg, curr_exp, exponent);
#endif
	assert(exponent > curr_exp);
	while (curr_exp != exponent){
		//shift right
		int8_t i;
		for (i = DEC80_NUM_LSU*2 - 1 - 1; i >= 0; i--){
			set_nibble(acc->lsu, i + 1, get_nibble(acc->lsu, i));
		}
		acc->lsu[0] &= 0xf0; //0 gets shifted into most significant digit
		curr_exp++;
	}

	//curr_exp does NOT get written back to acc->exponent
}

void add_decn(dec80* acc, const dec80* x){
	dec80 tmp;
	int8_t rel;
	uint8_t carry = 0;
	int8_t i;

	//check if zero
	if (decn_is_zero(x)){
		return;
	} else if (decn_is_zero(acc)){
		copy_decn(acc, x);
		return;
	}
	//handle cases where signs differ
	if (acc->exponent < 0 && x->exponent > 0){
		// -acc, +x
		rel = compare_magn(acc, x);
		if (rel == 1){
			sub_mag(acc, x);
			negate_decn(acc);
			return;
		} else if (rel == -1){
			copy_decn(&tmp, x);
			sub_mag(&tmp, acc);
			copy_decn(acc, &tmp);
			return;
		} else { //equal
			set_dec80_zero(acc);
			return;
		}
	} else if (acc->exponent > 0 && x->exponent < 0){
		// +acc, -x
		rel = compare_magn(acc, x);
		if (rel == 1){
			sub_mag(acc, x);
			return;
		} else if (rel == -1){
			copy_decn(&tmp, x);
			sub_mag(&tmp, acc);
			negate_decn(&tmp);
			copy_decn(acc, &tmp);
			return;
		} else { //equal
			set_dec80_zero(acc);
			return;
		}
	}
	//signs must now be the same, begin adding
	copy_decn(&tmp, x);
	//normalize
	remove_leading_zeros(acc);
	remove_leading_zeros(&tmp);
#ifdef DEBUG_ADD
	extern char buf[DECN_BUF_SIZE];
	dec80_to_str(buf, acc);
	printf("rem_leading_zeros acc: %s\n", buf);
	dec80_to_str(buf, &tmp);
	printf("rem_leading_zeros tmp: %s\n", buf);
#endif
	rel = compare_magn(acc, &tmp);
	if (rel == 1){
		_incr_exp(&tmp, get_exponent(acc));
	} else if (rel == -1){
		_incr_exp(acc, get_exponent(&tmp));
	}
#ifdef DEBUG_ADD
	extern char buf[DECN_BUF_SIZE];
	dec80_to_str(buf, acc);
	printf("incr_exp acc: %s\n", buf);
	dec80_to_str(buf, &tmp);
	printf("incr_exp tmp: %s\n", buf);
#endif
	//do addition
	for (i = DEC80_NUM_LSU*2 - 1; i >= 0; i--){
		uint8_t digit = get_nibble(acc->lsu, i) + get_nibble(tmp.lsu, i) + carry;
		set_nibble(acc->lsu, i, digit % 10);
		carry = digit / 10;
		assert(carry < 10);
	}
	//may need to rescale number
	if (carry > 0){
		int16_t curr_exp = get_exponent(acc);
		rel = (acc->exponent < 0 ? 1 : 0); //is_neg?
#ifdef DEBUG_ADD
		printf("carry out: %d", carry);
#endif
		//shift right
		for (i = DEC80_NUM_LSU*2 - 1 - 1; i >= 0; i--){
			set_nibble(acc->lsu, i + 1, get_nibble(acc->lsu, i));
		}
		acc->lsu[0] &= 0xf0; //carry gets shifted into most significant digit
		acc->lsu[0] |= carry;
		curr_exp++;
		if (rel){ //is_neg
			acc->exponent = curr_exp | 0x8000;
		} else {
			acc->exponent = curr_exp | 0x7fff;
		}
	}
}

//buf should hold at least 18 + 4 + 5 + 1 = 28
static void decn_to_str(char* buf, const dec80* x, uint8_t NUM_LSU){
	uint8_t i = 0;
	uint8_t nibble;
	int16_t exponent;
	uint8_t trailing_zeros = 0;

	//check sign of number
	if (x->exponent < 0){
#ifdef DEBUG
		printf ("  negative  ");
#endif
		buf[i] = '-';
		i++;
	}
	//discard leading zeros
	for (nibble = 0; nibble < NUM_LSU*2; nibble++){
		if (get_nibble(x->lsu, nibble) != 0){
			break;
		}
	}
#ifdef DEBUG
	printf ("  leading 0s discarded (%d)  ", nibble);
#endif
	//print 1st nonzero

	//handle corner case
	if (nibble == NUM_LSU*2){
#ifdef DEBUG
		printf ("  corner case, set to 0  ");
#endif
		buf[0] = '0';
		buf[1] = '\0';
		return;
	}

	buf[i] = get_nibble(x->lsu, nibble) + '0';
	i++;
	nibble++;
	buf[i] = '.';
	i++;
	exponent = 0;
	//print rest of significand
	for ( ; nibble < NUM_LSU*2; nibble++, i++, exponent++){
		buf[i] = get_nibble(x->lsu, nibble) + '0';
		if (get_nibble(x->lsu, nibble) == 0){
			trailing_zeros++;
		} else {
			trailing_zeros = 0;
		}
	}
	//calculate exponent
#ifdef DEBUG
	printf ("  exponent (%d,", exponent);
#endif
	exponent += get_exponent(x);
#ifdef DEBUG
	printf ("%d,", exponent);
#endif
	exponent -= trailing_zeros;
#ifdef DEBUG
	printf ("%d)  ", exponent);
#endif
	//remove trailing zeros
	i -= trailing_zeros;

	if (exponent != 0){
		buf[i] = 'E';
		i++;
		if (exponent < 0){
			buf[i] = '-';
			i++;
			u32str(-exponent, &buf[i], 10); //adds null terminator
		} else {
			u32str(exponent, &buf[i], 10); //adds null terminator
		}
	} else {
		//null terminate
		buf[i] = '\0';
	}

#ifdef DEBUG
	printf ("  final i (%d)  ", i);
#endif
}

void dec80_to_str(char* buf, const dec80* x){
	decn_to_str(buf, x, DEC80_NUM_LSU);
}

void dec64_to_str(char* buf, const dec64* x){
	decn_to_str(buf, (const dec80*) x, DEC64_NUM_LSU);
}


