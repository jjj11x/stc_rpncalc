/*
 * key.c
 *
 *  Created on: Mar 20, 2019
 */


#include <stdint.h>
#include "utils.h"
#include "key.h"

#ifdef DESKTOP
#include <stdio.h>
const char state_names[4][32] = {
	"STEADY_LOW"    ,
	"TRANS_LOW_HIGH",
	"STEADY_HIGH"   ,
	"TRANS_HIGH_LOW"
};
#else
#include "stc15.h"
#endif


#define M_COLS 4

__idata uint8_t Keys[TOTAL_ROWS]; //only bottom nibbles get set
int8_t NewKeyPressed; //row*M_COLS + col, see KEY_MAP[] in main.c
__idata static uint8_t last_state[TOTAL_ROWS]; //all used, 2 bits per key
__idata static int8_t last_count[TOTAL_ROWS][M_COLS]; //all used
static uint8_t unexpected_count; //count of unexpected transitions

static const int8_t COUNT_LIM_LOW  = -30;
static const int8_t COUNT_LIM_HIGH =  30;
static const int8_t THRESH_STEADY = 2;
static const int8_t THRESH_TRANS  = 2;

//2-bit state:
#define STEADY_LOW     0
#define TRANS_LOW_HIGH 1
#define STEADY_HIGH    2
#define TRANS_HIGH_LOW 3

#ifdef DESKTOP
//test functions
void KeyInit(void){
	//initialize counts
	for (uint8_t i = 0; i < TOTAL_ROWS; i++){
		for (uint8_t j = 0; j < M_COLS; j++){
			last_count[i][j] = COUNT_LIM_LOW;
		}
	}
}

void raw_scan(void){
	static uint8_t i = 0;
	static const uint8_t data[] = {
		0,0,0,0,0,0,0,0,0,0,0,0,
		0,2,0,2,0,2,0,2,0,2,0,2,
		2,2,0,2,2,0,2,2,0,2,2,0,
		2,2,2,2,2,2,2,2,2,2,2,2,
		0,2,0,2,0,2,0,2,0,2,0,2,
		0,0,2,0,0,2,0,0,2,0,0,2,
		0,0,0,0,0,0,0,0,0,0,0,0
	};

	Keys[1] = data[i];
	i++;
	if (i >= (sizeof(data)/sizeof(data[0]))){
		i = 0;
	}
}
#else
void KeyInit(void){
	uint8_t i, j;

	//set column drivers bits 7:4 to quasi-bidirectional w/ pullup M[1:0]=b00
	//and row inputs bits 3:0 to quasi-bidirectional w/ pullup M[1:0]=b00
	P1M1 = 0x00;
	P1M0 = 0x00;
	//all initially pulled up
	P1 = 0xff;

	//read top row input buttons using quasi-bidirectional w/ pullup M[1:0]=b00
	//P3[1:0]
	P3M1 &= ~(0x3);
	P3M0 &= ~(0x3);
	P3 |= 0x3; //pull up
	//P5[5:4]
	P5M1 &= ~(0x30);
	P5M0 &= ~(0x30);
	P5 |= 0x30; //pull up

	//initialize counts
	for (i = 0; i < TOTAL_ROWS; i++){
		for (j = 0; j < M_COLS; j++){
			last_count[i][j] = COUNT_LIM_LOW;
		}
	}

	//no new key
	NewKeyPressed = -1;
}

#pragma nooverlay
void raw_scan(void) __using(1){
	//top row not part of matrix
	static const uint8_t M_ROWS = 4;
	uint8_t i, j;

	//scan top row
	Keys[0] = 0; //initially
	//temporarily set P3[1:0] push pull output M[1:0]=b01
	P3M0 |= 0x3;
	P3 &= ~(0x3);
	P3 |=   0x3;
	//back to quasi-bidirectional w/ pullup M[1:0]=b00
	P3M0 &= ~(0x3);
	//read inputs
	Keys[0] += (P3_0 ? 0 : (1<<0)); //on
	Keys[0] += (P3_1 ? 0 : (1<<1));
	Keys[0] += (P5_4 ? 0 : (1<<2));
	Keys[0] += (P5_5 ? 0 : (1<<3));
	//scan matrix
	for (i = 0; i < M_ROWS; i++){
		Keys[i+1] = 0; //initially
		for (j = 0; j < M_COLS; j++){
			uint8_t read;
			P1 = (~(1 << (j+4))); //pulldown column (all others pull up)
			read = (P1 & (1<<i)); //read button
			read = (read == 0 ? 1 : 0); //pressed if grounded by column driver
			Keys[i+1] += (read << j);
			P1 = 0xff; //reset all as pullups
		}
	}
}
#endif

//based on quick draw/integrator hybrid debounce algorithm from
//https://summivox.wordpress.com/2016/06/03/keyboard-matrix-scanning-and-debouncing/
#pragma nooverlay
void debounce(void) __using(1){
	uint8_t i, j;
	NewKeyPressed = -1; //initially
//	new_keys_pressed = 0; //initially
	for (i = 0; i < TOTAL_ROWS; i++){
		for (j = 0; j < M_COLS; j++){
			int8_t new_count;
			uint8_t new_state;
			int8_t curr_count = last_count[i][j];
			uint8_t curr_state = last_state[i] & (0x3 << (j*2));
			curr_state >>= j*2;
			new_count = curr_count;
			new_state = curr_state;
			//update count
			if (Keys[i] & (1 << j)){
				if (curr_count < COUNT_LIM_HIGH){
					new_count = curr_count + 1;
				}
			} else {
				if (curr_count > COUNT_LIM_LOW){
					new_count = curr_count - 1;
				}
			}
			//update state
			switch(curr_state){
				////////
				case STEADY_LOW: {
					if (new_count >= COUNT_LIM_LOW + THRESH_TRANS){
						new_count = 0;
						new_state = TRANS_LOW_HIGH;
					}
					break;
				}
				////////
				case TRANS_LOW_HIGH: {
					if (new_count >= THRESH_STEADY){
						new_state = STEADY_HIGH;
						new_count = COUNT_LIM_HIGH;
					} else if (new_count <= -THRESH_STEADY){
						new_state = STEADY_LOW;
						new_count = COUNT_LIM_LOW;
						unexpected_count++;
					}
					break;
				}
				////////
				case STEADY_HIGH: {
					if (new_count <= COUNT_LIM_HIGH - THRESH_TRANS){
						new_count = 0;
						new_state = TRANS_HIGH_LOW;
					}
					break;
				}
				////////
				case TRANS_HIGH_LOW: {
					if (new_count <= -THRESH_STEADY){
						new_state = STEADY_LOW;
						new_count = COUNT_LIM_LOW;
					} else if (new_count >= THRESH_STEADY){
						new_state = STEADY_HIGH;
						new_count = COUNT_LIM_HIGH;
						unexpected_count++;
					}
					break;
				}
				////////
				default:
					new_state = STEADY_LOW;
					new_count = COUNT_LIM_LOW;
					unexpected_count++;
			} //switch(curr_state)
			//track new key down
			if (curr_state != TRANS_LOW_HIGH && new_state == TRANS_LOW_HIGH){
				NewKeyPressed = (i*M_COLS + j); //can only track 1 for now
#ifdef DESKTOP
				printf("new key (%d, %d) pressed\n", i, j);
//				printf("curr_state: %d, new_state: %d\n", curr_state, new_state);
#endif
//				new_keys_pressed |= ((uint32_t) 1 << (i*M_COLS + j));
			}
			//write back new count
			last_count[i][j] = new_count;
			//write back new state
			last_state[i] = (last_state[i] & ~(0x3 << (j*2))) | (new_state << (j*2));
		} //for cols
	} //for rows
}


#ifdef DEBUG_KEYS
const uint8_t* DebugGetKeys(void){
	return keys;
}
#endif




#ifdef KEY_TEST_APP

int main(void){
	KeyInit();

	for (int iii = 0; iii < 100; iii++){
		//look at '9' key at row 1, col 1 (0 indexed)
		//the corresponding new key code is 5, see KEY_MAP[] in main.c
		static const uint8_t i = 1, j = 1;
		int8_t curr_count = last_count[i][j/2] & (0xf << ((j&1)*4));
		uint8_t curr_state = last_state[i] & (0x3 << (j*2));
		curr_count >>= (j&1)*4;
		curr_state >>= j*2;
		if (iii == 0){
			printf("%3d: %x, %14s, %2d, %2d\n",
				   iii, Keys[1], state_names[curr_state], curr_count, NewKeyPressed);
		}
		KeyScan();
		printf("%3d: %x, %14s, %2d, %2d\n",
		       iii, Keys[1], state_names[curr_state], curr_count, NewKeyPressed);
	}

	return 0;
}
#endif
