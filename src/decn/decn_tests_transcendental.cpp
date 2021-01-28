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
 * decn_tests_transcendental.cpp
 *
 * Unit tests using https://github.com/catchorg/Catch2
 *
 * separate out transcendental function tests
 *
 *  Created on: Oct 26, 2020
 */


#include <string>
#include <random>
#include <boost/multiprecision/mpfr.hpp>
#include <catch2/catch.hpp>
#include "decn.h"
#include "../utils.h"

#include "decn_tests.h"


namespace bmp = boost::multiprecision;
using Catch::Matchers::Equals;


static void log_test_(bool base10, double epsilon){
	bmp::mpfr_float::default_precision(50);
	CAPTURE(base10);
	decn_to_str_complete(&AccDecn);
	CAPTURE(Buf);
	//build mpfr float
	bmp::mpfr_float x_actual(Buf);
	//calculate result
	if (base10){
		log10_decn();
	} else {
		ln_decn();
	}
	decn_to_str_complete(&AccDecn);
	CAPTURE(Buf);  // log(x)

	//calculate actual result
	CAPTURE(x_actual);
	if (decn_is_nan(&AccDecn)){
		//check that NaN is from result of log(-)
		CHECK(x_actual <= 0);
	} else {
		if (base10){
			x_actual = log10(x_actual);
		} else {
			x_actual = log(x_actual);
		}
		bmp::mpfr_float calculated(Buf);
		CAPTURE(calculated);
		bmp::mpfr_float rel_diff = abs((x_actual - calculated) / x_actual);
		CHECK(rel_diff < epsilon);
	}
}

static void log_test(bool base10=false){
	//check if near 1.0
	remove_leading_zeros(&AccDecn);
	double lsu0 = AccDecn.lsu[0];
	int exp = get_exponent(&AccDecn);
	if (exp == -1){
		lsu0 /= (double) 10;
		lsu0 += (double) AccDecn.lsu[1] / (10*100);
		lsu0 += (double) AccDecn.lsu[2] / (10*100*100);
		lsu0 += (double) AccDecn.lsu[3] / (10*100*100*100);
	} else if (exp == 0){
		lsu0 += (double) AccDecn.lsu[1] / 100;
		lsu0 += (double) AccDecn.lsu[2] / (100*100);
		lsu0 += (double) AccDecn.lsu[3] / (100*100*100);
	}
	CAPTURE((int) AccDecn.lsu[0]); CAPTURE((int) AccDecn.lsu[1]);
	CAPTURE(exp);
	CAPTURE(lsu0);
	if (exp == 0 || exp == -1){
		//check if near 1.0
		if (lsu0 >= 7 && lsu0 < 8){
			log_test_(base10, 7.5e-16);
		} else if (lsu0 >= 8 && lsu0 < 9){
			log_test_(base10, 1.5e-15);
		} else if (lsu0 >= 9 && lsu0 < 9.6){
			log_test_(base10, 1.0e-14);
		} else if (lsu0 >= 9.6 && lsu0 < 9.9){
			log_test_(base10, 4.1e-13);
		} else if (lsu0 >= 9.9 && lsu0 < 9.999){
			log_test_(base10, 1.5e-11);
		} else if (lsu0 >= 9.999 && lsu0 < 9.99999){
			log_test_(base10, 6.0e-10);
		} else if (lsu0 >= 9.99999 && lsu0 < 9.9999999){
			log_test_(base10, 3.0e-9);
		} else if (lsu0 >= 9.9999999 && lsu0 < 10.0){
			log_test_(base10, 1.3e-7);
		} else if (lsu0 >= 10.0 && lsu0 < 10.00001){
			log_test_(base10, 6.0e-10);
		} else if (lsu0 >= 10.00001 && lsu0 < 10.001){
			log_test_(base10, 6.0e-11);
		} else if (lsu0 >= 10.001 && lsu0 < 10.1){
			log_test_(base10, 1.5e-12);
		} else if (lsu0 >= 10.1 && lsu0 < 11){
			log_test_(base10, 1.6e-14);
		} else if (lsu0 >= 11 && lsu0 < 13){
			log_test_(base10, 2.0e-15);
		} else {
			log_test_(base10, 6.5e-16);
		}
	} else {
		log_test_(base10, 2e-16);
	}
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
	log_test(base10);
}

TEST_CASE("log"){
	log_test("0", 0);
	log_test("-1", 0);
	log_test("0.155", 0);
	log_test("10", 0);
	log_test("1.1", 10);
	log_test("2.02", -10);
	log_test("2.02", 0);
	log_test("1.5", 0, true);
	log_test("9", 99);
	log_test("123", 12345);
}

TEST_CASE("log random"){
	std::default_random_engine gen;
	std::uniform_int_distribution<int> distrib(0,99);
	std::uniform_int_distribution<int> exp_distrib(-99,99);
	std::uniform_int_distribution<int> sign_distrib(0,1);
	for (int j = 0; j < NUM_RAND_TESTS; j++){
		for (int i = 0; i < DEC80_NUM_LSU; i++){
			AccDecn.lsu[i] = distrib(gen);
		}
		int exp = exp_distrib(gen);
		set_exponent(&AccDecn, exp, 0);
		int base10 = sign_distrib(gen);
		log_test(base10);
	}
}

static void log_test_near1(int lsu0_low, int lsu0_high, int exp){
	std::default_random_engine gen;
	std::uniform_int_distribution<int> lsu0_distrib(lsu0_low, lsu0_high);
	std::uniform_int_distribution<int> distrib(0,99);
	std::uniform_int_distribution<int> exp_distrib(-99,99);
	std::uniform_int_distribution<int> sign_distrib(0,1);
	for (int j = 0; j < NUM_RAND_TESTS; j++){
		AccDecn.lsu[0] = lsu0_distrib(gen);
		for (int i = 1; i < DEC80_NUM_LSU; i++){
			AccDecn.lsu[i] = distrib(gen);
		}
		set_exponent(&AccDecn, exp, 0);
		int base10 = sign_distrib(gen);
		log_test(base10);
	}
}

TEST_CASE("log random 0 to 0.99..."){
	log_test_near1(0,  99, -1);
}
TEST_CASE("log random 0.8 to 0.99..."){
	log_test_near1(80, 99, -1);
}
TEST_CASE("log random 1.0 to 9.9"){
	log_test_near1(10, 99, 0);
}
TEST_CASE("log random 1.0 to 2.0"){
	log_test_near1(10, 20, 0);
}

static void exp_test_(bool base10, double epsilon){
	bmp::mpfr_float::default_precision(50);
	CAPTURE(base10);
	decn_to_str_complete(&AccDecn);
	CAPTURE(Buf);  //x
	CAPTURE(AccDecn.exponent);
	//build mpfr float
	bmp::mpfr_float x_actual(Buf);
	//calculate result
	if (base10){
		exp10_decn();
	} else {
		exp_decn();
	}
	decn_to_str_complete(&AccDecn);
	CAPTURE(Buf);  // exp(x)

	//calculate actual result
	bmp::mpfr_float calculated(Buf);
	if (base10){
		x_actual *= log(10);
	}
	x_actual = exp(x_actual);
	CAPTURE(x_actual);
	bmp::mpfr_float rel_diff = abs((x_actual - calculated) / x_actual);
	CHECK(rel_diff < epsilon);
}

static void exp_test(bool base10=false){
	double x;
	int exp = get_exponent(&AccDecn);
	if (exp == 1){
		x = AccDecn.lsu[0];
		x += (double) AccDecn.lsu[1] / 100;
	} else if (exp == 2){
		x = (double) AccDecn.lsu[0] * 10;
		x += (double) AccDecn.lsu[1] / 10;
	}
	CAPTURE((int) AccDecn.lsu[0]); CAPTURE((int) AccDecn.lsu[1]);
	CAPTURE(exp);
	CAPTURE(x);
	double epsilon;
	if (exp == 1 || exp == 2){
		if        (x > 230){
			epsilon = 8e-15;
		} else if (x > 210){
			epsilon = 6e-15;
		} else if (x > 180){
			epsilon = 5e-15;
		} else if (x > 150){
			epsilon = 4e-15;
		} else if (x > 125){
			epsilon = 3e-15;
		} else if (x > 100){
			epsilon = 2e-15;
		} else if (x > 65){
			epsilon = 1e-15;
		}
	} else {
		epsilon = 6e-16;
	}
	CAPTURE(base10);
	if (base10){
		epsilon *= 20;
	}
	exp_test_(base10, epsilon);
}

static void exp_test(
	//input
	const char* x_str, int x_exp,
	bool base10=false
)
{
	CAPTURE(x_str); CAPTURE(x_exp);
	CAPTURE(base10);
	build_dec80(x_str, x_exp);
	exp_test(base10);
}

static void exp10_test(const char* x_str, int x_exp){
	exp_test(x_str, x_exp, true);
}

TEST_CASE("exp"){
	exp_test("4.4", 0);
	exp_test("0.155", 0);
	exp_test("9.999", 0);
	exp_test("10", 0);
	exp_test("10.001", 0);
	exp_test("2.3", 2);//, 6e-15);
	exp_test("2.02", -10);
	exp_test("2.02", 0);
	exp_test("1.5", 0);
	exp_test("99.999999", 0);
	exp_test("230.2", 0);//, 6e-15);
	exp_test("-230", 0);//, 6e-15);
	exp_test("294.69999999", 0);//, 8e-15);

	//do not operate on NaN
	set_dec80_NaN(&AccDecn);
	exp_decn();
	CHECK(decn_is_nan(&AccDecn)); //still NaN
}


TEST_CASE("exp10"){
	exp10_test("4.4", 0);
	exp10_test("0.155", 0);
	exp10_test("9.999", 0);
	exp10_test("10", 0);
	exp10_test("10.001", 0);
	exp10_test("2.02", -10);
	exp10_test("2.02", 0);
	exp10_test("1.5", 0);
	exp10_test("127", 0);//, 3e-14);
	exp10_test("99.999999", 0);//, 2e-14);
}

static void test_exp_random(int exp_distrib_low){
	std::default_random_engine gen;
	std::uniform_int_distribution<int> distrib(0, 99);
	std::uniform_int_distribution<int> lsu0_high_distrib(0, 23);
	std::uniform_int_distribution<int> exp_distrib(exp_distrib_low, 2);
	std::uniform_int_distribution<int> sign_distrib(0, 1);
	for (int j = 0; j < NUM_RAND_TESTS; j++){
		int exp = exp_distrib(gen);
		int sign = sign_distrib(gen);
		if (exp == 2) {
			//limit x to approximately +/- 230
			AccDecn.lsu[0] = lsu0_high_distrib(gen);
		} else {
			AccDecn.lsu[0] = distrib(gen);
		}
		for (int i = 1; i < DEC80_NUM_LSU; i++){
			AccDecn.lsu[i] = distrib(gen);
		}
		set_exponent(&AccDecn, exp, sign);
		exp_test();
	}
}

TEST_CASE("exp random"){
	test_exp_random(-99);
}
TEST_CASE("exp large random"){
	test_exp_random(1);
}

static void pow_test(){ // a^b
	bmp::mpf_float::default_precision(50);
	decn_to_str_complete(&AccDecn);
	CAPTURE(Buf);  // a
	bmp::mpfr_float a_actual(Buf);
	decn_to_str_complete(&BDecn);
	CAPTURE(Buf);  // b
	bmp::mpfr_float b_actual(Buf);
	//calculate result
	pow_decn();
	//calculate actual result
	bmp::mpfr_float res_actual(pow(a_actual, b_actual));
	//check overflow or underflow
	if (decn_is_nan(&AccDecn)){
		//check overflow or underflow
		if (b_actual > 0) {
			CHECK(log(res_actual) > 100);
		} else {
			CHECK(log(res_actual) < -100);
		}
		return;
	}
	//not over/underflow, get string and log calculated result
	decn_to_str_complete(&AccDecn);
	CAPTURE(Buf);  // a^b
	bmp::mpfr_float calculated(Buf);
	//check relative error
	double rel_tol = 4.5e-14;
	if (a_actual > 1.0 && a_actual < 1.0001){
		rel_tol = 1e-7;
	} else if (a_actual > 0.9 && a_actual < 2.0){
		rel_tol = 1.5e-10;
	} else if (log(res_actual) > 100){
		rel_tol = 1e-12;
	}
	CAPTURE(a_actual);
	CAPTURE(rel_tol);
	if (decn_is_zero(&AccDecn)) {
		bmp::mpfr_float diff = abs(res_actual - calculated);
		CHECK(diff < rel_tol);
	} else {
		bmp::mpfr_float rel_diff = abs((res_actual - calculated)/res_actual);
		CHECK(rel_diff < rel_tol);
	}
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
	build_decn_at(&BDecn,   b_str, b_exp);
	build_dec80(a_str, a_exp);
	pow_test();
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

	pow_test(
		"5", 0,
		"0", 0
	);

	pow_test(
		"5", 0,
		"0", 2
	);

	pow_test(
		"0", 0,
		"5", 0
	);

	pow_test(
		"0", 0,
		"0", 0
	);
}

static void power_test(int lsu0_low, int lsu0_high, int exp_low=-99, int exp_high=99){
	std::default_random_engine gen;
	std::uniform_int_distribution<int> lsu0_distrib(lsu0_low, lsu0_high);
	std::uniform_int_distribution<int> distrib(0, 99);
	std::uniform_int_distribution<int> exp_distrib(exp_low, exp_high);
	std::uniform_int_distribution<int> sign_distrib(0,1);
	for (int j = 0; j < NUM_RAND_TESTS; j++){
		AccDecn.lsu[0] = lsu0_distrib(gen);
		for (int i = 1; i < DEC80_NUM_LSU; i++){
			AccDecn.lsu[i] = distrib(gen);
			BDecn.lsu[i] = distrib(gen);
		}
		set_exponent(&AccDecn, exp_distrib(gen), 0);
		//generate exponent for b to minimize chance of a^b overflowing:
		// a^b <= 1e100
		// b*log(a) <= log(1e100) = 100
		// b <= 100/log(a)
		// b_exponent <= log(100/log(a)) = log(100) - log(log(a))
		// b_exponent <= 2 - log(log(a))
		decn_to_str_complete(&AccDecn);
		bmp::mpfr_float acc(Buf);
		acc = 2.0 - log(log(acc));
		double b_exponent_high_flt = acc.convert_to<double>();
		int b_exponent_high = b_exponent_high_flt;
		int b_exponent_low = -99;
		//ensure b_exponent high in range
		if (b_exponent_high > 99){
			b_exponent_high = 99;
		} else if (b_exponent_high < b_exponent_low){
			b_exponent_high = b_exponent_low;
		}
		CAPTURE(b_exponent_low);
		CAPTURE(b_exponent_high);
		std::uniform_int_distribution<int> b_exp_distrib(b_exponent_low, b_exponent_high);
		int b_exponent = b_exp_distrib(gen);
		CAPTURE(b_exponent);
		int b_neg = sign_distrib(gen);
		set_exponent(&BDecn, b_exponent, b_neg);
		pow_test();
	}
}

TEST_CASE("power random"){
	power_test(0, 99);
}
TEST_CASE("power random 0.9 to 0.99..."){
	power_test(90, 99, -1, -1);
}
TEST_CASE("power random 1.0 to 2.0..."){
	power_test(10, 20, 0, 0);
}
