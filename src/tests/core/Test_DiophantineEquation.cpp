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
}




