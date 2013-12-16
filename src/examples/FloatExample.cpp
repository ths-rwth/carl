/**
 * This file is for short testing - not intended for distribution.
 * @file FloatExample.cpp
 * @author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 *
 * @since	2013-12-16
 * @version	2013-12-16
 */

#include "../carl/interval/rounding.h"
#include <mpfr.h>

int main (int argc, char** argv)
{
	mpfr_t number;
	mpfr_init(number);
	//mpfr_set_ui(number, 5, MPFR_RNDN);
	
	mpfr_clear(number);
	return 0;
}
