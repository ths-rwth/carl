#include "gtest/gtest.h"
#include "carl/core/RationalFunction.h"
#include "carl/core/VariablePool.h"
#include <gmpxx.h>

using namespace carl;

typedef RationalFunction<MultivariatePolynomial<mpq_class>>  RFunc;
typedef MultivariatePolynomial<mpq_class> Pol;

TEST(RationalFunction, Construction)
{
	VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setName(x, "x");
	Variable y = vpool.getFreshVariable();
    vpool.setName(y, "y");
	
    
    RFunc r1(Pol(3), Pol(5));
    
    
}
