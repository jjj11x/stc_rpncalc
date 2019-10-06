//prototype for exponential function
// complement of ln() function based on HP Journal article
// "Personal Calculator Algorithms IV: Logarithmic Functions"
// (uses same set of constants)

#include <stdio.h>
#include <iostream>
#include <math.h>
#include <boost/multiprecision/mpfr.hpp>
using namespace boost::multiprecision;

// #define DEBUG
// #define DEBUG_ALL

using std::cout;
using std::endl;

static const int NUM_A_ARR = 9;
mpfr_float a_arr[NUM_A_ARR];
mpfr_float ln_a_arr[NUM_A_ARR];

static const unsigned int CALC_PRECISION = 18;


int main(void){
	cout << std::scientific << std::setprecision(CALC_PRECISION);
	mpfr_float ln10 (10, CALC_PRECISION);
	ln10 = log( ln10 );
	cout << "ln(10):" << ln10 << endl;
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
	}
	cout << endl << endl ;
	//loop through values to test
	for (mpfr_float x(0.0001, CALC_PRECISION); x < 230.0; x += 0.015625){
// 	for (mpfr_float x(230, CALC_PRECISION); x < 230.01; x += 0.015625){
		//build mpf values
		mpfr_float accum(x, CALC_PRECISION);
		mpfr_float save (0, CALC_PRECISION);
		mpfr_float_1000 actual;
		actual = exp(x);

		int num_times_ln10;
		int num_times[NUM_A_ARR];
		int k_j = 0;
		//track number of times 10*ln(10) subtracted
		while (accum > 0){
			save = accum;
			accum -= 10*ln10;
			k_j++;
		}
		num_times_ln10 = (k_j - 1) * 10;
		accum = save; //restore
		//track number of times ln(10) subtracted
		k_j = 0;
		while (accum > 0){
			save = accum;
			accum -= ln10;
			k_j++;
		}
		num_times_ln10 += k_j - 1;
		accum = save; //restore
#ifdef DEBUG
		printf("  10: num_times: %d, ", num_times_ln10);
		cout << accum << endl;
#endif
		//track number of times ln_a_arr[j] subtracted
		for (int j = 0; j < NUM_A_ARR; j++){
			k_j = 0;
			while (accum > 0){
				save = accum;
				accum -= ln_a_arr[j];
#ifdef DEBUG_ALL
				printf("     %d: ", k_j);
				cout << accum << endl;
#endif
				k_j++;
			}
			num_times[j] = k_j - 1;
			accum = save; //restore
#ifdef DEBUG
			printf("  %d: num_times: %d, ", j, num_times[j]);
			cout << accum << endl;
#endif
		}

		//build final value
		accum = 1+save; //initialize product to 1+remainder
		//multiplies for ln(10)
		for (int k = 0; k < num_times_ln10; k++){
			accum *= 10;
		}
		//multiplies for ln(1 + 10^-j)
		for (int j = 0; j < NUM_A_ARR; j++){
			for (int k = 0; k < num_times[j]; k++){
				accum *= a_arr[j];
			}
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
#ifdef DEBUG_ALL
			cout << actual << ", ";
#endif
			cout << diff << endl;
		}
	}

	return 0;
}

