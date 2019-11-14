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



//prototype for natural log function based on HP Journal article
// "Personal Calculator Algorithms IV: Logarithmic Functions"

#include <stdio.h>
#include <iostream>
#include <math.h>
#include <boost/multiprecision/mpfr.hpp>
using namespace boost::multiprecision;

using std::cout;
using std::endl;

static const int NUM_A_ARR = 9;
mpfr_float a_arr[NUM_A_ARR];
mpfr_float ln_a_arr[NUM_A_ARR];

static const unsigned int CALC_PRECISION = 18;


int main(void){
	cout << std::scientific << std::setprecision(CALC_PRECISION);
	mpfr_float a(10, CALC_PRECISION);
	a = log(a);
	cout << "ln(10):" << a << endl;
	//initiallize a_arr and ln_a_arr
	for (int i = 0; i < NUM_A_ARR; i++){
		// a[i] = (1 + 10^-i)
		mpfr_float a(-i, CALC_PRECISION);
		a = pow(10, a);
		a_arr[i] = a + 1;
		ln_a_arr[i] = log(a_arr[i]);
		cout << i << endl;
		cout << "     a:" << a_arr[i] << endl;
		cout << " ln(a):" << ln_a_arr[i] << endl;
// 		a_arr[i].assign(a_arr1000[i], CALC_PRECISION);
// 		ln_a_arr[i].assign(ln_a_arr1000[i], CALC_PRECISION);
	}
	cout << endl << endl ;
	//loop through values to test
	for (mpfr_float x(0.0001, CALC_PRECISION); x < 1.0; x += 0.015625){
		//build mpf values
		mpfr_float A(x, CALC_PRECISION);
		mpfr_float b_j(1.0 - A, CALC_PRECISION); //initial b_0 = 1 - A
		mpfr_float accum(b_j, CALC_PRECISION);
		mpfr_float_1000 actual;
		actual = log(A);

		int num_times[NUM_A_ARR];
		//track number of times multiplied by a_arr[j]
		for (int j = 0; j < NUM_A_ARR; j++){
			if (j != 0){
				b_j *= 10.0;
			}
			accum = b_j;
			int k_j = 0;
			while (accum > 0){
				b_j = accum;
				accum *= a_arr[j];
				accum -= 1.0;
// 				printf("     %d: ", k_j);
// 				cout << b_j << endl;
				k_j++;
			}
			num_times[j] = k_j - 1;
// 			printf("  %d: num_times: %d, ", j, num_times[j]);
// 			cout << b_j << endl;
		}

		//build final value
		accum = b_j; //remainder
		accum *= pow(10.0, -(NUM_A_ARR - 1)); //scale remainder
// 		cout << "remainder: " << accum << endl;
		for (int j = NUM_A_ARR - 1; j >= 0; j--){ //sum in reverse order
			for (int k = 0; k < num_times[j]; k++){
				accum += ln_a_arr[j];
			}
		}
		accum = -accum;

		//calculate relative error
		mpfr_float_1000 calc, diff;
		calc = accum;
		diff = abs((actual - calc)/actual);

		if (diff > 5e-17){
			cout << x << ": ";
			cout << std::setprecision(18) << accum << ", ";
			cout << diff << endl;
		}
	}

	return 0;
}

