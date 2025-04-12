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
 * decn_tests_div_sqrt.cpp
 *
 * Unit tests using https://github.com/catchorg/Catch2
 *
 * separate out reciprocal/division and sqrt tests
 *
 *  Created on: Oct 26, 2020
 */


#include <string>
#include <random>
#include <boost/multiprecision/mpfr.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include "decn.h"
#include "../utils.h"

#include "decn_tests.h"


namespace bmp = boost::multiprecision;
using Catch::Matchers::Equals;


static void div_test(){ //acc / b
	bmp::mpf_float::default_precision(50);
	decn_to_str_complete(&AccDecn);
	CAPTURE(Buf);
	bmp::mpfr_float a_actual(Buf);
	decn_to_str_complete(&BDecn);
	CAPTURE(Buf);
	bmp::mpfr_float b_actual(Buf);
	//calc result
	div_decn();
	decn_to_str_complete(&AccDecn);
	CAPTURE(Buf);  // acc / b

	//calculate actual result
	a_actual /= b_actual;
	if (decn_is_nan(&AccDecn)){
		//check that NaN result of division by 0
		CAPTURE(a_actual);
		CHECK(b_actual == 0);
	} else {
		bmp::mpfr_float calculated(Buf);
		bmp::mpfr_float rel_diff = abs((a_actual - calculated) / a_actual);
		CHECK(rel_diff < 2e-17);
	}
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
	div_test();
}

TEST_CASE("division"){
	div_test(
		"1", 0,
		"0", 0
	);

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

TEST_CASE("division random"){
	std::default_random_engine gen;
	std::uniform_int_distribution<int> distrib(0, 99);
	std::uniform_int_distribution<int> sign_distrib(0,1);
	for (int j = 0; j < NUM_RAND_TESTS; j++){
		AccDecn.lsu[0] = distrib(gen);
		BDecn.lsu[0] = distrib(gen);
		for (int i = 1; i < DEC80_NUM_LSU; i++){
			AccDecn.lsu[i] = distrib(gen);
			BDecn.lsu[i] = distrib(gen);
		}
		set_exponent(&AccDecn, distrib(gen), sign_distrib(gen));
		set_exponent(&BDecn, distrib(gen), sign_distrib(gen));
		div_test();
	}
}

static void sqrt_test(){
	decn_to_str_complete(&AccDecn);
	CAPTURE(Buf);
	//calculate result
	sqrt_decn();
	//build mpfr float
	bmp::mpfr_float::default_precision(50);
	bmp::mpfr_float x_actual(Buf);
	//print calc result
	decn_to_str_complete(&AccDecn);
	CAPTURE(Buf);
	//calculate actual result
	CAPTURE(x_actual);
	if (decn_is_nan(&AccDecn)){
		//check that NaN is from result of sqrt(-)
		CHECK(x_actual <= 0);
	} else if (decn_is_zero(&AccDecn)){
		//check actual is also 0
		CHECK(x_actual == 0);
	} else {
		x_actual = sqrt(x_actual);
		CAPTURE(x_actual);
		bmp::mpfr_float calculated(Buf);
		bmp::mpfr_float rel_diff = abs((x_actual - calculated) / x_actual);
		CHECK(rel_diff < 2e-17);
	}
}

static void sqrt_test(const char* x_str, int x_exp)
{
	CAPTURE(x_str); CAPTURE(x_exp);
	build_dec80(x_str, x_exp);
	sqrt_test();
}

TEST_CASE("sqrt"){
	sqrt_test("0", 0);
	sqrt_test("2", 0);
	sqrt_test("-1", 0);
	sqrt_test("0.155", 0);
	sqrt_test("10", 0);
	sqrt_test("1.1", 10);
	sqrt_test("2.02", -10);
	sqrt_test("2.02", 0);
	sqrt_test("1.5", 0);
	sqrt_test("9", 99);
	sqrt_test("123", 12345);
}

TEST_CASE("sqrt random"){
	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(0,99);
	std::uniform_int_distribution<int> exp_distrib(-99,99);
	std::uniform_int_distribution<int> sign_distrib(0,1);
	for (int j = 0; j < NUM_RAND_TESTS; j++){
		for (int i = 0; i < DEC80_NUM_LSU; i++){
			AccDecn.lsu[i] = distribution(generator);
		}
		int sign = sign_distrib(generator);
		set_exponent(&AccDecn, exp_distrib(generator), sign);
		sqrt_test();
	}
}
