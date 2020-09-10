
#include <string>
#include <boost/multiprecision/mpfr.hpp>
#include <catch.hpp>
#include "decn.h"
namespace bmp = boost::multiprecision;
using Catch::Matchers::Equals;


static void trig_test(void (*operation)(void), bmp::mpfr_float (*mpfr_operation)(bmp::mpfr_float x),
	const char* a_str, int a_exp, double rtol, double atol)
{
	CAPTURE(a_str); CAPTURE(a_exp);
	build_dec80(a_str, a_exp);
	operation();
	decn_to_str_complete(&AccDecn);
	CAPTURE(Buf);

	bmp::mpfr_float::default_precision(50);
	std::string a_full_str(a_str);
	a_full_str += "e" + std::to_string(a_exp);

	bmp::mpfr_float a_actual(a_full_str);
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

static void sin_test(
	const char* a_str, int a_exp, double rtol=5e-3, double atol=1e-3)
{
	trig_test(sin_decn, [](bmp::mpfr_float x) -> bmp::mpfr_float {return sin(x);}, a_str, a_exp, rtol, atol);
}

static void cos_test(
	const char* a_str, int a_exp, double rtol=5e-3, double atol=1e-3)
{
	trig_test(cos_decn, [](bmp::mpfr_float x) -> bmp::mpfr_float {return cos(x);}, a_str, a_exp, rtol, atol);
}

static void tan_test(
	const char* a_str, int a_exp, double rtol=5e-3, double atol=1e-3)
{
	trig_test(tan_decn, [](bmp::mpfr_float x) -> bmp::mpfr_float {return tan(x);}, a_str, a_exp, rtol, atol);
}

static void atan_test(
	const char* a_str, int a_exp, double rtol=5e-3, double atol=1e-3)
{
	trig_test(arctan_decn, [](bmp::mpfr_float x) -> bmp::mpfr_float {return atan(x);}, a_str, a_exp, rtol, atol);
}

static void asin_test(
	const char* a_str, int a_exp, double rtol=5e-3, double atol=1e-3)
{
	trig_test(arcsin_decn, [](bmp::mpfr_float x) -> bmp::mpfr_float {return asin(x);}, a_str, a_exp, rtol, atol);
}

static void acos_test(
	const char* a_str, int a_exp, double rtol=5e-3, double atol=1e-3)
{
	trig_test(arccos_decn, [](bmp::mpfr_float x) -> bmp::mpfr_float {return acos(x);}, a_str, a_exp, rtol, atol);
}


const char * const pi = "3.141592653589793239";
const char * const pi_threequarters = "2.356194490192344929";
const char * const pi_halfed = "1.570796326794896619";
const char * const pi_quarted = ".7853981633974483096";


TEST_CASE("sin") {
	sin_test("0.1", 0);
	sin_test("0.05", 0);
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
	sin_test(pi_quarted, 0);
	sin_test(pi_halfed, 0);
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
	cos_test(pi_quarted, 0);
	cos_test(pi_halfed, 0, -1);
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
	tan_test("0.05", 0);
	tan_test("0.01", 0);
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
	atan_test("0.001", 0);
	atan_test("-0.001", 0);
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
	asin_test("0.1", 0);
	asin_test("-0.1", 0);
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
