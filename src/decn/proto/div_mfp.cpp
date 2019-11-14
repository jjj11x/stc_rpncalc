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



//prototype for reciprocal function for division based on newton-raphson iteration

#include <stdio.h>
#include <math.h>
#include <gmp.h>

static const int BSIZE = 200;
char buf[BSIZE];
char pbuf[BSIZE];
static const int PRECISION = 1000;


int main(void){
	static const int MULT = 1000;
	for (int x = 1 * MULT; x < 10 * MULT; x++){
		//build number
		sprintf(pbuf, "%d", x);
		//copy with decimal point
		if (pbuf[0]){
			buf[0] = pbuf[0];
		} else {
			buf[0] = '0';
		}
		buf[1] = '.';
		int b_i = 2;
		for (int pb_i = 1; pbuf[pb_i] != '\0' && b_i < BSIZE; b_i++, pb_i++){
			buf[b_i] = pbuf[pb_i];
		}
		//null terminate
		if (b_i < BSIZE){
			buf[b_i] = '\0';
		} else {
			buf[BSIZE-1] = '\0';
		}
		//build mpf
		mpf_t x_mpf, accum, recip, actual;
		static const int CALC_PRECISION = 18;
		mpf_init2(x_mpf, CALC_PRECISION); //copy of original x
		mpf_init2(accum, CALC_PRECISION); //working register
		mpf_init2(recip, CALC_PRECISION); //current estimate of reciprocal
		mpf_init2(actual, PRECISION); //truth value for 1/x
		mpf_set_str(x_mpf, buf, 10);
		mpf_set(actual, x_mpf);
		mpf_ui_div(actual, 1, actual); //actual = 1/x
		//get estimate
		if (x <= 2.0 * MULT) {
			sprintf(buf, "%de%d", 50, -2);
		} else if (x < 3.3 * MULT) {
			sprintf(buf, "%de%d", 30, -2);
		} else if (x <= 5.0 * MULT) {
			sprintf(buf, "%de%d", 20, -2);
		} else {
			sprintf(buf, "%de%d", 1, -1);
		}
		mpf_set_str(recip, buf, 10); //recip = estimate
		mpf_set(accum, recip);       //accum = estimate

		//do division
		for (int i = 0; i < 6; i++){ //fixed number of iterations
			mpf_mul(accum, accum, x_mpf); //accum *= x;
			mpf_neg(accum, accum); //accum *= -1;
			mpf_add_ui(accum, accum, 1); //accum += 1;
			mpf_mul(accum, accum, recip); //accum *= recip;
			mpf_add(accum, accum, recip);//accum += recip;
			//recip = recip + (1 - recip*x)*recip;
			mpf_set(recip, accum); //recip = accum;
		}
		//calculate error
		mpf_t calc, diff;
		mpf_init2(calc, PRECISION);
		mpf_init2(diff, PRECISION);
		int cmp;
		mpf_set(calc, accum);        //calc = accum
		mpf_reldiff(diff, actual, calc); //diff = (actual - calc)/actual
		cmp = mpf_cmp_d(diff, 4.5e-20);
		mp_exp_t exponent;
		mpf_get_str(pbuf, &exponent, 10, 20, accum);
		mpf_get_str(buf, &exponent, 10, 20, diff);
		//add decimal point
		exponent--;
		char tmp = buf[1];
		buf[1] = '.';
		for (int i = 2; i <= 5; i++){
			char tmp2 = buf[i];
			buf[i] = tmp;
			tmp = tmp2;
		}
		buf[6] = '\0';
		//print if out of tolerance
		if (cmp > 0){
			printf("%5d: %20s, %s(%ld)\n",
			   	    x, pbuf, buf, exponent);
		}
	}

	return 0;
}

