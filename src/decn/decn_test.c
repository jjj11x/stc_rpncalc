/*
 * decn_test.c
 *
 *  Created on: Mar 21, 2019
 */

#include <stdio.h>
#include "decn.h"

char Buf[DECN_BUF_SIZE];


static dec80 diff;

static void take_diff(void){ //diff = acc - diff
	negate_decn(&diff);
	dec80 tmp_copy, tmp_copy2;
	copy_decn(&tmp_copy, &AccDecn); //save
	copy_decn(&tmp_copy2, &BDecn); //save
	copy_decn(&BDecn, &diff);
	add_decn();
	copy_decn(&diff, &AccDecn);
	copy_decn(&AccDecn, &tmp_copy); //restore
	copy_decn(&BDecn, &tmp_copy2); //restore
}

static void div_test(
	const char* a_str, int a_exp,
	const char* b_str, int b_exp,
	const char* res_str,
	const char* res_calc, int res_exp)
{
	build_dec80(a_str, a_exp);
	build_decn_at(&BDecn,   b_str, b_exp);
	decn_to_str_complete(&AccDecn);
	printf(" acc: %s\n", Buf);
	decn_to_str_complete(&BDecn);
	printf("   b: %s\n", Buf);
	div_decn();
	decn_to_str_complete(&AccDecn);
	printf("acc/b: %s\n", Buf);
	printf("     : %s\n", res_str);
	build_decn_at(&diff, res_calc, res_exp);
	take_diff();
	decn_to_str_complete(&diff);
	printf("     : %s\n\n", Buf);
}

static void log_test(
	const char* x_str, int x_exp,
	const char* res_str, int res_exp)
{
	build_dec80(x_str, x_exp);
	decn_to_str_complete(&AccDecn);
	printf("  a  : %s\n", Buf);
	ln_decn();
	decn_to_str_complete(&AccDecn);
	printf("ln(a): %s\n", Buf);
	build_decn_at(&diff, res_str, res_exp);
	take_diff();
	decn_to_str_complete(&diff);
	printf("     : %s\n\n", Buf);
}

static void log10_test(
	const char* x_str, int x_exp,
	const char* res_str, int res_exp)
{
	build_dec80(x_str, x_exp);
	decn_to_str_complete(&AccDecn);
	printf("  a  : %s\n", Buf);
	log10_decn();
	decn_to_str_complete(&AccDecn);
	printf("ln(a): %s\n", Buf);
	build_decn_at(&diff, res_str, res_exp);
	take_diff();
	decn_to_str_complete(&diff);
	printf("     : %s\n\n", Buf);
}

int main(void){
//	dec80 acc, b;

	build_dec80("0.0009234567890123456", 7);
	decn_to_str_complete(&AccDecn);
	printf(" acc: %s\n", Buf);

	build_dec80("9.234567890123456", 3);
	decn_to_str_complete(&AccDecn);
	printf(" acc: %s\n", Buf);

	negate_decn(&AccDecn);
	decn_to_str_complete(&AccDecn);
	printf("-acc:  %s\n", Buf);

	dec80 tmp_copy;
	copy_decn(&tmp_copy, &AccDecn); //save
	build_dec80("-92.3456789012345678", 1);
	copy_decn(&BDecn, &AccDecn);
	copy_decn(&AccDecn, &tmp_copy); //restore
	copy_decn(&tmp_copy, &BDecn); //save
	decn_to_str_complete(&BDecn);
	printf("   b: %s\n", Buf);

	decn_to_str_complete(&AccDecn);
	printf("-acc:  %s\n", Buf);

	//compare result of b - acc
	add_decn();
	decn_to_str_complete(&AccDecn);
	printf("b - a: %s\n", Buf);
	printf("     : %s\n", "-10158.0246791358016");
	build_decn_at(&diff, "-1.01580246791358016", 4);
	take_diff();
	decn_to_str_complete(&diff);
	printf("     : %s\n\n", Buf);

	//new acc for acc - b test
	decn_to_str_complete(&AccDecn);
	printf("acc: %s\n", Buf);
	copy_decn(&BDecn, &tmp_copy); //restore
	negate_decn(&BDecn);
	decn_to_str_complete(&BDecn);
	printf(" -b: %s\n", Buf);

	add_decn();
	decn_to_str_complete(&AccDecn);
	//compare result of new acc - b
	printf("acc - b: %s\n", Buf);
	printf("       : %s\n", "-9234.567890123456");
	build_decn_at(&diff, "-9.234567890123456", 3);
	take_diff();
	decn_to_str_complete(&diff);
	printf("       : %s\n\n", Buf);

	//new acc and b for multiply test
//	build_dec80("7", 2);
	build_dec80("92.34567890123456", 2);
	build_decn_at(&BDecn,   "-92.3456789012345678", 1);
	decn_to_str_complete(&AccDecn);
	printf(" acc: %s\n", Buf);
	decn_to_str_complete(&BDecn);
	printf("   b: %s\n", Buf);
	mult_decn();
	decn_to_str_complete(&AccDecn);
	printf("acc*b: %s\n", Buf);
	printf("     : %s\n", "-8527724.41172991849");
	build_decn_at(&diff, "-8.52772441172991849", 6);
	take_diff();
	decn_to_str_complete(&diff);
	printf("     : %s\n\n", Buf);

	//new acc and b for divide test
	div_test(
			"3.14", 60,
			"-1.5", -2,
			"-2.09333333333333334E62",
			"-2.09333333333333334", 62
	);

	//new acc and b for divide test
	div_test(
			"4", 0,
			"4", 0,
			"1.",
			"1", 0
	);

	//new acc and b for divide test
	div_test(
			"1", 0,
			"3", 0,
			"0.333333333333333336",
			"3.33333333333333336", -1
	);

	//small fractions >= 1/10
	build_dec80("0.333", 0);
	build_decn_at(&BDecn,   "3.33", -1);
	decn_to_str_complete(&AccDecn);
	printf("  a  : %s\n", Buf);
	decn_to_str_complete(&BDecn);
	printf("  b  : %s\n", Buf);
	negate_decn(&BDecn);
	add_decn();
	decn_to_str_complete(&AccDecn);
	printf("a - b: %s\n", Buf);

	//new acc and b for divide test
	div_test(
			"500", 0,
			"99", 0,
			"5.05050505050505055",
			"5.05050505050505055", 0
	);

	//new acc and b for divide test
	div_test(
			"500", 0,
			"2", 0,
			"250.",
			"250", 0
	);

	//new acc and b for divide test
	div_test(
			"3", 0,
			"25", -15,
			"120000000000000",
			"1.2", 14
	);

	//new acc and b for divide test
	div_test(
			"0.02", 0,
			"0.03", 0,
			"0.666666666666666672",
			"0.666666666666666672", 0
	);

	//new acc for log test
	log_test(
		"0.155", 0,
		"-1.86433016206289043", 0
	);

	//new acc for log test
	log_test(
		"10", 0,
		"2.30258509299404568", 0
	);

	//new acc for log test
	log_test(
		"1.1", 10,
		"23.1211611097447817", 0
	);

	//new acc for log test
	log_test(
		"2.02", -10,
		"-22.3227534185273434", 0
	);

	//new acc for log test
	log10_test(
		"1.5", 0,
		"0.176091259", 0
	);
	return 0;
}
