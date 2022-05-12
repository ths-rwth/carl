#include <gtest/gtest.h>
#include <carl/poly/umvpoly/functions/GCD.h>
#include <carl/numbers/numbers.h>
#include <carl-common/meta/platform.h>

#include "../Common.h"

using namespace carl;

typedef mpq_class Rational;

TEST(GCD, constants) {
	using P = MultivariatePolynomial<Rational>;
	Variable x = freshRealVariable("x");
	EXPECT_EQ(P(1), carl::gcd(P(1), P(2)));
	EXPECT_EQ(P(1), carl::gcd(P(2), P(1)));
	EXPECT_EQ(P(1), carl::gcd(P(1), P(x)));
	EXPECT_EQ(P(1), carl::gcd(P(x), P(1)));
	EXPECT_EQ(P(2), carl::gcd(P(10), P(6)));
	EXPECT_EQ(P(2), carl::gcd(P(6), P(10)));
	EXPECT_EQ(P(1), carl::gcd(P(10), P(x)));
	EXPECT_EQ(P(1), carl::gcd(P(x), P(10)));
}

TEST(MultivariateGCD, test1)
{
    Variable x = freshRealVariable("x");
    Variable y = freshRealVariable("y");
    Variable z = freshRealVariable("z");
    typedef MultivariatePolynomial<Rational> P;
    P f1({(Rational)1*x*x*x*y*y, (Rational)-1*x*x*y*y*y, (Rational)1*x*y});
    P g1({(Rational)1*x*x*x*x*y, (Rational)3*x*y*y});

    std::cout << carl::gcd(f1, g1) << std::endl;

    P fxy({(Rational)1*x*y});
    P fz({(Rational)1*z});
    std::cout << carl::gcd(fxy, fz) << std::endl;

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
    
    P h1({(Rational)1*x*y});
    P h2({(Rational)1*y});
    EXPECT_EQ( carl::gcd( h1, h2 ), h2 );
}
