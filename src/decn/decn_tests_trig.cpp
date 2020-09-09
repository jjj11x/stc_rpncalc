
#include <string>
#include <boost/multiprecision/mpfr.hpp>
#include <catch.hpp>
#include "decn.h"
namespace bmp = boost::multiprecision;
using Catch::Matchers::Equals;

static void sin_test(
	const char* a_str, int a_exp, double rtol=5e-3, double atol=1e-3)
{
	CAPTURE(a_str); CAPTURE(a_exp);
	build_dec80(a_str, a_exp);
	sin_decn();
	decn_to_str_complete(&AccDecn);
	CAPTURE(Buf);

	bmp::mpfr_float::default_precision(50);
	std::string a_full_str(a_str);
	a_full_str += "e" + std::to_string(a_exp);

	bmp::mpfr_float a_actual(a_full_str);
	a_actual = sin(a_actual);
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

static void cos_test(
	const char* a_str, int a_exp, double rtol=1e-2)
{
	CAPTURE(a_str); CAPTURE(a_exp);
	build_dec80(a_str, a_exp);
	cos_decn();
	decn_to_str_complete(&AccDecn);
	CAPTURE(Buf);

	bmp::mpfr_float::default_precision(50);
	std::string a_full_str(a_str);
	a_full_str += "e" + std::to_string(a_exp);

	bmp::mpfr_float a_actual(a_full_str);
	a_actual = cos(a_actual);
	CAPTURE(a_actual);

	bmp::mpfr_float calculated(Buf);
	bmp::mpfr_float rel_diff = abs((a_actual - calculated) / a_actual);

	CHECK(rel_diff < rtol);
}

static void tan_test(
	const char* a_str, int a_exp, double rtol=1e-2, double atol=1e-3)
{
	CAPTURE(a_str); CAPTURE(a_exp);
	build_dec80(a_str, a_exp);
	tan_decn();
	decn_to_str_complete(&AccDecn);
	CAPTURE(Buf);

	bmp::mpfr_float::default_precision(50);
	std::string a_full_str(a_str);
	a_full_str += "e" + std::to_string(a_exp);

	bmp::mpfr_float a_actual(a_full_str);
	a_actual = tan(a_actual);
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
}


TEST_CASE("tan") {
	tan_test("0.1", 0);
	tan_test("0.05", 0);
	tan_test("0.01", 0);
	tan_test("0.001", 0);
	tan_test("0.0001", 0);
	tan_test("0.00001", 0);
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
