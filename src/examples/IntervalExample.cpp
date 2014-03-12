/**
 * An example how to use the general type interval with different instantiations.
 * @file IntervalExample.cpp
 * @author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 *
 * @since	2013-12-20
 * @version 2014-01-30
 */

#include "../carl/interval/Interval.h"
#include "../carl/numbers/FLOAT_T.h"
#ifdef USE_MPFR_FLOAT
#include <mpfr.h>
#endif
#include <gmpxx.h>
#include <cln/cln.h>

int main (int argc, char** argv)
{
	carl::Interval<double> doubleInterval(2.0);
	carl::Interval<double> result;
	
	mpq_class a = 2;
	mpq_class b = 4;
	mpq_class c = 6;
	mpq_class d = 3;
	
	cln::cl_RA e = 2;
	cln::cl_RA f = 4;
	cln::cl_RA g = 6;
	cln::cl_RA h = 3;
	
	carl::Interval<mpq_class> mpqInterval( a/b, carl::BoundType::WEAK, c/d, carl::BoundType::STRICT );
	carl::Interval<cln::cl_RA> clnInterval( e/f, carl::BoundType::WEAK, g/h, carl::BoundType::STRICT );
	
	double mag = doubleInterval.magnitude();
	
	mpq_class magMpq = mpqInterval.magnitude();
	
	cln::cl_RA magCln = clnInterval.magnitude();
	
	std::cout << "Magnitudes: " << mag << " MPQ: " << mpqInterval << " -> "<< magMpq << " CLN:" << clnInterval << " -> " << magCln << std::endl;
#ifdef USE_MPFR_FLOAT
	carl::Interval<carl::FLOAT_T<mpfr_t>> mpfrInterval(carl::FLOAT_T<mpfr_t>(2.0));
	
	result = doubleInterval.log();
	
	mpfrInterval.log_assign();
	
	std::cout << "Result: " << result << " and mpfr: " << mpfrInterval << std::endl;
#endif
}