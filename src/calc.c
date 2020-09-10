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
 * calc.c
 *
 *  Created on: Mar 28, 2019
 */

#include "decn/decn.h"
#include "utils.h"

#include "calc.h"
#include "stack_debug.h"

__xdata dec80 StoredDecn;
__xdata dec80 LastX;

#define STACK_SIZE 4 //must be a power of 2

#define STACK_X 0
#define STACK_Y 1
#define STACK_Z 2
#define STACK_T 3

uint8_t NoLift = 0;
__bit IsShiftedUp = 0;
__bit IsShiftedDown = 0;

//stack "grows" towards 0
__xdata dec80 Stack[STACK_SIZE]; //0->x, 1->y, 2->z, 3->t initially
uint8_t StackPtr = 0;

#define stack(x) Stack[(StackPtr + (x)) & (STACK_SIZE-1)]

static void pop(){
	copy_decn(&stack(STACK_X), &stack(STACK_T)); //duplicate t into x (which becomes new t)
	StackPtr++; //adjust pointer
}

void push_decn(__xdata const char* signif_str, __xdata exp_t exponent){
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
		copy_decn(&LastX, &stack(STACK_X)); //save LastX
		copy_decn(&AccDecn, &stack(STACK_Y));
		copy_decn(&BDecn, &stack(STACK_X));
		f_ptr();
		copy_decn(&stack(STACK_Y), &AccDecn);
	}
	pop();
}

static void do_unary_op(void (*f_ptr)(void)){
	if (!decn_is_nan(&stack(STACK_X))){
		copy_decn(&LastX, &stack(STACK_X)); //save LastX
		copy_decn(&AccDecn, &stack(STACK_X));
		f_ptr();
		copy_decn(&stack(STACK_X), &AccDecn);
	}
}

void process_cmd(char cmd){
	//turn off backlight before start of processing
	backlight_off();
	//process cmd
	switch(cmd){
		//////////
		case '+':{
			if (IsShiftedUp){ // LastX
				if (NoLift != 1){
					StackPtr--;
				}
				copy_decn(&stack(STACK_X), &LastX);
				IsShiftedUp = 0;
			} else { // +
				do_binary_op(add_decn);
			}
		} break;
		//////////
		case '*':{
			do_binary_op(mult_decn);
		} break;
		//////////
		case '-':{
			if (IsShiftedUp) {
				do_unary_op(to_radian_decn);
				IsShiftedUp = 0;
			} else if (IsShiftedDown) {
				do_unary_op(to_degree_decn);
				IsShiftedDown = 0;
			} else {
				negate_decn(&stack(STACK_X));
				do_binary_op(add_decn);
				negate_decn(&LastX); //stored LastX was after negation of X
			}
		} break;
		//////////
		case '/':{
			if (IsShiftedUp){
				StackPtr--;
				pi_decn();
				copy_decn(&stack(STACK_X), &AccDecn);
			} else if (IsShiftedDown) {
			} else {
				do_binary_op(div_decn);
			}
		} break;
		//////////
		case '=':{
			if (IsShiftedUp){ //RCL
				if (NoLift != 1){
					StackPtr--;
				}
				copy_decn(&stack(STACK_X), &StoredDecn);
			} else { //Enter
				if (!decn_is_nan(&stack(STACK_X))){
					StackPtr--;
					copy_decn(&stack(STACK_X), &stack(STACK_Y));
				}
			}
		} break;
		//////////
		case '.':{
			if (IsShiftedUp){ //STO
				copy_decn(&StoredDecn, &stack(STACK_X));
			}
		} break;
		//////////
		case 'c':{
			set_dec80_zero(&stack(STACK_X));
		} break;
		//////////
		case '<':{ //use as +/- and sqrt
			if (IsShiftedUp){ //take sqrt
				IsShiftedUp = 0;
				if (decn_is_zero(&stack(STACK_X))){
					//sqrt(0) = 0
				} else if (!decn_is_nan(&stack(STACK_X))){
					copy_decn(&LastX, &stack(STACK_X)); //save LastX
					copy_decn(&AccDecn, &stack(STACK_X));
					if (AccDecn.exponent < 0){ //negative
						set_dec80_NaN(&stack(STACK_X));
						break;
					}
					//b = 0.5
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
			if (IsShiftedUp){ //take 1/x
				do_unary_op(recip_decn);
			} else { // swap
				if (!decn_is_nan(&stack(STACK_X))){
					copy_decn(&AccDecn, &stack(STACK_X));
					copy_decn(&stack(STACK_X), &stack(STACK_Y));
					copy_decn(&stack(STACK_Y), &AccDecn);
				}
			}
		} break;
		//////////
		case 'm':{ //use as shift
			if (IsShiftedUp) {
				IsShiftedUp = 0;
				IsShiftedDown = 1;
			} else if (IsShiftedDown) {
				IsShiftedUp = 0;
				IsShiftedDown = 0;
			} else {
				IsShiftedUp = 1;
				IsShiftedDown = 0;
			}
			return;
		} break;
		//////////
		case '1':{
			if (IsShiftedUp){
				do_unary_op(sin_decn);
			} else if (IsShiftedDown){
				// do_unary_op(arcsin_decn);
			}
		} break;
		//////////
		case '2':{
			if (IsShiftedUp){
				do_unary_op(cos_decn);
			} else if (IsShiftedDown){
				// do_unary_op(arccos_decn);
			}
		} break;
		//////////
		case '3':{
			if (IsShiftedUp){
				do_unary_op(tan_decn);
			} else if (IsShiftedDown){
				do_unary_op(arctan_decn);
			}
		} break;
		//////////
		case '4':{
			if (IsShiftedUp){ //roll down
				StackPtr++;
			} else if (IsShiftedDown){ //roll up
				StackPtr--;
			}
		} break;
		//////////
		case '5':{
			if (IsShiftedUp){ //e^x
				do_unary_op(exp_decn);
			}
		} break;
		//////////
		case '6':{
			if (IsShiftedUp){ //10^x
				do_unary_op(exp10_decn);
			}
		} break;
		//////////
		case '9':{
			if (IsShiftedUp){ //log10(x)
				do_unary_op(log10_decn);
			}
		} break;
		//////////
		case '8':{
			if (IsShiftedUp){ //ln(x)
				do_unary_op(ln_decn);
			}
		} break;
		//////////
		case '7':{ //y^x
			if (decn_is_nan(&stack(STACK_Y)) || decn_is_nan(&stack(STACK_X))){
				set_dec80_NaN(&stack(STACK_Y));
			} else {
				copy_decn(&LastX, &stack(STACK_X)); //save LastX
				copy_decn(&AccDecn, &stack(STACK_Y));
				copy_decn(&BDecn, &stack(STACK_X));
				pow_decn();
				copy_decn(&stack(STACK_Y), &AccDecn);
			}
			pop();
		} break;
		//////////
	} //switch(cmd)
	IsShiftedUp = 0;
	IsShiftedDown = 0;
}


