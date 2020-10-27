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


#include <string>
#include <random>
#include <boost/multiprecision/mpfr.hpp>
#include <catch2/catch.hpp>
#include "decn.h"
#include "../utils.h"

#include "decn_tests.h"


namespace bmp = boost::multiprecision;
using Catch::Matchers::Equals;


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

	//small positive
	build_dec80(".1", 3);
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("100."));

	build_dec80("0.1", -1);
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("0.01"));

	build_dec80("0.01", 3);
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("10."));

	//zero
	build_dec80(".", 3);
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("0"));

	build_dec80(".0", 3);
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("0"));

	build_dec80("0.", 3);
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("0"));

	build_dec80("-0.0", -1);
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("0"));

	build_dec80("0", 3);
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("0"));

	//small negative
	build_dec80("-.1", 3);
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("-100."));

	build_dec80("-0.1", -1);
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("-0.01"));

	build_dec80("-0.001", 3);
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("-1."));

	//empty string -> 0
	build_dec80("", 90);
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("0"));

	//too many .
	build_dec80("..", 3);
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("Error"));

	//very long (truncated)
	build_dec80("12345678901234567890", -2);
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("123456789012345678."));

	build_dec80("12345678901234567890", +2);
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("1.23456789012345678E21"));

	//overflow
	build_dec80("100", DEC80_MAX_EXP-1);
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("Error"));

	build_dec80("1", DEC80_MAX_EXP+1);
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("Error"));

	build_dec80("0.1", DEC80_MAX_EXP+2);
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("Error"));

	//underflow
	build_dec80("10", DEC80_MIN_EXP-2);
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("Error"));

	build_dec80("1", DEC80_MIN_EXP-1);
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("Error"));

	build_dec80("0.3", DEC80_MIN_EXP);
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("Error"));
	CHECK(decn_is_nan(&AccDecn) == 1);

	//left/right count
	build_dec80("-100.001", 3);
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("-100001."));

	//invalid
	build_dec80(":", 0);
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("Error"));

	//special number that is not NaN
	AccDecn.lsu[1] = 3;
	CHECK(decn_is_nan(&AccDecn) == 0);
}

TEST_CASE("build_large"){
	int large_exp = DEC80_MAX_EXP/2 - 50;
	build_dec80("9.99", large_exp);
	decn_to_str_complete(&AccDecn);
	CHECK(AccDecn.exponent == large_exp);
	std::string expected = "9.99E";
	expected += std::to_string(large_exp);
	CHECK_THAT(Buf, Equals(expected));
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

	//add 0
	build_decn_at(&BDecn, "0", 0);
	add_decn();
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("-9234.567890123456")); //same

	//carry into MSB
	build_dec80(          "-82345678901234567.8", -1);
	build_decn_at(&BDecn, "-87654321098765432.2", -1);
	add_decn();
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf,Equals("-17000000000000000.")); //acc+b

	//don't negate NaN
	set_dec80_NaN(&AccDecn);
	negate_decn(&AccDecn);
	CHECK(decn_is_nan(&AccDecn));
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

	//overflow
	build_dec80("9.99", DEC80_MAX_EXP/2);
	build_decn_at(&BDecn, "9.99", DEC80_MAX_EXP/2);
	mult_decn();
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("Error")); //acc*b

	//NaN
	build_dec80("9.99", DEC80_MAX_EXP/2);
	set_dec80_NaN(&BDecn);
	mult_decn();
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("Error")); //acc*b

	//NaN
	set_dec80_NaN(&AccDecn);
	build_decn_at(&BDecn, "9.99", DEC80_MAX_EXP/2);
	mult_decn();
	decn_to_str_complete(&AccDecn);
	CHECK_THAT(Buf, Equals("Error")); //acc*b
}

TEST_CASE("u32str corner"){
	u32str(0, &Buf[0], 10);
	CHECK_THAT(Buf, Equals("0"));
}
