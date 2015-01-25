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

TEST(DiophantineEquation, test) {
    
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    Variable y = vpool.getFreshVariable();
    Variable z = vpool.getFreshVariable();
    
    MultivariatePolynomial<cln::cl_I> p({(cln::cl_I)1*x, (cln::cl_I)3*y, (cln::cl_I)-2*z, 
            (Term<cln::cl_I>)((cln::cl_I)-7), (cln::cl_I)-10*x});
    std::cout << p << std::endl;
    
    p.makeOrdered();
    std::cout << p << std::endl;
    
    std::cout<< p.isLinear() << std::endl;
    
    std::cout << p.getTerms() << std::endl;
    
    cln::cl_I gcd = gcdOfLinearCoefficients(p);
    
    std::cout << gcd << std::endl;
}


TEST(DiophantineEquation, solve) {
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    Variable y = vpool.getFreshVariable();
    Variable z = vpool.getFreshVariable();
    
    MultivariatePolynomial<cln::cl_I> p({(cln::cl_I)-4*x, (cln::cl_I)4*y, (cln::cl_I)12*z, (Term<cln::cl_I>)((cln::cl_I)8)});
    
}


TEST(DiophantineEquation, extended_gcd) {
    cln::cl_I s;
    cln::cl_I t;
    
    cln::cl_I a = 125400000465466474;
    cln::cl_I b = -288842288409876994;
    
    cln::cl_I gcd;
    gcd = extended_gcd(a,b,s,t);
    
    std::cout << "s = " << s << ", t = " << t << " , gcd = " << gcd << std::endl;
    
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
    res = solveLinearDiophantine(p1);
    
    std::cout << p1 << std::endl;
    std::cout << res << std::endl;
    
    res = solveLinearDiophantine(p2);
    std::cout << p2 << std::endl;
    std::cout << res << std::endl;
    
}




