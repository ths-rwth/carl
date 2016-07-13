#include "gtest/gtest.h"
#include "../Common.h"

#include "carl/core/VariablePool.h"
#include "carl/util/stringparser.h"

#include "carl/thom/ThomEncoding.h"
#include "carl/thom/ThomRootFinder.h"



using namespace carl;

TEST(Thom, Constructors) {   
    Variable x = freshRealVariable("x");
    
    ThomEncoding<Rational> enc(Rational(3), x);
    std::cout << "enc = " << enc << std::endl;
    EXPECT_TRUE(enc.isConsistent());
    EXPECT_TRUE(enc.dimension() == 1);
    EXPECT_TRUE(enc.isOneDimensional());
    EXPECT_TRUE(enc.represents(Rational(3)));
    
    UnivariatePolynomial<Rational> univPol(x, {Rational(-3), Rational(1)});
    std::shared_ptr<UnivariatePolynomial<Rational>> ptr = std::make_shared<UnivariatePolynomial<Rational>>(univPol);
    ThomEncoding<Rational> enc2(ptr, {});
    std::cout << "enc2 = " << enc2 << std::endl;
    EXPECT_TRUE(enc == enc2);
    
}

TEST(Thom, ComparisonUnivariateSamePoly) {
    typedef MultivariatePolynomial<Rational> Polynomial;
    
    Variable x = freshRealVariable("x");
    ThomEncoding<Rational> encSmall(Rational(-1), x);
    ThomEncoding<Rational> encBig(Rational(0), x);
    EXPECT_TRUE(encSmall < encBig);
    EXPECT_TRUE(encSmall == encSmall);
    EXPECT_TRUE(encBig == encBig);
    EXPECT_FALSE(encSmall == encBig);
    EXPECT_FALSE(encSmall > encBig);
    
    StringParser sp;
    sp.setVariables({"x", "y"});
    Polynomial pol = sp.parseMultivariatePolynomial<Rational>("x^2 + -2");
    
    std::shared_ptr<Polynomial> polyPointer = std::make_shared<Polynomial>(pol);
    encSmall = ThomEncoding<Rational>(polyPointer, {Sign::NEGATIVE});
    encBig = ThomEncoding<Rational>(polyPointer, {Sign::POSITIVE});
    std::cout << "encSmall.polynomial() = " << encSmall.polynomial() << std::endl;
    std::cout << "encBig.polynomial() = " << encBig.polynomial() << std::endl;
    EXPECT_TRUE(encSmall.polynomial() == encBig.polynomial());
    EXPECT_TRUE(encSmall < encBig);
}

TEST(Thom, ComparisonUnivariateDifferentPoly) {
    Variable x = freshRealVariable("x");
    UnivariatePolynomial<Rational> p(x, {(Rational)-2, (Rational)0, (Rational)1});
    UnivariatePolynomial<Rational> q(x, {(Rational)-3, (Rational)4, (Rational)-1});
    
    std::vector<ThomEncoding<Rational>> rp = realRoots(p);
    std::vector<ThomEncoding<Rational>> rq = realRoots(q);
    EXPECT_TRUE(rp.size() == 2 && rq.size() == 2);
    EXPECT_TRUE(rp[0] < rq[0]);
    EXPECT_TRUE(rp[0] < rq[1]);
    EXPECT_TRUE(rp[1] > rq[0]);
    EXPECT_TRUE(rp[1] < rq[1]);
}


TEST(Thom, RootFinderUnivariate) {
    Variable x = freshRealVariable("x");
    
    UnivariatePolynomial<Rational> a(x, {(Rational)-2, (Rational)0, (Rational)1});
    UnivariatePolynomial<Rational> b(x, {(Rational)2, (Rational)1});
    UnivariatePolynomial<Rational> c = a*b; 
    realRoots(c);
    
    UnivariatePolynomial<Rational> f1(x, {(Rational)-5, (Rational)1});
    UnivariatePolynomial<Rational> f2(x, {(Rational)-4, (Rational)1});
    UnivariatePolynomial<Rational> f3(x, {(Rational)-3, (Rational)1});
    UnivariatePolynomial<Rational> f4(x, {(Rational)-2, (Rational)1});
    UnivariatePolynomial<Rational> f5(x, {(Rational)-1, (Rational)1});
    UnivariatePolynomial<Rational> f6(x, {(Rational)1, (Rational)1});
    UnivariatePolynomial<Rational> f7(x, {(Rational)2, (Rational)1});
    UnivariatePolynomial<Rational> f8(x, {(Rational)3, (Rational)1});
    UnivariatePolynomial<Rational> f9(x, {(Rational)4, (Rational)1});
    UnivariatePolynomial<Rational> f10(x, {(Rational)5, (Rational)1});
    UnivariatePolynomial<Rational> f11(x, {(Rational)0, (Rational)1});
    UnivariatePolynomial<Rational> f = f1*f2*f3*f4*f5*f6*f7*f8*f9*f10*f11;
    
    std::vector<ThomEncoding<Rational>> roots = realRoots(f);
    
    unsigned i = 0;
    for(Rational r(-5); r <= Rational(5); r++) {
        EXPECT_TRUE(roots[i].represents(r));
        i++;
    }
}


TEST(Thom, RootFinderMultivariate) {
    typedef MultivariatePolynomial<Rational> Polynomial;
    StringParser sp;
    sp.setVariables({"x", "y"});
    //Variable x = sp.variables().at("x");
    Variable y = sp.variables().at("y");
    
    // Lecture slides "Satisfiability Checking" (slide 32 in CAD)
    Polynomial p1 = sp.parseMultivariatePolynomial<Rational>("x^2 + -4*x + y^2 + -4*y + 7");
    Polynomial p2 = sp.parseMultivariatePolynomial<Rational>("x + -1*y");
    
    Polynomial proj1 = sp.parseMultivariatePolynomial<Rational>("x^2 + -4*x + 3");
    Polynomial proj2 = sp.parseMultivariatePolynomial<Rational>("-8*x + 2*x^2 + 7"); proj2 /= Rational(2);
    
    std::vector<ThomEncoding<Rational>> roots_proj1 = realRoots(proj1.toUnivariatePolynomial());
    EXPECT_TRUE(roots_proj1.size() == 2);
    EXPECT_TRUE(roots_proj1[0].represents(Rational(1)));
    EXPECT_TRUE(roots_proj1[1].represents(Rational(3)));
    
    std::vector<ThomEncoding<Rational>> roots_proj2 = realRoots(proj2.toUnivariatePolynomial());
    EXPECT_TRUE(roots_proj2.size() == 2);
    
    std::vector<ThomEncoding<Rational>> rootsRightInner = realRoots(p1, y, roots_proj2[1]);
    EXPECT_TRUE(rootsRightInner.size() == 2);
    
    std::vector<ThomEncoding<Rational>> rootsRightInner2 = realRoots(p2, y, roots_proj2[1]);
    EXPECT_TRUE(rootsRightInner2.size() == 1);
       
    EXPECT_TRUE(rootsRightInner[0] < rootsRightInner2[0]);
    EXPECT_TRUE(rootsRightInner[1] == rootsRightInner2[0]);
     
    Polynomial y5 = sp.parseMultivariatePolynomial<Rational>("y^5 + -5*y^3 + 4*y");
    //std::vector<ThomEncoding<Rational>> roots5 = realRoots(y5, y, roots_proj1[0]);
    //std::cout << "roots5 = " << roots5 << std::endl;
}

TEST(Thom, IntermediatePointsUnivariate) {
    typedef MultivariatePolynomial<Rational> Polynomial;
    StringParser sp;
    sp.setVariables({"x", "y"});
    
    Polynomial proj1 = sp.parseMultivariatePolynomial<Rational>("x^2 + -4*x + 3");
    Polynomial proj2 = sp.parseMultivariatePolynomial<Rational>("-8*x + 2*x^2 + 7"); proj2 /= Rational(2);
    
    std::vector<ThomEncoding<Rational>> roots_proj1 = realRoots(proj1.toUnivariatePolynomial());
    std::vector<ThomEncoding<Rational>> roots_proj2 = realRoots(proj2.toUnivariatePolynomial());
    
    ThomEncoding<Rational> intermediate1 = intermediatePoint(roots_proj1[0], roots_proj2[0]);  
    EXPECT_TRUE(intermediate1 > roots_proj1[0]);
    EXPECT_TRUE(intermediate1 < roots_proj2[0]);
    
    ThomEncoding<Rational> intermediate2 = intermediatePoint(roots_proj2[1], roots_proj1[1]);
    EXPECT_TRUE(intermediate2 > roots_proj2[1]);
    EXPECT_TRUE(intermediate2 < roots_proj1[1]);
    
    std::cout << "intermediate1 = " << intermediate1 << std::endl;
    std::cout << "intermediate2 = " << intermediate2 << std::endl;
}

TEST(Thom, ThreeVariables) {
    typedef MultivariatePolynomial<Rational> Polynomial;
    Variable x = freshRealVariable("x");
    Variable y = freshRealVariable("y");
    Variable z = freshRealVariable("z");
    Variable w = freshRealVariable("w");
    
    Rational a = 1;
    Rational b = -1;
    Rational c = 2;
    
    Polynomial p1 = x - a;
    a ++;
    Polynomial tmp = a - x;
    Polynomial p2 = (y - b) * tmp;
    b ++;
    Polynomial tmp2 = b - x;
    Polynomial p3 = (z - c) * tmp * tmp2;
    a--; b--;
    
    // p1 = (x- a)
    // p2 = (y - b) * (a + 1 - x)
    // p3 = (z - c) * (b + 1 - y) * (a + 1 - x)
    
    std::vector<ThomEncoding<Rational>> roots_p1 = realRoots(p1.toUnivariatePolynomial());
    std::cout << roots_p1[0] << std::endl;
    EXPECT_TRUE(roots_p1.size() == 1);
    EXPECT_TRUE(roots_p1[0].represents(a));
    
    std::vector<ThomEncoding<Rational>> roots_p2_p1 = realRoots(p2, y, roots_p1[0]);
    EXPECT_TRUE(roots_p2_p1.size() == 1);
    std::cout << roots_p2_p1[0] << std::endl;
    
    std::vector<ThomEncoding<Rational>> roots_p3_p2_p1 = realRoots(p3, z, roots_p2_p1[0]);
    EXPECT_TRUE(roots_p3_p2_p1.size() == 1);
    std::cout << roots_p3_p2_p1[0] << std::endl;
    
    Polynomial p4(z);
    p4 *= z;
    p4 += x;
    p4 += y;
    p4 += Rational(-2);
    std::vector<ThomEncoding<Rational>> roots_p4_p2_p1 = realRoots(p4, z, roots_p2_p1[0]);
    EXPECT_TRUE(roots_p4_p2_p1.size() == 2);
    std::cout << roots_p4_p2_p1[0] << std::endl;
    std::cout << roots_p4_p2_p1[1] << std::endl;
    EXPECT_TRUE(roots_p4_p2_p1[0] < roots_p4_p2_p1[1]);
    
    
    Polynomial p5(z);
    p5 *= z;
    p5 = w - p5;
    std::vector<ThomEncoding<Rational>> roots_p5_p4_p2_p1 = realRoots(p5, w, roots_p4_p2_p1[0]);
    std::cout << roots_p5_p4_p2_p1[0] << std::endl;
    EXPECT_TRUE(roots_p5_p4_p2_p1.size() == 1);
    

}
