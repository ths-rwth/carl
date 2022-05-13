#include "gtest/gtest.h"
#include <carl/core/VariablePool.h>
#include <carl/poly/umvpoly/UnivariatePolynomial.h>
#include <carl/poly/umvpoly/functions/PrimitiveEuclidean.h>

using namespace carl;

TEST(PrimitiveEuclidean, Zx)
{
	Variable x = carl::fresh_integer_variable("x");
	UnivariatePolynomial<mpz_class> a(x, {-36, 42, -84, 48});
	UnivariatePolynomial<mpz_class> b(x, {-30, 44, -10, -4});
	//UnivariatePolynomial<mpz_class> result = gcd(a.normalized(), b.normalized());
	//std::cout << result << std::endl;
}
