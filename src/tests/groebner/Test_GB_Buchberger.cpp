#include "gtest/gtest.h"
#include <carl/groebner/GBProcedure.h>

#include <carl/groebner/Ideal.h>
#include <carl/groebner/groebner.h>
#include <carl-common/meta/platform.h>

#include "../Common.h"


using namespace carl;

template<typename Coeff>
using PolynomialWithReasonSet = MultivariatePolynomial<Coeff, GrLexOrdering, StdMultivariatePolynomialPolicies<BVReasons, NoAllocator>>;


TEST(GB_Buchberger, T1)
{
	Variable x = fresh_real_variable("x");
	Variable y = fresh_real_variable("y");

    MultivariatePolynomial<Rational> f1({(Rational)1*x*x*x, (Rational)-2*x*y} );
    MultivariatePolynomial<Rational> f2({(Rational)1*x*x*y, (Rational)-2*y*y, (Rational)1*x});
    MultivariatePolynomial<Rational> F1({(Rational)1*x*x} );
    MultivariatePolynomial<Rational> F2({(Rational)1*y*y, (Rational)-1*(Rational)1/(Rational)2*x} );
    MultivariatePolynomial<Rational> F3({(Rational)1*x*y} );
    GBProcedure<MultivariatePolynomial<Rational>, Buchberger, StdAdding> gbobject;
    EXPECT_TRUE(gbobject.inputEmpty());
    gbobject.addPolynomial(f1);
    gbobject.addPolynomial(f2);
    gbobject.reduceInput();
    EXPECT_FALSE(gbobject.inputEmpty());
    gbobject.calculate();
    EXPECT_EQ(F1,gbobject.getIdeal().getGenerator(0));
    EXPECT_EQ(F3,gbobject.getIdeal().getGenerator(1));
    EXPECT_EQ(F2,gbobject.getIdeal().getGenerator(2));
    GBProcedure<MultivariatePolynomial<Rational>, Buchberger, RealRadicalAwareAdding> gb2object;
    EXPECT_TRUE(gb2object.inputEmpty());
    gb2object.addPolynomial(f1);
    gb2object.addPolynomial(f2);
    EXPECT_FALSE(gb2object.inputEmpty());
    gb2object.calculate();
    EXPECT_EQ(x,gb2object.getIdeal().getGenerator(0));
    EXPECT_EQ(y,gb2object.getIdeal().getGenerator(1));
}

TEST(GB_Buchberger, T1_ReasonSets)
{
	Variable x = fresh_real_variable("x");
	Variable y = fresh_real_variable("y");

    MultivariatePolynomial<Rational> f1({(Rational)1*x*x*x, (Rational)-2*x*y} );
    PolynomialWithReasonSet<Rational> f1rs(f1);
    MultivariatePolynomial<Rational> f2({(Rational)1*x*x*y, (Rational)-2*y*y, (Rational)1*x});
    PolynomialWithReasonSet<Rational> f2rs(f2);
    
    PolynomialWithReasonSet<Rational> F1({ (Rational)1 * x*x });
    PolynomialWithReasonSet<Rational> F2({ (Rational)1 * y*y, (Rational)-1 * (Rational)1 / (Rational)2 * x });
    PolynomialWithReasonSet<Rational> F3({ (Rational)1 * x*y });
    GBProcedure<PolynomialWithReasonSet<Rational>, Buchberger, StdAdding> gbobject;
    EXPECT_TRUE(gbobject.inputEmpty());
    gbobject.addPolynomial(f1rs);
    gbobject.addPolynomial(f2rs);
    gbobject.reduceInput();
    EXPECT_FALSE(gbobject.inputEmpty());
    gbobject.calculate();
    EXPECT_EQ(F1,gbobject.getIdeal().getGenerator(0));
    EXPECT_EQ(F3,gbobject.getIdeal().getGenerator(1));
    EXPECT_EQ(F2,gbobject.getIdeal().getGenerator(2));
    GBProcedure<PolynomialWithReasonSet<Rational>, Buchberger, RealRadicalAwareAdding> gb2object;
    EXPECT_TRUE(gb2object.inputEmpty());
    gb2object.addPolynomial(f1rs);
    gb2object.addPolynomial(f2rs);
    EXPECT_FALSE(gb2object.inputEmpty());
    gb2object.calculate();
    EXPECT_EQ(x,gb2object.getIdeal().getGenerator(0));
    EXPECT_EQ(y,gb2object.getIdeal().getGenerator(1));
}
