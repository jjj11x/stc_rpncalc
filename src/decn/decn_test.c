/*
 * decn_test.c
 *
 *  Created on: Mar 21, 2019
 */

#include <stdio.h>
#include "decn.h"

char buf[DECN_BUF_SIZE];

int main(void){
	dec80 acc, b;

	build_dec80(&acc, "0.0009234567890123456", 7);
	dec80_to_str(buf, &acc);
	printf(" acc: %s\n", buf);

	build_dec80(&acc,  "9.234567890123456", 3);
	dec80_to_str(buf, &acc);
	printf(" acc: %s\n", buf);

	negate_decn(&acc);
	dec80_to_str(buf, &acc);
	printf("-acc:  %s\n", buf);

	build_dec80(&b, "-92.3456789012345678", 1);
	dec80_to_str(buf, &b);
	printf("   b: %s\n", buf);

	dec80_to_str(buf, &acc);
	printf("-acc:  %s\n", buf);

	add_decn(&acc, &b);
	dec80_to_str(buf, &acc);
	printf("b - a: %s\n", buf);

	printf("acc: %s\n", buf);
	negate_decn(&b);
	dec80_to_str(buf, &b);
	printf("-b: %s\n", buf);

	add_decn(&acc, &b);
	dec80_to_str(buf, &acc);
	printf("acc - b: %s\n", buf);


	return 0;
}
