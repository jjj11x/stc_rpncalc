/*
 * decn_test.c
 *
 *  Created on: Mar 21, 2019
 */

#include <stdio.h>
#include "decn.h"

char Buf[DECN_BUF_SIZE];

int main(void){
	dec80 acc, b;

	build_dec80(&acc, "0.0009234567890123456", 7);
	dec80_to_str(Buf, &acc);
	printf(" acc: %s\n", Buf);

	build_dec80(&acc,  "9.234567890123456", 3);
	dec80_to_str(Buf, &acc);
	printf(" acc: %s\n", Buf);

	negate_decn(&acc);
	dec80_to_str(Buf, &acc);
	printf("-acc:  %s\n", Buf);

	build_dec80(&b, "-92.3456789012345678", 1);
	dec80_to_str(Buf, &b);
	printf("   b: %s\n", Buf);

	dec80_to_str(Buf, &acc);
	printf("-acc:  %s\n", Buf);

	//compare result of b - acc
	add_decn(&acc, &b);
	dec80_to_str(Buf, &acc);
	printf("\nb - a: %s", Buf);
	printf("\n     : %s", "-10158.0246791358016");
	dec80 diff;
	build_dec80(&diff, "-1.01580246791358016", 4);
	negate_decn(&diff);
	add_decn(&diff, &acc);
	dec80_to_str(Buf, &diff);
	printf("\n     : %s\n\n", Buf);

	//new acc for acc - b test
	dec80_to_str(Buf, &acc);
	printf("acc: %s\n", Buf);
	negate_decn(&b);
	dec80_to_str(Buf, &b);
	printf(" -b: %s\n", Buf);

	add_decn(&acc, &b);
	dec80_to_str(Buf, &acc);
	//compare result of new acc - b
	printf("acc - b: %s\n", Buf);
	printf("       : %s\n", "-9234.567890123456");
	build_dec80(&diff, "-9.234567890123456", 3);
	negate_decn(&diff);
	add_decn(&diff, &acc);
	dec80_to_str(Buf, &diff);
	printf("       : %s\n\n", Buf);

	//new acc and b for multiply test
//	build_dec80(&acc,  "7", 2);
	build_dec80(&acc,  "92.34567890123456", 2);
	build_dec80(&b,   "-92.3456789012345678", 1);
	dec80_to_str(Buf, &acc);
	printf(" acc: %s\n", Buf);
	dec80_to_str(Buf, &b);
	printf("   b: %s\n", Buf);
	mult_decn(&acc, &b);
	dec80_to_str(Buf, &acc);
	printf("acc*b: %s\n", Buf);
	printf("     : %s\n", "-8527724.41172991849");
	build_dec80(&diff, "-8.52772441172991849", 6);
	negate_decn(&diff);
	add_decn(&diff, &acc);
	dec80_to_str(Buf, &diff);
	printf("     : %s\n\n", Buf);

	//new acc and b for divide test
	build_dec80(&acc,  "3.14", 88);
	build_dec80(&b,   "-1.5", -2);
	dec80_to_str(Buf, &acc);
	printf(" acc: %s\n", Buf);
	dec80_to_str(Buf, &b);
	printf("   b: %s\n", Buf);
	div_decn(&acc, &b);
	dec80_to_str(Buf, &acc);
	printf("acc/b: %s\n", Buf);
	printf("     : %s\n", "-2.09333333333333334E90");
	build_dec80(&diff, "-2.09333333333333334", 90);
	negate_decn(&diff);
	add_decn(&diff, &acc);
	dec80_to_str(Buf, &diff);
	printf("     : %s\n\n", Buf);


	return 0;
}
