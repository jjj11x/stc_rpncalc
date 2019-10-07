/*
 * calc.c
 *
 *  Created on: Mar 28, 2019
 */

#include "decn/decn.h"
#include "utils.h"

#include "calc.h"

#define STACK_SIZE 4 //must be a power of 2

#define STACK_X 0
#define STACK_Y 1
#define STACK_Z 2
#define STACK_T 3

uint8_t NoLift = 0;
uint8_t IsShifted = 0;

//stack "grows" towards 0
__xdata dec80 Stack[STACK_SIZE]; //0->x, 1->y, 2->z, 3->t initially
uint8_t StackPtr = 0;

#define stack(x) Stack[(StackPtr + (x)) & (STACK_SIZE-1)]

static void pop(){
	copy_decn(&stack(STACK_X), &stack(STACK_T)); //duplicate t into x (which becomes new t)
	StackPtr++; //adjust pointer
}

void push_decn(__xdata const char* signif_str, exp_t exponent){
	if (!NoLift){
		StackPtr--;
	}
	build_dec80(signif_str, exponent);
	copy_decn(&stack(STACK_X), &AccDecn);
}

void clear_x(void){
	set_dec80_zero(&stack(STACK_X));
}

__xdata dec80* get_x(void){
	return &stack(STACK_X);
}
__xdata dec80* get_y(void){
	return &stack(STACK_Y);
}

static void do_binary_op(void (*f_ptr)(void)){
	if (decn_is_nan(&stack(STACK_Y)) || decn_is_nan(&stack(STACK_X))){
		set_dec80_NaN(&stack(STACK_Y));
	} else {
		copy_decn(&AccDecn, &stack(STACK_Y));
		copy_decn(&BDecn, &stack(STACK_X));
		f_ptr();
		copy_decn(&stack(STACK_Y), &AccDecn);
	}
	pop();
}

static void do_unary_op(void (*f_ptr)(void)){
	if (!decn_is_nan(&stack(STACK_X))){
		copy_decn(&AccDecn, &stack(STACK_X));
		f_ptr();
		copy_decn(&stack(STACK_X), &AccDecn);
	}
}

static void toggle_shifted(void){
	IsShifted ^= 1;
}

void process_cmd(char cmd){
	//turn off backlight before start of processing
	backlight_off();
	//process cmd
	switch(cmd){
		//////////
		case '+':{
			do_binary_op(add_decn);
		} break;
		//////////
		case '*':{
			do_binary_op(mult_decn);
		} break;
		//////////
		case '-':{
			negate_decn(&stack(STACK_X));
			do_binary_op(add_decn);
		} break;
		//////////
		case '/':{
			do_binary_op(div_decn);
		} break;
		//////////
		case '=':{
			if (!decn_is_nan(&stack(STACK_X))){
				StackPtr--;
				copy_decn(&stack(STACK_X), &stack(STACK_Y));
			}
		} break;
		//////////
		case 'c':{
			set_dec80_zero(&stack(STACK_X));
		} break;
		//////////
		case '<':{ //use as +/- and sqrt
			if (IsShifted){ //take sqrt
				IsShifted = 0;
				if (!decn_is_nan(&stack(STACK_X))){
					copy_decn(&AccDecn, &stack(STACK_X));
					if (AccDecn.exponent < 0){ //negative
						set_dec80_NaN(&stack(STACK_X));
						break;
					}
					set_dec80_zero(&BDecn);
					BDecn.lsu[0] = 5;
					pow_decn();
					copy_decn(&stack(STACK_X), &AccDecn);
				}
			} else { // +/-
				if (!decn_is_nan(&stack(STACK_X))){
					negate_decn(&stack(STACK_X));
				}
			}
		} break;
		//////////
		case 'r':{ //use as swap and 1/x
			if (IsShifted){ //take 1/x
				IsShifted = 0;
				do_unary_op(recip_decn);
			} else { // swap
				if (!decn_is_nan(&stack(STACK_X))){
					dec80 tmp;
					copy_decn(&tmp, &stack(STACK_X));
					copy_decn(&stack(STACK_X), &stack(STACK_Y));
					copy_decn(&stack(STACK_Y), &tmp);
				}
			}
		} break;
		//////////
		case 'm':{ //use as shift
			toggle_shifted();
		} break;
		//////////
		case '5':{
			if (IsShifted){ //e^x
				do_unary_op(exp_decn);
				IsShifted = 0;
			}
		} break;
		//////////
		case '6':{
			if (IsShifted){ //10^x
				do_unary_op(exp10_decn);
				IsShifted = 0;
			}
		} break;
		//////////
		case '9':{
			if (IsShifted){ //log10(x)
				do_unary_op(log10_decn);
				IsShifted = 0;
			}
		} break;
		//////////
		case '8':{
			if (IsShifted){ //ln(x)
				do_unary_op(ln_decn);
				IsShifted = 0;
			}
		} break;
		//////////
		case '7':{
			if (decn_is_nan(&stack(STACK_Y)) || decn_is_nan(&stack(STACK_X))){
				set_dec80_NaN(&stack(STACK_Y));
			} else {
				copy_decn(&AccDecn, &stack(STACK_Y));
				copy_decn(&BDecn, &stack(STACK_X));
				pow_decn();
				copy_decn(&stack(STACK_Y), &AccDecn);
			}
			pop();
			IsShifted = 0;
		} break;
		//////////
	} //switch(cmd)
}


