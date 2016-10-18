#include "gtest/gtest.h"

#include "carl/core/VariablePool.h"
#include "carl/io/SMTLIBStream.h"

#include "../Common.h"


TEST(SMTLIBStream, Base)
{

	carl::Variable x = carl::freshRealVariable("x");
	carl::SMTLIBStream sls;
	//sls << x;
	
	auto mon = x*x*x;
	
	Rational r = 4;
	
	carl::UnivariatePolynomial<Rational> up(x, {r*r*r, r*r, r});
	carl::MultivariatePolynomial<Rational> mp = Rational(r*r)*x*x + r*x + r;

	sls << mp << std::endl;
	std::cout << sls << std::endl;
	std::cout << mp << std::endl;


}
