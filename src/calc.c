/*
 * calc.c
 *
 *  Created on: Mar 28, 2019
 */

#include "decn/decn.h"
#include "utils.h"

#define STACK_SIZE 4 //must be a power of 2

#define STACK_X 0
#define STACK_Y 1
#define STACK_Z 2
#define STACK_T 3

//stack "grows" towards 0
__xdata dec80 Stack[STACK_SIZE]; //0->x, 1->y, 2->z, 3->t initially
uint8_t StackPtr = 0;

#define stack(x) Stack[(StackPtr + (x)) & (STACK_SIZE-1)]

static void pop(){
	copy_decn(&stack(STACK_X), &stack(STACK_T)); //duplicate t into x (which becomes new t)
	StackPtr++; //adjust pointer
}

void push_decn(const char* signif_str, int16_t exponent, uint8_t no_lift){
	if (!no_lift){
		StackPtr--;
	}
	build_dec80(&stack(STACK_X), signif_str, exponent);
}

void clear_x(void){
	set_dec80_zero(&stack(STACK_X));
}

void set_x(const char* signif_str, int16_t exponent){
	build_dec80(&stack(STACK_X), signif_str, exponent);
}

__xdata dec80* get_x(void){
	return &stack(STACK_X);
}
__xdata dec80* get_y(void){
	return &stack(STACK_Y);
}

static void do_binary_op(void (*f_ptr)(dec80*, const dec80*)){
	f_ptr(&stack(STACK_Y), &stack(STACK_X));
	pop();
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
			StackPtr--;
			copy_decn(&stack(STACK_X), &stack(STACK_Y));
		} break;
		//////////
		case 'c':{
			set_dec80_zero(&stack(STACK_X));
		} break;
		//////////
		case '<':{ //use as +/-
			negate_decn(&stack(STACK_X));
		} break;
		//////////
		case 'r':{ //use as swap
			dec80 tmp;
			copy_decn(&tmp, &stack(STACK_X));
			copy_decn(&stack(STACK_X), &stack(STACK_Y));
			copy_decn(&stack(STACK_Y), &tmp);
		} break;
		//////////
	} //switch(cmd)
}


