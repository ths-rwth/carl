#include "gtest/gtest.h"
#include "../numbers/config.h"
#include "carl/core/MultivariatePolynomial.h"
#include "carl/core/UnivariatePolynomial.h"
#include "carl/core/VariablePool.h"
#include "carl/interval/Interval.h"
#include <cln/cln.h>
#include <gmpxx.h>
#include "Util.cpp"
#include <list>
#include "carl/converter/OldGinacConverter.h"
#include "carl/util/stringparser.h"
#include "carl/core/DiophantineEquation.h"


using namespace carl;



TEST(DiophantineEquation, solve) {
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    Variable y = vpool.getFreshVariable();
    Variable z = vpool.getFreshVariable();
    
    MultivariatePolynomial<cln::cl_I> p({(cln::cl_I)-4*x, (cln::cl_I)4*y, (cln::cl_I)12*z, (Term<cln::cl_I>)((cln::cl_I)8)});
    
}


TEST(DiophantineEquation, extended_gcd_integer) {
    cln::cl_I s, t, a, b;
    cln::cl_I gcd;
    
    a = 4077; b = 4077;
    gcd = extended_gcd_integer(a,b,s,t);
    std::cout << s << "*" << a << " + " << t << "*" << b << " = " << gcd << std::endl;
    EXPECT_EQ(s*a + t*b, gcd);
    
    a = 12; b = 8;
    gcd = extended_gcd_integer(a, b, s, t);
    std::cout << s << "*" << a << " + " << t << "*" << b << " = " << gcd << std::endl;
    EXPECT_EQ(s*a + t*b, gcd);
    
    a = -11; b = -11;
    gcd = extended_gcd_integer(a, b, s, t);
    std::cout << s << "*" << a << " + " << t << "*" << b << " = " << gcd << std::endl;
    EXPECT_EQ(s*a + t*b, gcd);
    
    a = -77261; b = -884;
    gcd = extended_gcd_integer(a, b, s, t);
    std::cout << s << "*" << a << " + " << t << "*" << b << " = " << gcd << std::endl;
    EXPECT_EQ(s*a + t*b, gcd);
    
    a = 163; b = -9;
    gcd = extended_gcd_integer(a, b, s, t);
    std::cout << s << "*" << a << " + " << t << "*" << b << " = " << gcd << std::endl;
    EXPECT_EQ(s*a + t*b, gcd);
    
    a = -33; b = 33;
    gcd = extended_gcd_integer(a, b, s, t);
    std::cout << s << "*" << a << " + " << t << "*" << b << " = " << gcd << std::endl;
    EXPECT_EQ(s*a + t*b, gcd);
    
    a = 0; b = 5;
    gcd = extended_gcd_integer(a, b, s, t);
    std::cout << s << "*" << a << " + " << t << "*" << b << " = " << gcd << std::endl;
    EXPECT_EQ(s*a + t*b, gcd);
    
    a = -17; b = 0;
    gcd = extended_gcd_integer(a, b, s, t);
    std::cout << s << "*" << a << " + " << t << "*" << b << " = " << gcd << std::endl;
    EXPECT_EQ(s*a + t*b, gcd);
    
    a = 0; b = 0;
    gcd = extended_gcd_integer(a, b, s, t);
    std::cout << s << "*" << a << " + " << t << "*" << b << " = " << gcd << std::endl;
    EXPECT_EQ(s*a + t*b, gcd);
}

TEST(DiophantineEquation, solve_linear) {
    VariablePool& vpool = VariablePool::getInstance();
    vpool.clear();
    Variable x = vpool.getFreshVariable();
    Variable y = vpool.getFreshVariable();
    Variable z = vpool.getFreshVariable();
    
    MultivariatePolynomial<cln::cl_I> p1({(cln::cl_I)6*x, (cln::cl_I)4*y, (cln::cl_I)6*z, (Term<cln::cl_I>)((cln::cl_I)8)});
    MultivariatePolynomial<cln::cl_I> p2({(cln::cl_I)4*x, (cln::cl_I)2*y, (cln::cl_I)3*z, (Term<cln::cl_I>)((cln::cl_I)7)});
    
    std::vector<cln::cl_I> res;
    res = solveDiophantine(p1);
    
    std::cout << p1 << std::endl;
    std::cout << res << std::endl;
    
    res = solveDiophantine(p2);
    std::cout << p2 << std::endl;
    std::cout << res << std::endl;
    
}




