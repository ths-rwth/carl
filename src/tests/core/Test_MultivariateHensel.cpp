#include <gtest/gtest.h>
#include "carl/core/MultivariateHensel.h"
#include "carl/core/VariablePool.h"
#include <carl-common/meta/platform.h>

#include <carl/numbers/numbers.h>

using namespace carl;
/*
TEST(Diophantine, Constructor)
{
	VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setName(x, "x");
    UnivariatePolynomial<mpz_class> A1(x, {(mpz_class)1, (mpz_class)1, (mpz_class)0, (mpz_class)1});
	const GaloisField<mpz_class>* gf5 = new GaloisField<mpz_class>(5);
    UnivariatePolynomial<GFNumber<mpz_class>> a1 = A1.toFiniteDomain(gf5);
	UnivariatePolynomial<mpz_class> A2(x, {(mpz_class)3, (mpz_class)2, (mpz_class)0, (mpz_class)1});
	UnivariatePolynomial<GFNumber<mpz_class>> a2 = A2.toFiniteDomain(gf5);
    DiophantineEquations<mpz_class> d(5,1);
	
	std::vector<UnivariatePolynomial<GFNumber<mpz_class>>> result = d.univariateDiophantine({a1,a2}, x, 4);
	EXPECT_EQ((unsigned)2,result.size());
	std::cout << result.front() << std::endl;
	std::cout << result.back() << std::endl;
}
*/
