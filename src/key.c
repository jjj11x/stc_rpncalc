/*
 * key.c
 *
 *  Created on: Mar 20, 2019
 */

#include <stdint.h>
#include "stc15.h"


static uint8_t keys[5];

void KeyInit(void){
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
}


void KeyScan(void){
	//top row not part of matrix
	static const uint8_t M_ROWS = 4;
	static const uint8_t M_COLS = 4;
	uint8_t i, j;

	//scan top row
	keys[0] = 0; //initially
	//temporarily set P3[1:0] push pull output M[1:0]=b01
	P3M0 |= 0x3;
	P3 &= ~(0x3);
	P3 |=   0x3;
	//back to quasi-bidirectional w/ pullup M[1:0]=b00
	P3M0 &= ~(0x3);
	//read inputs
	keys[0] += (P3_0 ? 0 : (1<<0)); //on
	keys[0] += (P3_1 ? 0 : (1<<1));
	keys[0] += (P5_4 ? 0 : (1<<2));
	keys[0] += (P5_5 ? 0 : (1<<3));
	//scan matrix
	for (i = 0; i < M_ROWS; i++){
		keys[i+1] = 0; //initially
		for (j = 0; j < M_COLS; j++){
			uint8_t read;
			P1 = (~(1 << (j+4))); //pulldown column (all others pull up)
			read = (P1 & (1<<i)); //read button
			read = (read == 0 ? 1 : 0); //pressed if grounded by column driver
			keys[i+1] += (read << j);
			P1 = 0xff; //reset all as pullups
		}
	}
}

uint8_t* GetKeys(void){
	return keys;
}

