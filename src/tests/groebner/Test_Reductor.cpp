#include "gtest/gtest.h"
#include "carl/core/MultivariatePolynomial.h"
#include "carl/groebner/Reductor.h"

#include <cln/cln.h>

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
    Ideal<MultivariatePolynomial<cln::cl_RA>> ideal;  
    MultivariatePolynomial<cln::cl_RA> p1;
    p1 += x*x;
    p1 += z;
    MultivariatePolynomial<cln::cl_RA> p2;
    p2 += y*y;
    ideal.addGenerator(p1);
    ideal.addGenerator(p2);
    MultivariatePolynomial<cln::cl_RA> f;
    f += y;
    
    Reductor<MultivariatePolynomial<cln::cl_RA>, MultivariatePolynomial<cln::cl_RA>> reductor(ideal, f);
    MultivariatePolynomial<cln::cl_RA> fres = reductor.fullReduce();
    EXPECT_EQ(f,fres);
    
    MultivariatePolynomial<cln::cl_RA> f2;
    f2 += y*y;
    
    Reductor<MultivariatePolynomial<cln::cl_RA>, MultivariatePolynomial<cln::cl_RA>> reductor2(ideal, f2);
    fres = reductor2.fullReduce();
    EXPECT_EQ(MultivariatePolynomial<cln::cl_RA>(),fres);
    
    MultivariatePolynomial<cln::cl_RA> f3;
    f3 += x*z;
    Reductor<MultivariatePolynomial<cln::cl_RA>, MultivariatePolynomial<cln::cl_RA>> reductor3(ideal, f3);
    fres = reductor3.fullReduce();
    EXPECT_EQ(f3, fres);
    
    MultivariatePolynomial<cln::cl_RA> f4;
    f4 += x*x;
    Reductor<MultivariatePolynomial<cln::cl_RA>, MultivariatePolynomial<cln::cl_RA>> reductor4(ideal, f4);
    fres = reductor4.fullReduce();
    EXPECT_EQ((cln::cl_RA)-1 * z, fres);
    
}