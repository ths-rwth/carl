#include "gtest/gtest.h"
#include "carl/core/MultivariateGCD.h"
#include "carl/core/PrimitiveEuclideanAlgorithm.h"
#include "carl/util/platform.h"

#ifdef __WIN
	#pragma warning(push, 0)
	#include <mpirxx.h>
	#pragma warning(pop)
#else
	#include <gmpxx.h>
#endif

#include "../Common.h"

using namespace carl;

typedef mpq_class Rational;

TEST(MultivariateGCD, test1)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setName(x, "x");
    Variable y = vpool.getFreshVariable();
    vpool.setName(y, "y");
    Variable z = vpool.getFreshVariable();
    vpool.setName(z, "z");
    typedef MultivariatePolynomial<Rational> P;
    P f1({(Rational)1*x*x*x*y*y, (Rational)-1*x*x*y*y*y, (Rational)1*x*y});
    P g1({(Rational)1*x*x*x*x*y, (Rational)3*x*y*y});

    MultivariateGCD<PrimitiveEuclidean, Rational> gcd(f1,g1);
    std::cout << gcd.calculate() << std::endl;

    P fxy({(Rational)1*x*y});
    P fz({(Rational)1*z});
    MultivariateGCD<PrimitiveEuclidean, Rational> gcd2(fxy, fz);
    std::cout << gcd2.calculate() << std::endl;

//    UnivariatePolynomial<mpz_class> A1(x, {(mpz_class)1, (mpz_class)1, (mpz_class)0, (mpz_class)1});
//    const GaloisField<mpz_class>* gf5 = new GaloisField<mpz_class>(5);
//    UnivariatePolynomial<GFNumber<mpz_class>> a1 = A1.toFiniteDomain(gf5);
//    UnivariatePolynomial<mpz_class> A2(x, {(mpz_class)3, (mpz_class)2, (mpz_class)0, (mpz_class)1});
//    UnivariatePolynomial<GFNumber<mpz_class>> a2 = A2.toFiniteDomain(gf5);
//    DiophantineEquations<mpz_class> d(5,1);

//    std::vector<UnivariatePolynomial<GFNumber<mpz_class>>> result = d.univariateDiophantine({a1,a2}, x, 4);
//    EXPECT_EQ((unsigned)2,result.size());
//    std::cout << result.front() << std::endl;
//    std::cout << result.back() << std::endl;
}
