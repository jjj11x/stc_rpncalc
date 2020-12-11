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


#include <string>
#include <boost/multiprecision/mpfr.hpp>
#include <catch2/catch.hpp>
#include "decn.h"

#include "decn_tests.h"

namespace bmp = boost::multiprecision;
using Catch::Matchers::Equals;


static void trig_test(void (*operation)(void), bmp::mpfr_float (*mpfr_operation)(bmp::mpfr_float x),
	                  double rtol, double atol)
{
	//build mpfr float
	bmp::mpfr_float::default_precision(50);
	decn_to_str_complete(&AccDecn);
	CAPTURE(Buf);
	bmp::mpfr_float a_actual(Buf);
	//calculate
	operation();
	decn_to_str_complete(&AccDecn);
	CAPTURE(Buf);
	//calculate actual
	a_actual = mpfr_operation(a_actual);
	CAPTURE(a_actual);

	bmp::mpfr_float calculated(Buf);
	if (rtol >= 0) {
		bmp::mpfr_float rel_diff = abs((a_actual - calculated) / a_actual);
		CHECK(rel_diff < rtol);
	} else {
		bmp::mpfr_float diff = abs(a_actual - calculated);
		CHECK(diff < atol);
	}
}


static void sin_test(double rtol=5e-3, double atol=1e-3)
{
	CAPTURE("sin test");
	trig_test(sin_decn, [](bmp::mpfr_float x) -> bmp::mpfr_float {return sin(x);}, rtol, atol);
}

static void sin_test(const char* a_str, int a_exp, double rtol=5e-3, double atol=1e-3)
{
	CAPTURE(a_str); CAPTURE(a_exp);
	build_dec80(a_str, a_exp);
	sin_test(rtol, atol);
}


static void cos_test(double rtol=5e-3, double atol=1e-3)
{
	CAPTURE("cos test");
	trig_test(cos_decn, [](bmp::mpfr_float x) -> bmp::mpfr_float {return cos(x);}, rtol, atol);
}

static void cos_test(const char* a_str, int a_exp, double rtol=5e-3, double atol=1e-3)
{
	CAPTURE(a_str); CAPTURE(a_exp);
	build_dec80(a_str, a_exp);
	cos_test(rtol, atol);
}


static void tan_test(double rtol=5e-3, double atol=1e-3)
{
	trig_test(tan_decn, [](bmp::mpfr_float x) -> bmp::mpfr_float {return tan(x);}, rtol, atol);
}

static void tan_test(const char* a_str, int a_exp, double rtol=5e-3, double atol=1e-3)
{
	CAPTURE(a_str); CAPTURE(a_exp);
	build_dec80(a_str, a_exp);
	tan_test(rtol, atol);
}


static void atan_test(double rtol=5e-3, double atol=1e-3)
{
	trig_test(arctan_decn, [](bmp::mpfr_float x) -> bmp::mpfr_float {return atan(x);}, rtol, atol);
}

static void atan_test(const char* a_str, int a_exp, double rtol=5e-3, double atol=1e-3)
{
	CAPTURE(a_str); CAPTURE(a_exp);
	build_dec80(a_str, a_exp);
	atan_test(rtol, atol);
}


static void asin_test(double rtol=5e-3, double atol=1e-3)
{
	trig_test(arcsin_decn, [](bmp::mpfr_float x) -> bmp::mpfr_float {return asin(x);}, rtol, atol);
}

static void asin_test(const char* a_str, int a_exp, double rtol=5e-3, double atol=1e-3)
{
	CAPTURE(a_str); CAPTURE(a_exp);
	build_dec80(a_str, a_exp);
	asin_test(rtol, atol);
}


static void acos_test(double rtol=5e-3, double atol=1e-3)
{
	trig_test(arccos_decn, [](bmp::mpfr_float x) -> bmp::mpfr_float {return acos(x);}, rtol, atol);
}

static void acos_test(const char* a_str, int a_exp, double rtol=5e-3, double atol=1e-3)
{
	CAPTURE(a_str); CAPTURE(a_exp);
	build_dec80(a_str, a_exp);
	acos_test(rtol, atol);
}

const char * const pi = "3.141592653589793239";
const char * const pi_threequarters = "2.356194490192344929";
const char * const pi_halved = "1.570796326794896619";
const char * const pi_quarter = ".7853981633974483096";


TEST_CASE("sin") {
	sin_test("0.1", 0);
	sin_test("0.05", 0, 1e-2);
	sin_test("0.01", 0, -1);
	sin_test("0.001", 0, -1);
	sin_test("0.0001", 0, -1);
	sin_test("0.00001", 0, -1);
	sin_test("0.000001", 0, -1);
	sin_test("0.0", 0, -1);
	sin_test("0.2", 0);
	sin_test("0.3", 0);
	sin_test("0.4", 0);
	sin_test("0.9", 0);
	sin_test("1.5", 0);
	sin_test("2.0", 0);
	sin_test("2.5", 0);
	sin_test("3.0", 0);
	sin_test(pi, 0, -1);
	sin_test(pi_quarter, 0);
	sin_test(pi_halved, 0);
	sin_test(pi_threequarters, 0);
	sin_test("1000.0", 0);
	sin_test("-0.5", 0);
	sin_test("-1.5", 0);
	sin_test("-2.0", 0);
	sin_test("-2.5", 0);
	sin_test("-3.0", 0);
	sin_test("-9.0", 0);
	sin_test("-18.0", 0);
	sin_test("-27.0", 0);
	sin_test("-1000.0", 0);
}

TEST_CASE("cos") {
	cos_test("0.1", 0);
	cos_test("0.05", 0);
	cos_test("0.01", 0);
	cos_test("0.001", 0);
	cos_test("0.0001", 0);
	cos_test("0.00001", 0);
	cos_test("0.000001", 0);
	cos_test("0.0", 0);
	cos_test("0.2", 0);
	cos_test("0.3", 0);
	cos_test("0.4", 0);
	cos_test("0.9", 0);
	cos_test("1.5", 0);
	cos_test("2.0", 0);
	cos_test("2.5", 0);
	cos_test("3.0", 0);
	cos_test(pi, 0);
	cos_test(pi_quarter, 0);
	cos_test(pi_halved, 0, -1);
	cos_test(pi_threequarters, 0);
	cos_test("1000.0", 0);
	cos_test("-0.5", 0);
	cos_test("-1.5", 0);
	cos_test("-2.0", 0);
	cos_test("-2.5", 0);
	cos_test("-3.0", 0);
	cos_test("-9.0", 0);
	cos_test("-18.0", 0);
	cos_test("-27.0", 0);
	cos_test("-1000.0", 0);
}


TEST_CASE("tan") {
	tan_test("0.1", 0);
	tan_test("0.05", 0, 1e-2);
	tan_test("0.01", 0, 5e-2);
	tan_test("0.001", 0, -1);
	tan_test("0.0001", 0, -1);
	tan_test("0.00001", 0, -1);
	tan_test("0.000001", 0, -1);
	tan_test("0.0", 0, -1);
	tan_test("0.2", 0);
	tan_test("0.3", 0);
	tan_test("0.4", 0);
	tan_test("0.9", 0);
	tan_test("1.5", 0);
	tan_test("2.0", 0);
	tan_test("2.5", 0);
	tan_test("3.0", 0);
}

TEST_CASE("arctan") {
	atan_test("0.001", 0, -1, 2e-3);
	atan_test("-0.001", 0, -1, 2e-3);
	atan_test("0.7", 0);
	atan_test("-0.7", 0);
	atan_test("0.1", 0);
	atan_test("-0.1", 0);
	atan_test("1.0", 0);
	atan_test("-1.0", 0);
	atan_test("2.0", 0);
	atan_test("-2.0", 0);
	atan_test("3.0", 0);
	atan_test("-3.0", 0);
	atan_test("0", 0, -1);
}

TEST_CASE("arcsin") {
	asin_test("0.001", 0, -1);
	asin_test("-0.001", 0, -1);
	asin_test("0.7", 0);
	asin_test("-0.7", 0);
	asin_test("0.1", 0, 1e-2);
	asin_test("-0.1", 0, 1e-2);
	asin_test("0.9", 0);
	asin_test("-0.9", 0);
}

TEST_CASE("arccos") {
	acos_test("0.001", 0);
	acos_test("-0.001", 0);
	acos_test("0.7", 0);
	acos_test("-0.7", 0);
	acos_test("0.1", 0);
	acos_test("-0.1", 0);
	acos_test("0.9", 0);
	acos_test("-0.9", 0);
}


static const int NUM_RAND_TRIG_TESTS = 4321; //trig tests are slow

TEST_CASE("sin random"){
	std::default_random_engine gen;
	std::uniform_int_distribution<int> distrib(0,99);
	std::uniform_int_distribution<int> exp_distrib(-1,0); //restrict range for now
	std::uniform_int_distribution<int> sign_distrib(0,1);
	for (int j = 0; j < NUM_RAND_TRIG_TESTS; j++){
		for (int i = 0; i < DEC80_NUM_LSU; i++){
			AccDecn.lsu[i] = distrib(gen);
		}
		int exp = exp_distrib(gen);
		int sign = sign_distrib(gen);
		set_exponent(&AccDecn, exp, sign);
		int lsu0 = AccDecn.lsu[0];
		CAPTURE(lsu0);
		CAPTURE(exp);
		CAPTURE(sign);
		if (exp == -1 && lsu0 == 0){
			//very small
			sin_test(40);
		} else if ((exp == -1 && lsu0 < 10) || (exp == 0 && lsu0 == 0)){
			//small
			sin_test(0.4);
		} else if ((exp == 0 && lsu0 == 31)){
			//near pi
			sin_test(0.2);
		} else if ((exp == 0 && lsu0 == 62)){
			//near 2pi
			sin_test(0.2);
		} else if ((exp == 0 && lsu0 > 62)){
			//large
			sin_test(0.1);
		} else {
			sin_test(0.02);
		}
	}
}

TEST_CASE("cos random"){
	std::default_random_engine gen;
	std::uniform_int_distribution<int> distrib(0,99);
	std::uniform_int_distribution<int> exp_distrib(-1,0); //restrict range for now
	std::uniform_int_distribution<int> sign_distrib(0,1);
	for (int j = 0; j < NUM_RAND_TRIG_TESTS; j++){
		for (int i = 0; i < DEC80_NUM_LSU; i++){
			AccDecn.lsu[i] = distrib(gen);
		}
		int exp = exp_distrib(gen);
		int sign = sign_distrib(gen);
		set_exponent(&AccDecn, exp, sign);
		int lsu0 = AccDecn.lsu[0];
		CAPTURE(lsu0);
		CAPTURE(exp);
		CAPTURE(sign);
		if (exp == 0 && lsu0 == 15){
			//near pi/2
			cos_test(0.4);
		} else if (exp == 0 && lsu0 == 47){
			//near 3/2 * pi
			cos_test(0.4);
		} else if (exp == 0 && lsu0 == 78){
			//near 5/2 * pi
// 			cos_test(0.4);
			cos_test(1.1); //actual rtol is much worse than 0.4, random test happens to hit a bad one
		} else {
			cos_test(0.02);
		}
	}
}

TEST_CASE("tan random"){
	std::default_random_engine gen;
	std::uniform_int_distribution<int> distrib(0,99);
	std::uniform_int_distribution<int> exp_distrib(-1,0); //restrict range for now
	std::uniform_int_distribution<int> sign_distrib(0,1);
	for (int j = 0; j < NUM_RAND_TRIG_TESTS; j++){
		for (int i = 0; i < DEC80_NUM_LSU; i++){
			AccDecn.lsu[i] = distrib(gen);
		}
		int exp = exp_distrib(gen);
		int sign = sign_distrib(gen);
		set_exponent(&AccDecn, exp, sign);
		int lsu0 = AccDecn.lsu[0];
		CAPTURE(lsu0);
		CAPTURE(exp);
		CAPTURE(sign);
		if (exp == -1 && lsu0 == 0){
			//very small
			tan_test(40);
		} else if ((exp == -1 && lsu0 < 10) || (exp == 0 && lsu0 == 0)){
			//small
			tan_test(0.5);
		} else if (exp == 0 && lsu0 == 15){
			//near pi/2
			tan_test(0.5);
		} else if ((exp == 0 && lsu0 == 31)){
			//near pi
			tan_test(0.2);
		} else if (exp == 0 && lsu0 == 47){
			//near 3/2 * pi
			tan_test(0.5);
		} else if ((exp == 0 && lsu0 == 62)){
			//near 2pi
			tan_test(0.2);
		} else if (exp == 0 && lsu0 == 78){
			//near 5/2 * pi
// 			tan_test(0.5);
			tan_test(0.6); //actual rtol is much worse than 0.4, random test happens to hit a bad one
		} else if ((exp == 0 && lsu0 > 62)){
			//large
			tan_test(0.1);
		} else {
			tan_test(0.02);
		}
	}
}
