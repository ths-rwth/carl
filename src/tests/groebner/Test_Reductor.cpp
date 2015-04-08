#include "gtest/gtest.h"
#include "carl/core/MultivariatePolynomial.h"
#include "carl/groebner/Reductor.h"

#ifdef USE_CLN_NUMBERS
#include <cln/cln.h>
typedef cln::cl_RA Rational;
typedef cln::cl_I Integer;
#else
#include <gmpxx.h>
typedef mpq_class Rational;
typedef mpz_class Integer;
#endif

using namespace carl;

TEST(Reductor, Constructor)
{
    
}

TEST(Reductor, Reduction)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setName(x, "x");
    Variable y = vpool.getFreshVariable();
    vpool.setName(y, "y");
    Variable z = vpool.getFreshVariable();
    vpool.setName(z, "z");
    Ideal<MultivariatePolynomial<Rational>> ideal;  
    MultivariatePolynomial<Rational> p1;
    p1 += x*x;
    p1 += z;
    MultivariatePolynomial<Rational> p2;
    p2 += y*y;
    ideal.addGenerator(p1);
    ideal.addGenerator(p2);
    MultivariatePolynomial<Rational> f;
    f += y;
    
    Reductor<MultivariatePolynomial<Rational>, MultivariatePolynomial<Rational>> reductor(ideal, f);
    MultivariatePolynomial<Rational> fres = reductor.fullReduce();
    EXPECT_EQ(f,fres);
    
    MultivariatePolynomial<Rational> f2;
    f2 += y*y;
    
    Reductor<MultivariatePolynomial<Rational>, MultivariatePolynomial<Rational>> reductor2(ideal, f2);
    fres = reductor2.fullReduce();
    EXPECT_EQ(MultivariatePolynomial<Rational>(),fres);
    
    MultivariatePolynomial<Rational> f3;
    f3 += x*z;
    Reductor<MultivariatePolynomial<Rational>, MultivariatePolynomial<Rational>> reductor3(ideal, f3);
    fres = reductor3.fullReduce();
    EXPECT_EQ(f3, fres);
    
    MultivariatePolynomial<Rational> f4;
    f4 += x*x;
    Reductor<MultivariatePolynomial<Rational>, MultivariatePolynomial<Rational>> reductor4(ideal, f4);
    fres = reductor4.fullReduce();
    EXPECT_EQ((Rational)-1 * z, fres);
    
}