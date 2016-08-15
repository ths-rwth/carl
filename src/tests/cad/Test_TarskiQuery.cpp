#include "gtest/gtest.h"

#include "carl/core/VariablePool.h"
#include "carl/util/stringparser.h"
#include "carl/groebner/groebner.h"

#include "carl/thom/TarskiQuery/TarskiQuery.h"
#include "carl/thom/TarskiQuery/GroebnerBase.h"
#include "carl/thom/TarskiQuery/MultiplicationTable2.h"

#include "../Common.h"

#include <algorithm>

using namespace carl;


TEST(TarskiQueryUnivariateTest, Query) {
    
    typedef UnivariatePolynomial<Rational> Polynomial;
    Variable x = freshRealVariable("x");
    
    // Real algebraic geometry, example page 393
    Polynomial p1(x, {(Rational)-2, (Rational)1});
    Polynomial p2(x, {(Rational)1, (Rational)1});
    Polynomial p3(x, {(Rational)0, (Rational)1});
    Polynomial one = p1.one();
    Polynomial z(x, {(Rational)9, (Rational)0, (Rational)-1, (Rational)-9, (Rational)0, (Rational)1});
    
    // set up the tarski query manager
    std::vector<Polynomial> zeroSet;
    zeroSet.push_back(z);
    TarskiQueryManager<Polynomial> tarskiQuery(zeroSet);
    
    EXPECT_EQ(tarskiQuery(one), 3);
    EXPECT_EQ(tarskiQuery(p3), 1);
    EXPECT_EQ(tarskiQuery(p3*p3), 3);
    EXPECT_EQ(tarskiQuery(p2), 1);
    EXPECT_EQ(tarskiQuery(p2*p2), 3);
    EXPECT_EQ(tarskiQuery(p2*p3), 3);
    EXPECT_EQ(tarskiQuery(p1), -1);
    EXPECT_EQ(tarskiQuery(p1*p1), 3);
    EXPECT_EQ(tarskiQuery(p1*p3), 1);
    EXPECT_EQ(tarskiQuery(p1*p2), 1);
    
    // TODO add some more tests here
}

/*
 * 2 dimensional, intersecting circle and ellipse, 4 zeros
 */
class TarskiQueryMultivariateTest : public ::testing::Test {
    
protected:
    
    typedef MultivariatePolynomial<Rational> Polynomial;
    
    StringParser sp;
        
    Polynomial circle;
    Polynomial ellipse;
    
    std::vector<Polynomial> zeroSet;
    std::vector<Polynomial> queryPolynomials;
    std::vector<int> queryResults;
    
    _Monomial<Rational> mon_0;
    _Monomial<Rational> mon_1;
    _Monomial<Rational> mon_2;
    _Monomial<Rational> mon_3;
    
    std::vector<Polynomial> gb;
    
        
    virtual void SetUp() {
        sp.setVariables({"x", "y"});
        circle = sp.parseMultivariatePolynomial<Rational>("x^2 + y^2 + -1");
        std::cout << "circle = " << circle << std::endl;
        ellipse = sp.parseMultivariatePolynomial<Rational>("x^2 + 4*y^2 + x*y + -2");
        std::cout << "ellipse = " << ellipse << std::endl;
        zeroSet = {circle, ellipse};
        
        // define some polynomials for which the query on the zero set will be tested
        Polynomial qp1 = circle;                                                        // result 0
        Polynomial qp2 = ellipse;                                                       // result 0
        Polynomial qp3 = sp.parseMultivariatePolynomial<Rational>("1");                 // result 4
        Polynomial qp4 = -qp3;                                                          // result -4
        Polynomial qp5 = sp.parseMultivariatePolynomial<Rational>("x^2 + -1*y^2 + -2"); // result -4
        Polynomial qp6 = -qp4;                                                          // result 4
        Polynomial qp7 = sp.parseMultivariatePolynomial<Rational>("y + -1*x^2");        // result -2
        Polynomial qp8 = -qp7;                                                          // result 2
        
        queryPolynomials = {qp1, qp2, qp3, qp4, qp5, qp6, qp7, qp8};
        queryResults = {0, 0, 4, -4, -4, 4, -2, 2};
        
        mon_0 = sp.parseTerm<Rational>("1");
        mon_1 = sp.parseTerm<Rational>("x");
        mon_2 = sp.parseTerm<Rational>("y");
        mon_3 = sp.parseTerm<Rational>("x^2");
        
        GBProcedure<Polynomial, Buchberger, StdAdding> gbobject;
        gbobject.addPolynomial(circle);
        gbobject.addPolynomial(ellipse);
        gbobject.reduceInput();
        gbobject.calculate();
        gb = gbobject.getIdeal().getGenerators();
        
        std::cout << "groebner base is: " << gb << std::endl;
    }
};

TEST(TarskiQuery, Table2) {
    typedef MultivariatePolynomial<Rational> Polynomial;
    StringParser sp;  
    sp.setVariables({"x", "y"});
    Polynomial circle = sp.parseMultivariatePolynomial<Rational>("x^2 + y^2 + -1");
    Polynomial ellipse = sp.parseMultivariatePolynomial<Rational>("x^2 + 4*y^2 + x*y + -2");
    Polynomial curve = sp.parseMultivariatePolynomial<Rational>("y^3*x + -4*y + -1*x");
    Polynomial pol = sp.parseMultivariatePolynomial<Rational>("y^2*x^2 + x*y + -1*x + 2");
    std::vector<Polynomial> polys = {circle, ellipse};
    
    GroebnerBase<Rational> gb(polys.begin(), polys.end());
    std::cout << "input: " << polys << std::endl;
    std::cout << "groebner base: " << gb.get() << std::endl;
    std::cout << "reduce " << circle << ": " << gb.reduce(circle) << std::endl;
    std::cout << "reduce " << curve << ": " << gb.reduce(curve) << std::endl;
    std::cout << "cor: " << gb.cor() << std::endl;
    std::cout << "mon: " << gb.mon() << std::endl;
    std::cout << "bor: " << gb.bor() << std::endl;
    
    MultiplicationTable2<Rational> mult_tab(gb);
    std::cout << mult_tab << std::endl;
    BaseRepresentation<Rational> curve_br(mult_tab.getBase(), gb.reduce(curve));
    BaseRepresentation<Rational> pol_br(mult_tab.getBase(), gb.reduce(pol));
    std::cout << "curve_br = " << curve_br << std::endl;
    std::cout << "pol_br = " << pol_br << std::endl;
    std::cout << "multiply(curve_br, pol_br): " << mult_tab.multiply(curve_br, pol_br) << std::endl;
    
    _Monomial<Rational> mon1 = sp.parseTerm<Rational>("x^4");
    _Monomial<Rational> mon2 = sp.parseTerm<Rational>("x^2*y");
    BaseRepresentation<Rational> br_mon1 = mult_tab.getEntry(mon1).br;
    BaseRepresentation<Rational> br_mon2 = mult_tab.getEntry(mon2).br;
    std::cout << "br_mon1 = " << br_mon1 << std::endl;
    std::cout << "br_mon2 = " << br_mon2 << std::endl;
    std::cout << "multiply(br_mon1, br_mon2): " << mult_tab.multiply(br_mon1, br_mon2) << std::endl;
    std::cout << "multiply(br_mon2, br_mon1): " << mult_tab.multiply(br_mon2, br_mon1) << std::endl;
    
    MultivariatePolynomial<Rational> p = sp.parseMultivariatePolynomial<Rational>("x^4 + y^3");
    std::cout << "trace: " << mult_tab.trace(mult_tab.reduce(p)) << std::endl;
}

TEST(TarskiQuery, Groebner) {
    typedef MultivariatePolynomial<Rational> Polynomial;
    
    Variable x = freshRealVariable("x");
    Variable y = freshRealVariable("y");
    Variable z = freshRealVariable("z");
    
    Polynomial p1 = Polynomial({Term<Rational>(2)*y*y*y, Term<Rational>(-1)});
    Polynomial p2 = Polynomial({Term<Rational>(x)*x, Term<Rational>(y)*y});
    Polynomial p3 = Polynomial({Term<Rational>(y)*y, Term<Rational>(x)*x, Term<Rational>(-1)});
    Polynomial p4 = Polynomial({Term<Rational>(2)*z*z*z, Term<Rational>(-1)});
    Polynomial p5 = Polynomial({Term<Rational>(z)*z, Term<Rational>(x)*x});
    Polynomial p6 = Polynomial({Term<Rational>(z)*z, Term<Rational>(x)*x, Term<Rational>(-1)});
    Polynomial p7 = Polynomial({Term<Rational>(y)*y*x*x, Term<Rational>(x)*x*x*x, Term<Rational>(y)*y, Term<Rational>(-1)});
    Polynomial p8 = Polynomial({Term<Rational>(x)*x*x*x, Term<Rational>(-1)});
    std::cout << "p1 = " << p1 << std::endl;
    std::cout << "p2 = " << p2 << ", lmon is " << p2.lmon() << std::endl;
    std::cout << "p3 = " << p3 << std::endl;
    std::cout << "p4 = " << p4 << std::endl;
    std::cout << "p5 = " << p5 << std::endl;
    std::cout << "p6 = " << p6 << std::endl;
    std::cout << "p7 = " << p7 << std::endl;
    std::cout << "p8 = " << p8 << std::endl;
    
    GBProcedure<Polynomial, Buchberger, StdAdding> gbobject;
    gbobject.addPolynomial(p1);
    gbobject.addPolynomial(p2);
    gbobject.reduceInput();
    gbobject.calculate();
    std::cout << "groebner basis of {p1,p2}: " << gbobject.getIdeal().getGenerators() << std::endl;
    
    GBProcedure<MultivariatePolynomial<Rational>, Buchberger, StdAdding> gbobject2;
    gbobject2.addPolynomial(p1);
    gbobject2.addPolynomial(p3);
    gbobject2.reduceInput();
    gbobject2.calculate();
    std::cout << "groebner basis of {p1,p3}: " << gbobject2.getIdeal().getGenerators() << std::endl;
    
    GBProcedure<MultivariatePolynomial<Rational>, Buchberger, StdAdding> gbobject3;
    gbobject3.addPolynomial(p4);
    gbobject3.addPolynomial(p5);
    gbobject3.reduceInput();
    gbobject3.calculate();
    std::cout << "groebner basis of {p4,p5}: " << gbobject3.getIdeal().getGenerators() << std::endl;
    
    GBProcedure<MultivariatePolynomial<Rational>, Buchberger, StdAdding> gbobject4;
    gbobject4.addPolynomial(p4);
    gbobject4.addPolynomial(p6);
    gbobject4.reduceInput();
    gbobject4.calculate();
    std::cout << "groebner basis of {p4,p6}: " << gbobject4.getIdeal().getGenerators() << std::endl;
    
    GBProcedure<MultivariatePolynomial<Rational>, Buchberger, StdAdding> gbobject5;
    gbobject5.addPolynomial(p7);
    gbobject5.addPolynomial(p8);
    gbobject5.reduceInput();
    gbobject5.calculate();
    std::cout << "groebner basis of {p7,p8}: " << gbobject5.getIdeal().getGenerators() << std::endl;
}

TEST_F(TarskiQueryMultivariateTest, Cor) {   
    std::cout << "cor = " << cor(gb) << std::endl;
}

TEST_F(TarskiQueryMultivariateTest, Mon) { 
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

TEST_F(TarskiQueryMultivariateTest, bor) {
    std::cout << "bor = " << bor(gb) << std::endl;
}

TEST_F(TarskiQueryMultivariateTest, MultiplicationTable) {
    MultiplicationTable<Rational> tab;
    tab.init(gb);
    std::cout << tab << std::endl;
    
    std::cout << "----------------------------" << std::endl;
    
    // test for multiply   
    _Monomial<Rational> mon1 = sp.parseTerm<Rational>("x^4");
    _Monomial<Rational> mon2 = sp.parseTerm<Rational>("x^2*y");
    std::cout << "representation of x^4: " << tab.get(mon1) << std::endl;
    std::cout << "representation of x^2*y: " << tab.get(mon2) << std::endl;
    
    std::cout << "multiply: " << multiply(tab.get(mon1), tab.get(mon2), tab) << std::endl;  
}

TEST_F(TarskiQueryMultivariateTest, NormalForm) {
    MultivariatePolynomial<Rational> mon = sp.parseMultivariatePolynomial<Rational>("x^4");
    normalForm(mon, gb);
    
    std::cout << std::endl;
    
    _Monomial<Rational> mon1 = sp.parseTerm<Rational>("x^4");
    _Monomial<Rational> mon2 = sp.parseTerm<Rational>("x^2*y");
    normalForm(MultivariatePolynomial<Rational>(mon2 * mon1), gb);
    
    std::cout << std::endl;
    
    normalForm(circle, gb);
}

TEST_F(TarskiQueryMultivariateTest, MultiplicationTableInit2) {
    MultiplicationTable<Rational> tab;
    tab.init2(gb);
    std::cout << tab << std::endl;
}

TEST_F(TarskiQueryMultivariateTest, Trace) {
    MultiplicationTable<Rational> tab;
    tab.init2(gb);
    
    std::cout << tab << std::endl;
    
    // a random polynomial
    MultivariatePolynomial<Rational> p = sp.parseMultivariatePolynomial<Rational>("x^4 + y^3");
    std::cout << trace(normalForm(p, gb), tab) << std::endl;
}

/*TEST_F(TarskiQueryMultivariateTest, MultivariateTarskiQuery) {
    MultiplicationTable<Rational> tab;
    tab.init2(gb);
    
    MultivariatePolynomial<Rational> Q = sp.parseMultivariatePolynomial<Rational>("x^2 + -1*y^2 + -2");
    int q = multivariateTarskiQuery(Q, tab);
    std::cout << "tarski query: " << q << std::endl;
    
    
}*/

TEST_F(TarskiQueryMultivariateTest, Query) {
    
    TarskiQueryManager<Polynomial> tarskiQuery(zeroSet);
    
    for(size_t i = 0; i < queryPolynomials.size(); i++) {
        EXPECT_EQ(tarskiQuery(queryPolynomials[i]), queryResults[i]);
    }
    
}

/*
 * 2 dimensional, two parallel lines and a higher degree curve, 4 zeros
 */
class TarskiQueryMultivariateTest2 : public ::testing::Test {
    
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

/*TEST_F(TarskiQueryMultivariateTest2, MultivariateTarskiQuery) {
    MultiplicationTable<Rational> tab;
    tab.init2(gb);
    
    std::cout << tab << std::endl;
    
    MultivariatePolynomial<Rational> Q = sp.parseMultivariatePolynomial<Rational>("1");
    int q = multivariateTarskiQuery(Q, tab, gb);
    std::cout << "tarski query: " << q << std::endl;
}*/






/*TEST_F(TarskiQueryTest2, MultiplicationTable) {
   
    MultiplicationTable<Rational> tab;
    tab.init(gb);
}*/