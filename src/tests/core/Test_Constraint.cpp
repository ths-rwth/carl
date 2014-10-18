#include "gtest/gtest.h"
#include "carl/core/ConstraintOperations.h"
#include "carl/util/stringparser.h"

#include "carl/core/MultivariatePolynomial.h"

#include <unordered_set>

typedef mpq_class Rational;


typedef carl::MultivariatePolynomial<Rational> Pol;
typedef carl::Constraint<Pol> PolCon;
typedef carl::RationalFunction<Pol> RFunc;
typedef carl::Constraint<RFunc> RFuncCon;



TEST(Constraint, Operations)
{
    using carl::CompareRelation;
    carl::StringParser sp;
    sp.setVariables({"x", "y", "z"});
    
    std::vector<RFuncCon> v1;
    
    Pol p1 = sp.parseMultivariatePolynomial<Rational>("3*x");
    Pol p2 = sp.parseMultivariatePolynomial<Rational>("1");
    RFunc r1(p1,p2);
    v1.push_back(RFuncCon(r1, CompareRelation::GEQ));
    std::unordered_set<PolCon> s1;
    carl::constraints::toPolynomialConstraints<Pol, false>(v1.begin(), v1.end(), inserter(s1, s1.end()));
    EXPECT_EQ((size_t)1, s1.size());
    v1.clear();
    s1.clear();
    
    
    Pol p3 = sp.parseMultivariatePolynomial<Rational>("z");
    RFunc r2(p1,p3);
    v1.push_back(RFuncCon(r2, CompareRelation::GEQ));
    carl::constraints::toPolynomialConstraints<Pol, false>(v1.begin(), v1.end(), inserter(s1, s1.end()));
    EXPECT_EQ((size_t)2, s1.size());
    v1.clear();
    s1.clear();
    
    Pol p4 = sp.parseMultivariatePolynomial<Rational>("-2");
    RFunc r3(p4);
    v1.push_back(RFuncCon(r3, CompareRelation::GEQ));
    carl::constraints::toPolynomialConstraints<Pol, false>(v1.begin(), v1.end(), inserter(s1, s1.end()));
    EXPECT_EQ((size_t)1, s1.size());
    v1.clear();
    s1.clear();
    
    
    
    
    
    
   
}


