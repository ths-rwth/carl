#include "gtest/gtest.h"

#include "carl/core/VariablePool.h"
#include "carl/util/stringparser.h"
#include "carl/groebner/groebner.h"
#include "carl/thom/TarskiQuery/GroebnerBaseOps.tpp"
#include "carl/thom/TarskiQuery/MultiplicationTable.h"
#include "carl/thom/TarskiQuery/Trace.h"
#include "carl/thom/TarskiQuery/MultivariateTarskiQuery.h"
#include "carl/thom/TarskiQuery/CharPol.h"

#include "../Common.h"

#include <algorithm>

using namespace carl;

class TarskiQueryTest : public ::testing::Test {
    
protected:
    
    StringParser sp;
        
    MultivariatePolynomial<Rational> circle;
    MultivariatePolynomial<Rational> ellipse;
    _Monomial<Rational> mon_0;
    _Monomial<Rational> mon_1;
    _Monomial<Rational> mon_2;
    _Monomial<Rational> mon_3;
    
    std::vector<MultivariatePolynomial<Rational>> gb;
    
        
    virtual void SetUp() {
        sp.setVariables({"x", "y"});
        circle = sp.parseMultivariatePolynomial<Rational>("x^2 + y^2 + -1");
        std::cout << "circle = " << circle << std::endl;
        ellipse = sp.parseMultivariatePolynomial<Rational>("x^2 + 4*y^2 + x*y + -2");
        std::cout << "ellipse = " << ellipse << std::endl;
        mon_0 = sp.parseTerm<Rational>("1");
        mon_1 = sp.parseTerm<Rational>("x");
        mon_2 = sp.parseTerm<Rational>("y");
        mon_3 = sp.parseTerm<Rational>("x^2");
        
        GBProcedure<MultivariatePolynomial<Rational>, Buchberger, StdAdding> gbobject;
        gbobject.addPolynomial(circle);
        gbobject.addPolynomial(ellipse);
        gbobject.reduceInput();
        gbobject.calculate();
        gb = gbobject.getIdeal().getGenerators();
        
        std::cout << "groebner base is: " << gb << std::endl;
    }
    
    // virtual void TearDown() {}
    
};

class TarskiQueryTest2 : public ::testing::Test {
    
protected:
        
    MultivariatePolynomial<Rational> lines;
    MultivariatePolynomial<Rational> curve;
    
    StringParser sp;
    
    std::vector<MultivariatePolynomial<Rational>> gb;
        
    virtual void SetUp() {
        sp.setVariables({"x", "y"});
        lines = sp.parseMultivariatePolynomial<Rational>("x^2 + -2");
        std::cout << "lines = " << lines << std::endl;
        curve = sp.parseMultivariatePolynomial<Rational>("y^3*x + -4*y + -1*x");
        std::cout << "curve = " << curve << std::endl;
        
        GBProcedure<MultivariatePolynomial<Rational>, Buchberger, StdAdding> gbobject;
        gbobject.addPolynomial(lines);
        gbobject.addPolynomial(curve);
        gbobject.reduceInput();
        gbobject.calculate();
        gb = gbobject.getIdeal().getGenerators();
        
        std::cout << "groebner base is: " << gb << std::endl;
        std::cout << std::endl;
    }
    
    // virtual void TearDown() {}
    
};




TEST_F(TarskiQueryTest, Cor) {   
    std::cout << "cor = " << cor(gb) << std::endl;
}

TEST_F(TarskiQueryTest, Mon) { 
    ASSERT_TRUE(hasFiniteMon(gb));
    std::vector<_Monomial<Rational>> Mon = mon(gb);
    std::sort(Mon.begin(), Mon.end());
    std::cout << "Mon = " << Mon << std::endl;
    EXPECT_EQ(Mon.size(), 4);
    EXPECT_EQ(Mon[3], mon_3);
    EXPECT_EQ(Mon[2], mon_2);
    EXPECT_EQ(Mon[1], mon_1);
    EXPECT_EQ(Mon[0], mon_0);
}

TEST_F(TarskiQueryTest, bor) {
    std::cout << "bor = " << bor(gb) << std::endl;
}



TEST_F(TarskiQueryTest, MultiplicationTable) {
   
    MultiplicationTable<Rational> tab;
    tab.init(gb);
    std::cout << tab << std::endl;
    
    std::cout << "----------------------------" << std::endl;
    
    // test for multiply   
    _Monomial<Rational> mon1 = sp.parseTerm<Rational>("x^4");
    _Monomial<Rational> mon2 = sp.parseTerm<Rational>("x^2*y");
    std::cout << "representation of x^4: " << tab.get(mon1) << std::endl;
    std::cout << "representation of x^2*y: " << tab.get(mon2) << std::endl;
    
    multiply(tab.get(mon1), tab.get(mon2), tab);
    
    
}

TEST_F(TarskiQueryTest, NormalForm) {
    MultivariatePolynomial<Rational> mon = sp.parseMultivariatePolynomial<Rational>("x^4");
    normalForm(mon, gb);
    
    std::cout << std::endl;
    
    _Monomial<Rational> mon1 = sp.parseTerm<Rational>("x^4");
    _Monomial<Rational> mon2 = sp.parseTerm<Rational>("x^2*y");
    normalForm(MultivariatePolynomial<Rational>(mon2 * mon1), gb);
    
    std::cout << std::endl;
    
    normalForm(circle, gb);
}

TEST_F(TarskiQueryTest, Init2) {
    MultiplicationTable<Rational> tab;
    tab.init2(gb);
    std::cout << tab << std::endl;
}

TEST_F(TarskiQueryTest, Trace) {
    MultiplicationTable<Rational> tab;
    tab.init2(gb);
    
    std::cout << tab << std::endl;
    
    // a random polynomial
    MultivariatePolynomial<Rational> p = sp.parseMultivariatePolynomial<Rational>("x^4 + y^3");
    std::cout << trace(normalForm(p, gb), tab) << std::endl;
}

TEST_F(TarskiQueryTest, MultivariateTarskiQuery) {
    MultiplicationTable<Rational> tab;
    tab.init2(gb);
    
    MultivariatePolynomial<Rational> Q = sp.parseMultivariatePolynomial<Rational>("x^2 + -1*y^2 + -2");
    int q = multivariateTarskiQuery(Q, tab, gb);
    std::cout << "tarski query: " << q << std::endl;
    
    
}

TEST_F(TarskiQueryTest2, MultivariateTarskiQuery) {
    MultiplicationTable<Rational> tab;
    tab.init2(gb);
    
    std::cout << tab << std::endl;
    
    MultivariatePolynomial<Rational> Q = sp.parseMultivariatePolynomial<Rational>("1");
    int q = multivariateTarskiQuery(Q, tab, gb);
    std::cout << "tarski query: " << q << std::endl;
}






/*TEST_F(TarskiQueryTest2, MultiplicationTable) {
   
    MultiplicationTable<Rational> tab;
    tab.init(gb);
}*/