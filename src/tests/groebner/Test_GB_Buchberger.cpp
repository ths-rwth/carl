#include "gtest/gtest.h"
#include "carl/groebner/GBProcedure.h"

#include "carl/core/MultivariatePolynomial.h"

#include "carl/groebner/Ideal.h"
#include "carl/groebner/groebner.h"

#include <cln/cln.h>

using namespace carl;

template<typename Coeff>
using PolynomialWithReasonSet = MultivariatePolynomial<Coeff, GrLexOrdering, StdMultivariatePolynomialPolicies<BVReasons, NoAllocator>>;
    

TEST(GB_Buchberger, T1)
{  
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setVariableName(x, "x");
    Variable y = vpool.getFreshVariable();
    vpool.setVariableName(y, "y");
//    Variable z = vpool.getFreshVariable();
    
    MultivariatePolynomial<cln::cl_RA> f1({(cln::cl_RA)1*x*x*x, (cln::cl_RA)-2*x*y} );
    MultivariatePolynomial<cln::cl_RA> f2({(cln::cl_RA)1*x*x*y, (cln::cl_RA)-2*y*y, (cln::cl_RA)1*x});
    
    MultivariatePolynomial<cln::cl_RA> F1({(cln::cl_RA)1*x*x} );
    MultivariatePolynomial<cln::cl_RA> F2({(cln::cl_RA)1*y*y, (cln::cl_RA)-1*(cln::cl_RA)1/2*x} );
    MultivariatePolynomial<cln::cl_RA> F3({(cln::cl_RA)1*x*y} );
    GBProcedure<MultivariatePolynomial<cln::cl_RA>, Buchberger, StdAdding> gbobject;
    EXPECT_TRUE(gbobject.inputEmpty());
    gbobject.addPolynomial(f1);
    gbobject.addPolynomial(f2);
    gbobject.reduceInput();
    EXPECT_FALSE(gbobject.inputEmpty());
    gbobject.calculate();
    EXPECT_EQ(F2,gbobject.getIdeal().getGenerator(0));
    EXPECT_EQ(F3,gbobject.getIdeal().getGenerator(1));
    EXPECT_EQ(F1,gbobject.getIdeal().getGenerator(2));
    GBProcedure<MultivariatePolynomial<cln::cl_RA>, Buchberger, RealRadicalAwareAdding> gb2object;
    EXPECT_TRUE(gb2object.inputEmpty());
    gb2object.addPolynomial(f1);
    gb2object.addPolynomial(f2);
    EXPECT_FALSE(gb2object.inputEmpty());
    gb2object.calculate();
    EXPECT_EQ(y,gb2object.getIdeal().getGenerator(0));
    EXPECT_EQ(x,gb2object.getIdeal().getGenerator(1));
}

TEST(GB_Buchberger, T1_ReasonSets)
{  
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setVariableName(x, "x");
    Variable y = vpool.getFreshVariable();
    vpool.setVariableName(y, "y");
//    Variable z = vpool.getFreshVariable();
    
    MultivariatePolynomial<cln::cl_RA> f1({(cln::cl_RA)1*x*x*x, (cln::cl_RA)-2*x*y} );
    PolynomialWithReasonSet<cln::cl_RA> f1rs(f1);
    MultivariatePolynomial<cln::cl_RA> f2({(cln::cl_RA)1*x*x*y, (cln::cl_RA)-2*y*y, (cln::cl_RA)1*x});
    PolynomialWithReasonSet<cln::cl_RA> f2rs(f2);
    
    PolynomialWithReasonSet<cln::cl_RA> F1({(cln::cl_RA)1*x*x} );
    PolynomialWithReasonSet<cln::cl_RA> F2({(cln::cl_RA)1*y*y, (cln::cl_RA)-1*(cln::cl_RA)1/2*x} );
    PolynomialWithReasonSet<cln::cl_RA> F3({(cln::cl_RA)1*x*y} );
    GBProcedure<PolynomialWithReasonSet<cln::cl_RA>, Buchberger, StdAdding> gbobject;
    EXPECT_TRUE(gbobject.inputEmpty());
    gbobject.addPolynomial(f1rs);
    gbobject.addPolynomial(f2rs);
    gbobject.reduceInput();
    EXPECT_FALSE(gbobject.inputEmpty());
    gbobject.calculate();
    EXPECT_EQ(F2,gbobject.getIdeal().getGenerator(0));
    EXPECT_EQ(F3,gbobject.getIdeal().getGenerator(1));
    EXPECT_EQ(F1,gbobject.getIdeal().getGenerator(2));
    GBProcedure<PolynomialWithReasonSet<cln::cl_RA>, Buchberger, RealRadicalAwareAdding> gb2object;
    EXPECT_TRUE(gb2object.inputEmpty());
    gb2object.addPolynomial(f1rs);
    gb2object.addPolynomial(f2rs);
    EXPECT_FALSE(gb2object.inputEmpty());
    gb2object.calculate();
    EXPECT_EQ(y,gb2object.getIdeal().getGenerator(0));
    EXPECT_EQ(x,gb2object.getIdeal().getGenerator(1));
}


