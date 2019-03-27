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

	//compare result of b - acc
	add_decn(&acc, &b);
	dec80_to_str(buf, &acc);
	printf("\nb - a: %s", buf);
	printf("\n     : %s", "-1.01580246791358016E4");
	dec80 diff;
	build_dec80(&diff, "-1.01580246791358016", 4);
	negate_decn(&diff);
	add_decn(&diff, &acc);
	dec80_to_str(buf, &diff);
	printf("\n     : %s\n\n", buf);

	//new acc for acc - b test
	dec80_to_str(buf, &acc);
	printf("acc: %s\n", buf);
	negate_decn(&b);
	dec80_to_str(buf, &b);
	printf("-b: %s\n", buf);

	add_decn(&acc, &b);
	dec80_to_str(buf, &acc);
	//compare result of new acc - b
	printf("\nacc - b: %s", buf);
	printf("\n       : %s", "-9.234567890123456E3");
	build_dec80(&diff, "-9.234567890123456", 3);
	negate_decn(&diff);
	add_decn(&diff, &acc);
	dec80_to_str(buf, &diff);
	printf("\n       : %s\n\n", buf);

	//new acc and b for multiply test
//	build_dec80(&acc,  "2", 2);
	build_dec80(&acc,  "92.34567890123456", 2);
	build_dec80(&b,   "-92.3456789012345678", 1);
	dec80_to_str(buf, &acc);
	printf(" acc: %s\n", buf);
	dec80_to_str(buf, &b);
	printf("   b: %s\n", buf);
	mult_decn(&acc, &b);
	dec80_to_str(buf, &acc);
	printf("acc*b: %s\n\n", buf);

	//new acc and b for divide test
	build_dec80(&acc,  "3.14", 88);
	build_dec80(&b,   "-1.5", -2);
	dec80_to_str(buf, &acc);
	printf(" acc: %s\n", buf);
	dec80_to_str(buf, &b);
	printf("   b: %s\n", buf);
	div_decn(&acc, &b);
	dec80_to_str(buf, &acc);
	printf("acc/b: %s\n", buf);


	return 0;
}
