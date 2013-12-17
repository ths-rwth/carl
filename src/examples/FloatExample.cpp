/**
 * This file is for short testing - not intended for distribution.
 * @file FloatExample.cpp
 * @author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 *
 * @since	2013-12-16
 * @version	2013-12-16
 */

#include "../carl/interval/rounding.h"
#include "../carl/numbers/roundingConversion.h"
#include <mpfr.h>

int main (int argc, char** argv)
{
	mpfr_ptr numberA = new mpfr_t();
	mpfr_init(numberA);
	mpfr_set_ui(numberA, 5, MPFR_RNDN);
	
	mpfr_ptr numberB = new mpfr_t;
	mpfr_init(numberB);
	mpfr_set_ui(numberB, 5, MPFR_RNDN);
	
	mpfr_ptr result;
	mpfr_init(result);
	
	carl::convRnd<mpfr_ptr> r;
	mpfr_rnd_t tmp =  r.operator() (carl::CARL_RND::CARL_RNDD);
	
	//result = carl::add_down(numberA, numberB);
	
	mpfr_clears(numberA, numberB, result);
	return 0;
}
