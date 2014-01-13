/**
 * An example how to use the general type interval with different instantiations.
 * @file IntervalExample.cpp
 * @author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 *
 * @since	2013-12-20
 * @version 2014-01-10
 */

#include "../carl/interval/Interval.h"
#include "../carl/numbers/FLOAT_T.h"
#include <mpfr.h>

int main (int argc, char** argv)
{
	carl::Interval<double> doubleInterval(2.0);
	carl::Interval<double> result;
	
	carl::Interval<carl::FLOAT_T<mpfr_t>> mpfrInterval(carl::FLOAT_T<mpfr_t>(2.0));
	
	result = doubleInterval.log();
	
	mpfrInterval.log_assign();
	
	std::cout << "Result: " << result << " and mpfr: " << mpfrInterval << std::endl;
}