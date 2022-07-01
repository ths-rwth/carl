/**
 * An example how to use the general type interval with different instantiations.
 * @file IntervalExample.cpp
 * @author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 *
 * @since	2013-12-20
 * @version 2014-01-30
 */

#include <carl-arith/interval/Interval.h>
#include <carl-arith/numbers/numbers.h>

#ifdef USE_CLN_NUMBERS
typedef cln::cl_RA Rational;
typedef cln::cl_I Integer;
#else
typedef mpq_class Rational;
typedef mpz_class Integer;
#endif

int main (int argc, char** argv)
{
	carl::Interval<double> doubleInterval(2.0);
	carl::Interval<double> result;
	
	Rational a = 2;
	Rational b = 4;
	Rational c = 6;
	Rational d = 3;
	
	Rational e = 2;
	Rational f = 4;
	Rational g = 6;
	Rational h = 3;
	
	carl::Interval<Rational> mpqInterval( a/b, carl::BoundType::WEAK, c/d, carl::BoundType::STRICT );
	carl::Interval<Rational> clnInterval( e/f, carl::BoundType::WEAK, g/h, carl::BoundType::STRICT );
	
	double mag = doubleInterval.magnitude();
	
	Rational magMpq = mpqInterval.magnitude();
	
	Rational magCln = clnInterval.magnitude();
	
	std::cout << "Magnitudes: " << mag << " MPQ: " << mpqInterval << " -> "<< magMpq << " CLN:" << clnInterval << " -> " << magCln << std::endl;
#ifdef USE_MPFR_FLOAT
	carl::Interval<carl::FLOAT_T<mpfr_t>> mpfrInterval(carl::FLOAT_T<mpfr_t>(2.0));
	
	result = doubleInterval.log();
	
	mpfrInterval.log_assign();
	
	std::cout << "Result: " << result << " and mpfr: " << mpfrInterval << std::endl;
#endif
}