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

static const bmp::mpfr_float mPI = boost::math::constants::pi<bmp::mpfr_float>();


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


static void sin_test(double rtol=6e-3, double atol=1e-3)
{
	CAPTURE("sin test");
	trig_test(sin_decn, [](bmp::mpfr_float x) -> bmp::mpfr_float {return sin(x * mPI / 180);}, rtol, atol);
}

static void sin_test(const char* a_str, int a_exp, double rtol=5e-3, double atol=1e-3)
{
	CAPTURE(a_str); CAPTURE(a_exp);
	build_dec80(a_str, a_exp);
	sin_test(rtol, atol);
}


static void cos_test(double rtol=6e-3, double atol=1e-3)
{
	CAPTURE("cos test");
	trig_test(cos_decn, [](bmp::mpfr_float x) -> bmp::mpfr_float {return cos(x * mPI / 180);}, rtol, atol);
}

static void cos_test(const char* a_str, int a_exp, double rtol=5e-3, double atol=1e-3)
{
	CAPTURE(a_str); CAPTURE(a_exp);
	build_dec80(a_str, a_exp);
	cos_test(rtol, atol);
}


static void tan_test(double rtol=5e-3, double atol=1e-3)
{
	trig_test(tan_decn, [](bmp::mpfr_float x) -> bmp::mpfr_float {return tan(x * mPI / 180);}, rtol, atol);
}

static void tan_test(const char* a_str, int a_exp, double rtol=5e-3, double atol=1e-3)
{
	CAPTURE(a_str); CAPTURE(a_exp);
	build_dec80(a_str, a_exp);
	tan_test(rtol, atol);
}


static void atan_test(double rtol=5e-3, double atol=1e-3)
{
	trig_test(arctan_decn, [](bmp::mpfr_float x) -> bmp::mpfr_float {return atan(x)*180/mPI;}, rtol, atol);
}

static void atan_test(const char* a_str, int a_exp, double rtol=5e-3, double atol=1e-3)
{
	CAPTURE(a_str); CAPTURE(a_exp);
	build_dec80(a_str, a_exp);
	atan_test(rtol, atol);
}


static void asin_test(double rtol=5e-3, double atol=1e-3)
{
	trig_test(arcsin_decn, [](bmp::mpfr_float x) -> bmp::mpfr_float {return asin(x)*180/mPI;}, rtol, atol);
}

static void asin_test(const char* a_str, int a_exp, double rtol=5e-3, double atol=1e-3)
{
	CAPTURE(a_str); CAPTURE(a_exp);
	build_dec80(a_str, a_exp);
	asin_test(rtol, atol);
}


static void acos_test(double rtol=5e-3, double atol=1e-3)
{
	trig_test(arccos_decn, [](bmp::mpfr_float x) -> bmp::mpfr_float {return acos(x)*180/mPI;}, rtol, atol);
}

static void acos_test(const char* a_str, int a_exp, double rtol=5e-3, double atol=1e-3)
{
	CAPTURE(a_str); CAPTURE(a_exp);
	build_dec80(a_str, a_exp);
	acos_test(rtol, atol);
}

// const char * const pi = "3.141592653589793239";
// const char * const pi_threequarters = "2.356194490192344929";
// const char * const pi_halved = "1.570796326794896619";
// const char * const pi_quarter = ".7853981633974483096";


TEST_CASE("sin") {
	sin_test("0.1", 0, 0.2);
	sin_test("0.0", 0, -1);
	sin_test("1.5", 0, 0.02);
	sin_test("2.0", 0, 0.02);
	sin_test("2.5", 0, 0.02);
	sin_test("3.0", 0, 0.02);
	sin_test("10", 0);
	sin_test("20", 0);
	sin_test("30", 0);
	sin_test("40", 0);
	sin_test("80", 0);
	sin_test("120", 0);
	sin_test("160", 0);
	sin_test("200", 0);
	sin_test("240", 0);
	sin_test("280", 0);
	sin_test("320", 0);
	sin_test("359", 0, 0.02);
	sin_test("360", 0, -1, 0.001);
	sin_test("361", 0, 0.02);
	sin_test("400", 0);
// 	sin_test(pi, 0, -1);
// 	sin_test(pi_quarter, 0);
// 	sin_test(pi_halved, 0);
// 	sin_test(pi_threequarters, 0);
	sin_test("180.0", 0, -1);
	sin_test("45.0", 0);
	sin_test("90.0", 0);
	sin_test("135.0", 0);
	sin_test("1000.0", 0);
	sin_test("-0.5", 0, 0.2);
	sin_test("-1.5", 0, 0.02);
	sin_test("-2.0", 0, 0.02);
	sin_test("-2.5", 0, 0.02);
	sin_test("-3.0", 0, 0.02);
	sin_test("-9.0", 0);
	sin_test("-18.0", 0);
	sin_test("-27.0", 0);
	sin_test("-1000.0", 0);
	sin_test("-30", 0);
	sin_test("-40", 0);
	sin_test("-80", 0);
	sin_test("-120", 0);
	sin_test("-160", 0);
	sin_test("-200", 0);
	sin_test("-240", 0);
	sin_test("-280", 0);
	sin_test("-320", 0);
	sin_test("-360", 0, -1, 0.001);
	sin_test("-400", 0);
}

TEST_CASE("cos") {
	cos_test("0.1", 0, 0.2);
	cos_test("0.0", 0, -1);
	cos_test("1.5", 0, 0.02);
	cos_test("2.0", 0, 0.02);
	cos_test("2.5", 0, 0.02);
	cos_test("3.0", 0, 0.02);
	cos_test("10", 0);
	cos_test("20", 0);
	cos_test("30", 0);
	cos_test("40", 0);
	cos_test("80", 0);
	cos_test("120", 0);
	cos_test("160", 0);
	cos_test("200", 0);
	cos_test("240", 0);
	cos_test("280", 0, 0.006);
	cos_test("320", 0);
	cos_test("359", 0, 0.02);
	cos_test("360", 0, -1, 0.001);
	cos_test("361", 0, 0.02);
	cos_test("400", 0);
// 	cos_test(pi, 0, -1);
// 	cos_test(pi_quarter, 0);
// 	cos_test(pi_halved, 0);
// 	cos_test(pi_threequarters, 0);
	cos_test("180.0", 0, -1);
	cos_test("45.0", 0);
	cos_test("90.0", 0, -1, 0.001);
	cos_test("135.0", 0);
	cos_test("1000.0", 0, 0.006);
	cos_test("-0.5", 0, 0.2);
	cos_test("-1.5", 0, 0.02);
	cos_test("-2.0", 0, 0.02);
	cos_test("-2.5", 0, 0.02);
	cos_test("-3.0", 0, 0.02);
	cos_test("-9.0", 0);
	cos_test("-18.0", 0);
	cos_test("-27.0", 0);
	cos_test("-1000.0", 0);
	cos_test("-30", 0);
	cos_test("-40", 0);
	cos_test("-80", 0, 0.006);
	cos_test("-120", 0);
	cos_test("-160", 0);
	cos_test("-200", 0);
	cos_test("-240", 0);
	cos_test("-280", 0);
	cos_test("-320", 0);
	cos_test("-360", 0, -1, 0.001);
	cos_test("-400", 0);
}


TEST_CASE("tan") {
	tan_test("0.1", 0, 0.2);
	tan_test("0.0", 0, -1);
	tan_test("1.5", 0, 0.02);
	tan_test("2.0", 0, 0.02);
	tan_test("2.5", 0, 0.02);
	tan_test("3.0", 0, 0.02);
	tan_test("10", 0);
	tan_test("20", 0);
	tan_test("30", 0);
	tan_test("40", 0);
	tan_test("80", 0);
	tan_test("120", 0);
	tan_test("160", 0);
	tan_test("200", 0);
	tan_test("240", 0);
	tan_test("280", 0, 0.006);
	tan_test("320", 0);
	tan_test("359", 0, 0.02);
	tan_test("360", 0, -1, 0.001);
	tan_test("361", 0, 0.02);
	tan_test("400", 0);
// 	tan_test(pi, 0, -1);
// 	tan_test(pi_quarter, 0);
// 	tan_test(pi_halved, 0);
// 	tan_test(pi_threequarters, 0);
	tan_test("180.0", 0, -1);
	tan_test("45.0", 0);
	tan_test("90.0", 0, 2);
	tan_test("135.0", 0);
	tan_test("1000.0", 0, 0.006);
	tan_test("-0.5", 0, 0.2);
	tan_test("-1.5", 0, 0.02);
	tan_test("-2.0", 0, 0.02);
	tan_test("-2.5", 0, 0.02);
	tan_test("-3.0", 0, 0.02);
	tan_test("-9.0", 0);
	tan_test("-18.0", 0);
	tan_test("-27.0", 0);
	tan_test("-1000.0", 0);
	tan_test("-30", 0);
	tan_test("-40", 0);
	tan_test("-80", 0, 0.006);
	tan_test("-120", 0);
	tan_test("-160", 0);
	tan_test("-200", 0);
	tan_test("-240", 0);
	tan_test("-280", 0);
	tan_test("-320", 0);
	tan_test("-360", 0, -1, 0.001);
	tan_test("-400", 0);
}

TEST_CASE("arctan") {
	atan_test("0.001",  0, -1, 0.06);
	atan_test("-0.001", 0, -1, 0.06);
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
	atan_test("0", 0, -1, 0.06);
}

TEST_CASE("arcsin") {
	asin_test("0.001",  0, -1, 0.06);
	asin_test("-0.001", 0, -1, 0.06);
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
	std::uniform_int_distribution<int> exp_distrib(0, 2); //restrict range for now
	std::uniform_int_distribution<int> sign_distrib(0,1);
	for (int j = 0; j < NUM_RAND_TRIG_TESTS; j++){
		for (int i = 0; i < DEC80_NUM_LSU; i++){
			AccDecn.lsu[i] = distrib(gen);
		}
		int exp = exp_distrib(gen);
		int sign = sign_distrib(gen);
		set_exponent(&AccDecn, exp, sign);
		remove_leading_zeros(&AccDecn);
		int lsu0 = AccDecn.lsu[0];
		exp = get_exponent(&AccDecn);
		CAPTURE(lsu0);
		CAPTURE(exp);
		CAPTURE(sign);
		if (exp <= -1){
			//very small
			sin_test(4000);
		} else if (exp == 0 && lsu0 < 50){
			//small
			sin_test(0.4);
		} else if (exp == 2 && lsu0 >= 17 && lsu0 <= 19){
			//near 180
			sin_test(3);
		} else if (exp == 2 && lsu0 >= 35 && lsu0 <= 36){
			//near 360
			sin_test(12);
		} else if (exp == 2 && lsu0 > 50){
			//large
			sin_test(35);
		} else {
			sin_test(0.02);
		}
	}
}

TEST_CASE("cos random"){
	std::default_random_engine gen;
	std::uniform_int_distribution<int> distrib(0,99);
	std::uniform_int_distribution<int> exp_distrib(0, 2); //restrict range for now
	std::uniform_int_distribution<int> sign_distrib(0,1);
	for (int j = 0; j < NUM_RAND_TRIG_TESTS; j++){
		for (int i = 0; i < DEC80_NUM_LSU; i++){
			AccDecn.lsu[i] = distrib(gen);
		}
		int exp = exp_distrib(gen);
		int sign = sign_distrib(gen);
		set_exponent(&AccDecn, exp, sign);
		remove_leading_zeros(&AccDecn);
		int lsu0 = AccDecn.lsu[0];
		exp = get_exponent(&AccDecn);
		CAPTURE(lsu0);
		CAPTURE(exp);
		CAPTURE(sign);
		if (exp == 1 && lsu0 >= 89 && lsu0 <= 90){
			//very near 90
			cos_test(500);
		} else if (exp == 1 && lsu0 >= 87 && lsu0 <= 92){
			//near 90
			cos_test(2);
		} else if (exp == 2 && lsu0 >= 26 && lsu0 <= 27){
			//near 270
			cos_test(500);
		} else if (exp == 2 && lsu0 >= 44){
			//large
			cos_test(20);
		} else {
			cos_test(0.02);
		}
	}
}

TEST_CASE("tan random"){
	std::default_random_engine gen;
	std::uniform_int_distribution<int> distrib(0,99);
	std::uniform_int_distribution<int> exp_distrib(0, 2); //restrict range for now
	std::uniform_int_distribution<int> sign_distrib(0,1);
	for (int j = 0; j < NUM_RAND_TRIG_TESTS; j++){
		for (int i = 0; i < DEC80_NUM_LSU; i++){
			AccDecn.lsu[i] = distrib(gen);
		}
		int exp = exp_distrib(gen);
		int sign = sign_distrib(gen);
		set_exponent(&AccDecn, exp, sign);
		remove_leading_zeros(&AccDecn);
		int lsu0 = AccDecn.lsu[0];
		exp = get_exponent(&AccDecn);
		CAPTURE(lsu0);
		CAPTURE(exp);
		CAPTURE(sign);
		if (exp <= -3){
			//extremely small
			tan_test(5000);
		} if (exp <= -1){
			//very small
			tan_test(400);
		} else if (exp == 0 && lsu0 < 50){
			//small
			tan_test(1);
		} else if (exp == 1 && lsu0 >= 89 && lsu0 <= 90){
			//very near 90
			tan_test(5);
		} else if (exp == 1 && lsu0 >= 87 && lsu0 <= 92){
			//near 90
			tan_test(1);
		} else if (exp == 2 && lsu0 >= 17 && lsu0 <= 19){
			//near 180
			tan_test(3);
		} else if (exp == 2 && lsu0 >= 26 && lsu0 <= 27){
			//near 270
			tan_test(5);
		} else if (exp == 2 && lsu0 >= 35 && lsu0 <= 37){
			//near 360
			tan_test(20);
		} else if (exp == 2 && lsu0 >= 44){
			//large
			tan_test(50);
		} else {
			tan_test(0.02);
		}
	}
}

TEST_CASE("atan random"){
	std::default_random_engine gen;
	std::uniform_int_distribution<int> distrib(0, 99);
	std::uniform_int_distribution<int> exp_distrib(-1, 0); //restrict range for now
	std::uniform_int_distribution<int> sign_distrib(0, 1);
	for (int j = 0; j < NUM_RAND_TRIG_TESTS; j++){
		for (int i = 0; i < DEC80_NUM_LSU; i++){
			AccDecn.lsu[i] = distrib(gen);
		}
		int exp = exp_distrib(gen);
		int sign = sign_distrib(gen);
		set_exponent(&AccDecn, exp, sign);
		remove_leading_zeros(&AccDecn);
		int lsu0 = AccDecn.lsu[0];
		exp = get_exponent(&AccDecn);
		CAPTURE(lsu0);
		CAPTURE(exp);
		CAPTURE(sign);
		if (exp <= -6){
			//extremely small
			atan_test(10000);
		} else if (exp < -1 || (exp == -1 && lsu0 == 0)){
			//very small
			atan_test(100);
		} else if ((exp == -1 && lsu0 < 10) || (exp == 0 && lsu0 == 0)){
			//small
			atan_test(3);
		} else {
			atan_test(0.02);
		}
	}
}

TEST_CASE("asin random"){
	std::default_random_engine gen;
	std::uniform_int_distribution<int> distrib(0, 99);
	std::uniform_int_distribution<int> exp_distrib(-2, -1); //restrict range for now
	std::uniform_int_distribution<int> sign_distrib(0, 1);
	for (int j = 0; j < NUM_RAND_TRIG_TESTS; j++){
		for (int i = 0; i < DEC80_NUM_LSU; i++){
			AccDecn.lsu[i] = distrib(gen);
		}
		int exp = exp_distrib(gen);
		int sign = sign_distrib(gen);
		set_exponent(&AccDecn, exp, sign);
		remove_leading_zeros(&AccDecn);
		int lsu0 = AccDecn.lsu[0];
		exp = get_exponent(&AccDecn);
		CAPTURE(lsu0);
		CAPTURE(exp);
		CAPTURE(sign);
		if (exp <= -7) {
			//extremely small
			asin_test(50000);
		} else if (exp < -5) {
			//very very small
			asin_test(1000);
		} else if (exp < -1 || (exp == -1 && lsu0 == 0)){
			//very small
			asin_test(100);
		} else if ((exp == -1 && lsu0 < 10) || (exp == 0 && lsu0 == 0)){
			//small
			asin_test(0.5);
		} else {
			asin_test(0.02);
		}
	}
}


TEST_CASE("acos random"){
	std::default_random_engine gen;
	std::uniform_int_distribution<int> distrib(0, 99);
	std::uniform_int_distribution<int> exp_distrib(-2, -1); //restrict range for now
	std::uniform_int_distribution<int> sign_distrib(0, 1);
	for (int j = 0; j < NUM_RAND_TRIG_TESTS; j++){
		for (int i = 0; i < DEC80_NUM_LSU; i++){
			AccDecn.lsu[i] = distrib(gen);
		}
		int exp = exp_distrib(gen);
		int sign = sign_distrib(gen);
		set_exponent(&AccDecn, exp, sign);
		remove_leading_zeros(&AccDecn);
		int lsu0 = AccDecn.lsu[0];
		exp = get_exponent(&AccDecn);
		CAPTURE(lsu0);
		CAPTURE(exp);
		CAPTURE(sign);
		if ((exp == -1 && lsu0 == 99)){
			//near 1
			acos_test(10);
		} else {
			acos_test(0.02);
		}
	}
}
