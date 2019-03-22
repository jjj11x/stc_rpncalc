/*
 * decn_test.c
 *
 *  Created on: Mar 21, 2019
 */

#include <stdio.h>
#include "decn.h"

char buf[DECN_BUF_SIZE];

int main(void){
	dec64 a;
	dec80 acc, b;

	build_dec64(&a,  "9.234567890123456", 3);
	dec64_to_str(buf, &a);
	printf("   a: %s\n", buf);

	dec64to80(&acc, &a);
	dec80_to_str(buf, &acc);
	printf(" acc: %s\n", buf);
	negate_decn(&acc);
	dec80_to_str(buf, &acc);
	printf("-acc:  %s\n", buf);
	build_dec64(&a, "-92.3456789012345678", 1);
	dec64to80(&b, &a);
	dec80_to_str(buf, &b);
	printf("   b: %s\n", buf);

	dec80_to_str(buf, &acc);
	printf("-acc:  %s\n", buf);

	add_decn(&acc, &b);
	dec80_to_str(buf, &acc);
	printf("b - a: %s\n", buf);


	return 0;
}
