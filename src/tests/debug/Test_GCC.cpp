#include "gtest/gtest.h"

#include <list>
#include <cln/cln.h>
#include <map>

#include "carl/core/logging.h"
#include "carl/core/RealAlgebraicNumber.h"
#include "carl/core/rootfinder/RootFinder.h"

using namespace carl;

TEST(GCC, BasicOperations)
{
	Variable x = VariablePool::getInstance().getFreshVariable();
	
	UnivariatePolynomial<MultivariatePolynomial<cln::cl_RA>> p(x);
	std::map<Variable, RealAlgebraicNumber<cln::cl_RA>*> map;
	ExactInterval<cln::cl_RA> interval;
	rootfinder::SplittingStrategy strategy = rootfinder::SplittingStrategy::DEFAULT;
	
	rootfinder::realRoots(p, map, interval, strategy);
}
