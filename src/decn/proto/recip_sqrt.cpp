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


//prototype for calculating reciprocal square root 1/sqrt(x)
//based on 0x5F3759DF fast inverse square root:
// calculate using newton-raphson iterations,
// get initial estimate using linear approximation

#include <stdio.h>
#include <iostream>
#include <math.h>
#include <boost/multiprecision/mpfr.hpp>
using namespace boost::multiprecision;

// #define DEBUG

using std::cout;
using std::endl;


static const unsigned int CALC_PRECISION = 18;

int get_exp(mpfr_float& x){
	mpfr_float x_exp(log10(x), CALC_PRECISION);

	return x_exp.convert_to<int>();
}

double get_significand(mpfr_float& x){
	int exp = get_exp(x);
	double signif = x.convert_to<double>();
	signif = signif / pow(10.0, exp);

	return signif;
}


int main(void){
	cout << std::scientific << std::setprecision(CALC_PRECISION);

	//loop through values to test
#ifdef DEBUG
	mpfr_float x(0.1, CALC_PRECISION);
	{
#else
	for (mpfr_float x(1e-99, CALC_PRECISION); x < 1e99; x *= 1.03){
#endif
		//build mpf values
		mpfr_float x_orig_2(x/2, CALC_PRECISION); //copy of original x / 2
		mpfr_float est(0, CALC_PRECISION);        //current estimate for 1/sqrt(x)
		mpfr_float accum(0, CALC_PRECISION);      //working register
		mpfr_float_1000 actual;
		actual = 1  / sqrt(x);

		//get decimal floating point representation
		int x_exp = get_exp(x);
		double x_signif = get_significand(x);
		//normalize: make sure significand is between 1 and 10
		while (x_signif < 1.0){
			x_signif *= 10;
			x_exp--;
		}
#ifdef DEBUG
		printf("x = %f*10^%d\n", x_signif, x_exp);
#endif
		//get initial estimate for 1/sqrt(x) == 10^(-0.5 * log(x)):
		// approximate significand == 10^(-0.5 * log(x_signif))
		//  with linear approximation: -0.18 * x_signif + 2.5
		// new exponent part is (10^(-0.5 * log(10^x_exp)))
		//                    == 10^(-0.5 * x^exp)
		double est_signif;
		if (x_exp & 0x1){ //odd
			//increment x_exp and
			//approximate estimate significand as (-0.056*x_signif + 0.79) * 10^0.5
			//                                  == -0.18 * x_signif + 2.5
			x_exp++;
			est_signif = -0.18 * x_signif + 2.5;
		} else { //even
			//keep x_exp as is and approximate estimate significand as
			//                   -0.056*x_signif + 0.79
			est_signif = -0.056 * x_signif + 0.79;
		}
		int est_exp = -x_exp / 2;
		est = est_signif * pow(10.0, est_exp);
#ifdef DEBUG
		printf("x: %f (%d)\n", x_signif, x_exp);
		printf("est: %f (%d)\n", est_signif, est_exp);
		cout << "initial estimate for " << x << ": " << est << endl;
#endif

		//newton-raphson iterations
		for (int i = 0; i < 6; i++){
			accum = est * est;
			accum *= x_orig_2;     //accum = x/2 * est * est
			accum = -accum;
			accum += 1.5;          //accum = 3/2 - x/2 * est * est
			accum *= est;          //accum = 0.5 * est * (3 - x * est * est)
			est = accum;
#ifdef DEBUG
			printf("%2d: ", i);
			cout << est << endl;
#endif
		}


		//calculate relative error
		mpfr_float_1000 calc, diff;
		calc = accum;
		diff = abs((actual - calc)/actual);

#ifdef DEBUG_ALL
		if (1){
#else
		if (diff > 5e-17){
#endif
			cout << x << ": ";
			cout << std::setprecision(18) << accum << ", ";
#if 1
			cout << actual << ", ";
#endif
			cout << diff << endl;
		}
	}

	return 0;
}

