/**
 * An example how to use the general type interval with different instantiations.
 * @file IntervalExample.cpp
 * @author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 *
 * @since	2013-12-20
 * @version 2014-01-07
 */

#include "../carl/interval/Interval.h"

int main (int argc, char** argv)
{
	carl::Interval<double> doubleInterval(2.0);
	carl::Interval<double> result;
	
	result = doubleInterval.log();
	
	result.log_assign();
}