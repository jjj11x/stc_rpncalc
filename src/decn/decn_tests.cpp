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
 * decn_tests.cpp
 *
 * Unit tests using https://github.com/catchorg/Catch2
 *
 *  Created on: Nov 14, 2019
 */


#include <string.h>
#include <boost/multiprecision/mpfr.hpp>
#include <catch.hpp>
#include "decn.h"


namespace bmp = boost::multiprecision;
using Catch::Matchers::Equals;



char Buf[DECN_BUF_SIZE];


TEST_CASE("build decn"){
	build_dec80("0.0009234567890123456", 7);
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("9234.567890123456"));

	build_dec80("9.234567890123456", 3);
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("9234.567890123456"));

	negate_decn(&AccDecn);
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("-9234.567890123456"));
}

TEST_CASE("small fractions >= 1/10"){
	build_dec80("0.333", 0);
	build_decn_at(&BDecn,   "3.33", -1);
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("0.333"));
	decn_to_str_complete(&BDecn);
	CHECK_THAT(Buf, Equals("0.333"));
	negate_decn(&BDecn);
	add_decn();
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("0"));
}

TEST_CASE("add"){
	dec80 tmp_copy;
	build_dec80("-9.234567890123456", 3);
	copy_decn(&tmp_copy, &AccDecn); //save
	build_dec80("-92.3456789012345678", 1);
	copy_decn(&BDecn, &AccDecn);
	copy_decn(&AccDecn, &tmp_copy); //restore
	copy_decn(&tmp_copy, &BDecn); //save
	decn_to_str_complete(&BDecn);
	CHECK_THAT(Buf, Equals("-923.456789012345678")); //b

	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("-9234.567890123456")); //-acc

	//compare result of b - acc
	add_decn();
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("-10158.0246791358016")); //b-acc

	//new acc for acc - b test
	decn_to_str_complete(&AccDecn);
	copy_decn(&BDecn, &tmp_copy); //restore
	negate_decn(&BDecn);
	decn_to_str_complete(&BDecn);
	CHECK_THAT(Buf, Equals("923.456789012345678")); //-b

	add_decn();
	decn_to_str_complete(&AccDecn);
	//compare result of new acc - b
	CHECK_THAT(Buf, Equals("-9234.567890123456")); //acc-b
}

TEST_CASE("multiply"){
	build_dec80("92.34567890123456", 2);
	build_decn_at(&BDecn,   "-92.3456789012345678", 1);
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("9234.567890123456")); //acc
	decn_to_str_complete(&BDecn);
	CHECK_THAT(Buf, Equals("-923.456789012345678")); //b
	mult_decn();
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("-8527724.41172991849")); //acc*b
}

static void div_test(
	//input
	const char* a_str, int a_exp,
	const char* b_str, int b_exp
)
{
	CAPTURE(a_str); CAPTURE(a_exp);
	CAPTURE(b_str); CAPTURE(b_exp);
	//do division
	build_dec80(a_str, a_exp);
	build_decn_at(&BDecn,   b_str, b_exp);
// 	decn_to_str_complete(&AccDecn);
// 	printf(" acc: %s\n", Buf);
// 	decn_to_str_complete(&BDecn);
// 	printf("   b: %s\n", Buf);
	div_decn();
	decn_to_str_complete(&AccDecn);
	CAPTURE(Buf);  // acc / b

	//calculate actual result
	bmp::mpfr_float::default_precision(50);
	bmp::mpfr_float calculated(Buf);
	std::string a_full_str(a_str);
	a_full_str += "e" + std::to_string(a_exp);
	std::string b_full_str(b_str);
	b_full_str += "e" + std::to_string(b_exp);;
// 	CAPTURE(a_full_str);
// 	CAPTURE(b_full_str);
	bmp::mpfr_float a_actual(a_full_str);
	bmp::mpfr_float b_actual(b_full_str);
	a_actual /= b_actual;
	bmp::mpfr_float rel_diff = abs((a_actual - calculated) / a_actual);
	CHECK(rel_diff < 1e-17);
}

TEST_CASE("division"){
	div_test(
		"3.14", 60,
		"-1.5", -2
	);

	div_test(
		"4", 0,
		"4", 0
	);

	div_test(
		"1", 0,
		"3", 0
	);

	div_test(
		"500", 0,
		"99", 0
	);

	div_test(
		"500", 0,
		"2", 0
	);

	div_test(
		"3", 0,
		"25", -15
	);

	div_test(
		"0.02", 0,
		"0.03", 0
	);
}

static void log_test(
	//input
	const char* x_str, int x_exp,
	bool base10=false
)
{
	CAPTURE(x_str); CAPTURE(x_exp);
	CAPTURE(base10);
	build_dec80(x_str, x_exp);
	// 	decn_to_str_complete(&AccDecn);
	// 	printf(" acc: %s\n", Buf);
	if (base10){
		log10_decn();
	} else {
		ln_decn();
	}
	decn_to_str_complete(&AccDecn);
	CAPTURE(Buf);  // log(x)

	//calculate actual result
	bmp::mpfr_float::default_precision(50);
	bmp::mpfr_float calculated(Buf);
	std::string x_full_str(x_str);
	   x_full_str += "e" + std::to_string(x_exp);
	// 	CAPTURE(x_full_str);
	bmp::mpfr_float x_actual(x_full_str);
	if (base10){
		x_actual = log10(x_actual);
	} else {
		x_actual = log(x_actual);
	}
	CAPTURE(x_actual);
	bmp::mpfr_float rel_diff = abs((x_actual - calculated) / x_actual);
	CHECK(rel_diff < 3e-16); //TODO
}

TEST_CASE("log"){
	log_test("0.155", 0);
	log_test("10", 0);
	log_test("1.1", 10);
	log_test("2.02", -10);
	log_test("2.02", 0);
	log_test("1.5", 0, true);
	log_test("9", 99);
}

static void exp_test(
	//input
	const char* x_str, int x_exp,
	double epsilon=6e-16
)
{
	CAPTURE(x_str); CAPTURE(x_exp);
	build_dec80(x_str, x_exp);
	exp_decn();
	decn_to_str_complete(&AccDecn);
	CAPTURE(Buf);  // exp(x)

	//calculate actual result
	bmp::mpfr_float::default_precision(50);
	bmp::mpfr_float calculated(Buf);
	std::string x_full_str(x_str);
	x_full_str += "e" + std::to_string(x_exp);
	bmp::mpfr_float x_actual(x_full_str);
	x_actual = exp(x_actual);
	CAPTURE(x_actual);
	bmp::mpfr_float rel_diff = abs((x_actual - calculated) / x_actual);
	CHECK(rel_diff < epsilon);
}

TEST_CASE("exp"){
	exp_test("4.4", 0);
	exp_test("0.155", 0);
	exp_test("9.999", 0);
	exp_test("10", 0);
	exp_test("10.001", 0);
	exp_test("2.3", 2, 6e-15);
	exp_test("2.02", -10);
	exp_test("2.02", 0);
	exp_test("1.5", 0);
}

static void pow_test(
	//input
	const char* a_str, int a_exp,
	const char* b_str, int b_exp
)
{
	CAPTURE(a_str); CAPTURE(a_exp);
	CAPTURE(b_str); CAPTURE(b_exp);

	//compute power
	log_test(a_str, a_exp);
	build_decn_at(&BDecn,   b_str, b_exp);
	mult_decn();
	int8_t exp_test_exp = decn_to_str(&AccDecn);
	std::string copied(Buf);
	exp_test(copied.c_str(), exp_test_exp, 6e-15);

	decn_to_str_complete(&AccDecn);
	CAPTURE(Buf);  // a^b

	//calculate actual result
	bmp::mpfr_float::default_precision(50);
	bmp::mpfr_float calculated(Buf);
	std::string a_full_str(a_str);
	a_full_str += "e" + std::to_string(a_exp);
	std::string b_full_str(b_str);
	b_full_str += "e" + std::to_string(b_exp);;
	// 	CAPTURE(a_full_str);
	// 	CAPTURE(b_full_str);
	bmp::mpfr_float a_actual(a_full_str);
	bmp::mpfr_float b_actual(b_full_str);
	a_actual = exp(log(a_actual) * b_actual);
	bmp::mpfr_float rel_diff = abs((a_actual - calculated) / a_actual);
	CHECK(rel_diff < 3e-14);
}

TEST_CASE("power"){
	pow_test(
		"3.14", 60,
		"-1.5", -2
	);

	pow_test(
		"3", 0,
		"201", 0
	);
}
