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
	decn_to_str_complete(Buf, &acc);
	printf(" acc: %s\n", Buf);

	build_dec80(&acc,  "9.234567890123456", 3);
	decn_to_str_complete(Buf, &acc);
	printf(" acc: %s\n", Buf);

	negate_decn(&acc);
	decn_to_str_complete(Buf, &acc);
	printf("-acc:  %s\n", Buf);

	build_dec80(&b, "-92.3456789012345678", 1);
	decn_to_str_complete(Buf, &b);
	printf("   b: %s\n", Buf);

	decn_to_str_complete(Buf, &acc);
	printf("-acc:  %s\n", Buf);

	//compare result of b - acc
	add_decn(&acc, &b);
	decn_to_str_complete(Buf, &acc);
	printf("b - a: %s\n", Buf);
	printf("     : %s\n", "-10158.0246791358016");
	dec80 diff;
	build_dec80(&diff, "-1.01580246791358016", 4);
	negate_decn(&diff);
	add_decn(&diff, &acc);
	decn_to_str_complete(Buf, &diff);
	printf("     : %s\n\n", Buf);

	//new acc for acc - b test
	decn_to_str_complete(Buf, &acc);
	printf("acc: %s\n", Buf);
	negate_decn(&b);
	decn_to_str_complete(Buf, &b);
	printf(" -b: %s\n", Buf);

	add_decn(&acc, &b);
	decn_to_str_complete(Buf, &acc);
	//compare result of new acc - b
	printf("acc - b: %s\n", Buf);
	printf("       : %s\n", "-9234.567890123456");
	build_dec80(&diff, "-9.234567890123456", 3);
	negate_decn(&diff);
	add_decn(&diff, &acc);
	decn_to_str_complete(Buf, &diff);
	printf("       : %s\n\n", Buf);

	//new acc and b for multiply test
//	build_dec80(&acc,  "7", 2);
	build_dec80(&acc,  "92.34567890123456", 2);
	build_dec80(&b,   "-92.3456789012345678", 1);
	decn_to_str_complete(Buf, &acc);
	printf(" acc: %s\n", Buf);
	decn_to_str_complete(Buf, &b);
	printf("   b: %s\n", Buf);
	mult_decn(&acc, &b);
	decn_to_str_complete(Buf, &acc);
	printf("acc*b: %s\n", Buf);
	printf("     : %s\n", "-8527724.41172991849");
	build_dec80(&diff, "-8.52772441172991849", 6);
	negate_decn(&diff);
	add_decn(&diff, &acc);
	decn_to_str_complete(Buf, &diff);
	printf("     : %s\n\n", Buf);

	//new acc and b for divide test
	build_dec80(&acc,  "3.14", 60);
	build_dec80(&b,   "-1.5", -2);
	decn_to_str_complete(Buf, &acc);
	printf(" acc: %s\n", Buf);
	decn_to_str_complete(Buf, &b);
	printf("   b: %s\n", Buf);
	div_decn(&acc, &b);
	decn_to_str_complete(Buf, &acc);
	printf("acc/b: %s\n", Buf);
	printf("     : %s\n", "-2.09333333333333334E62");
	build_dec80(&diff, "-2.09333333333333334", 62);
	negate_decn(&diff);
	add_decn(&diff, &acc);
	decn_to_str_complete(Buf, &diff);
	printf("     : %s\n\n", Buf);

	//new acc and b for divide test
	build_dec80(&acc,  "4", 0);
	build_dec80(&b,   "4", 0);
	decn_to_str_complete(Buf, &acc);
	printf(" acc: %s\n", Buf);
	decn_to_str_complete(Buf, &b);
	printf("   b: %s\n", Buf);
	div_decn(&acc, &b);
	decn_to_str_complete(Buf, &acc);
	printf("acc/b: %s\n", Buf);
	printf("     : %s\n", "1.");
	build_dec80(&diff, "1", 0);
	negate_decn(&diff);
	add_decn(&diff, &acc);
	decn_to_str_complete(Buf, &diff);
	printf("     : %s\n\n", Buf);

	//new acc and b for divide test
	build_dec80(&acc,  "1", 0);
	build_dec80(&b,   "3", 0);
	decn_to_str_complete(Buf, &acc);
	printf(" acc: %s\n", Buf);
	decn_to_str_complete(Buf, &b);
	printf("   b: %s\n", Buf);
	div_decn(&acc, &b);
	decn_to_str_complete(Buf, &acc);
	printf("acc/b: %s\n", Buf);
	printf("     : %s\n", "0.333333333333333336");
	build_dec80(&diff, "3.33333333333333336", -1);
	decn_to_str_complete(Buf, &diff);
	printf("     : %s\n", Buf);
	negate_decn(&diff);
	add_decn(&diff, &acc);
	decn_to_str_complete(Buf, &diff);
	printf("     : %s\n\n", Buf);

	//small fractions >= 1/10
	build_dec80(&acc, "0.333", 0);
	build_dec80(&b,   "3.33", -1);
	decn_to_str_complete(Buf, &acc);
	printf("  a  : %s\n", Buf);
	decn_to_str_complete(Buf, &b);
	printf("  b  : %s\n", Buf);
	negate_decn(&b);
	add_decn(&acc, &b);
	decn_to_str_complete(Buf, &acc);
	printf("a - b: %s\n", Buf);

	//new acc and b for divide test
	build_dec80(&acc,  "500", 0);
	build_dec80(&b,   "99", 0);
	decn_to_str_complete(Buf, &acc);
	printf(" acc: %s\n", Buf);
	decn_to_str_complete(Buf, &b);
	printf("   b: %s\n", Buf);
	div_decn(&acc, &b);
	decn_to_str_complete(Buf, &acc);
	printf("acc/b: %s\n", Buf);
	printf("     : %s\n", "5.05050505050505055");
	build_dec80(&diff, "5.05050505050505055", 0);
	negate_decn(&diff);
	add_decn(&diff, &acc);
	decn_to_str_complete(Buf, &diff);
	printf("     : %s\n\n", Buf);

	//new acc and b for divide test
	build_dec80(&acc,  "500", 0);
	build_dec80(&b,   "2", 0);
	decn_to_str_complete(Buf, &acc);
	printf(" acc: %s\n", Buf);
	decn_to_str_complete(Buf, &b);
	printf("   b: %s\n", Buf);
	div_decn(&acc, &b);
	decn_to_str_complete(Buf, &acc);
	printf("acc/b: %s\n", Buf);
	printf("     : %s\n", "250.");
	build_dec80(&diff, "250", 0);
	negate_decn(&diff);
	add_decn(&diff, &acc);
	decn_to_str_complete(Buf, &diff);
	printf("     : %s\n\n", Buf);

	//new acc and b for divide test
	build_dec80(&acc,  "3", 0);
	build_dec80(&b,   "25", -15);
	decn_to_str_complete(Buf, &acc);
	printf(" acc: %s\n", Buf);
	decn_to_str_complete(Buf, &b);
	printf("   b: %s\n", Buf);
	div_decn(&acc, &b);
	decn_to_str_complete(Buf, &acc);
	printf("acc/b: %s\n", Buf);
	printf("     : %s\n", "120000000000000");
	build_dec80(&diff, "1.2", 14);
	negate_decn(&diff);
	add_decn(&diff, &acc);
	decn_to_str_complete(Buf, &diff);
	printf("     : %s\n\n", Buf);

	//new acc and b for divide test
	build_dec80(&acc,  "0.02", 0);
	build_dec80(&b,   "0.03", 0);
	decn_to_str_complete(Buf, &acc);
	printf(" acc: %s\n", Buf);
	decn_to_str_complete(Buf, &b);
	printf("   b: %s\n", Buf);
	div_decn(&acc, &b);
	decn_to_str_complete(Buf, &acc);
	printf("acc/b: %s\n", Buf);
	printf("     : %s\n", "0.666666666666666672");
	build_dec80(&diff, "0.666666666666666672", 0);
	negate_decn(&diff);
	add_decn(&diff, &acc);
	decn_to_str_complete(Buf, &diff);
	printf("     : %s\n\n", Buf);


	return 0;
}
