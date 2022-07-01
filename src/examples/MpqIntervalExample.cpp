/**
 * An example used to test, which of the operators for multiplication of mpq_class
 * types is used: The official one or the overridden one.
 * @file MpqIntervalExample.cpp
 * @author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 *
 * @since	2014-01-29
 * @version 2014-01-29
 */

#include <carl-arith/interval/Interval.h>
int main (int argc, char** argv)
{
	mpq_class a = 2;
	mpq_class b = 4;
	mpq_class c = 6;
	mpq_class d = 3;
	
	carl::Interval<mpq_class> mpqInterval( a*b, carl::BoundType::WEAK, c/d, carl::BoundType::STRICT );
}