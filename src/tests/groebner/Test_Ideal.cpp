#include "gtest/gtest.h"
#include "carl/groebner/Reductor.h"

#include "carl/core/Monomial.h"
#include "carl/core/MultivariatePolynomial.h"

#include "carl/groebner/Ideal.h"

#include <cln/cln.h>

using namespace carl;

TEST(Ideal, Construction)
{  
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    Variable y = vpool.getFreshVariable();
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
    ideal.print();
}



