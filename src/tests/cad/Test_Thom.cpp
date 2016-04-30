#include "gtest/gtest.h"

#include "carl/thom/ThomEncoding.h"
#include "carl/thom/TarskiQuery.h"
#include "carl/core/VariablePool.h"

#include "../Common.h"

using namespace carl;

TEST(Thom, TarskiQuery) {
    Variable x = freshRealVariable("x");
    
    // Real algebraic geometry, example page 393
    UnivariatePolynomial<Rational> p1(x, {(Rational)-2, (Rational)1});
    UnivariatePolynomial<Rational> p2(x, {(Rational)1, (Rational)1});
    UnivariatePolynomial<Rational> p3(x, {(Rational)0, (Rational)1});
    UnivariatePolynomial<Rational> one = p1.one();
    UnivariatePolynomial<Rational> z(x, {(Rational)9, (Rational)0, (Rational)-1, (Rational)-9, (Rational)0, (Rational)1});
    
    std::cout << "tarski query of " << one << " and " << z << ": ";
    int taq = tarskiQuery<Rational>(one, z);
    std::cout << taq << std::endl;
    EXPECT_EQ(taq, 3);
    
    std::cout << "tarski query of " << p3 << " and " << z << ": ";
    taq = tarskiQuery<Rational>(p3, z);
    std::cout << taq << std::endl;
    EXPECT_EQ(taq, 1);
    
    std::cout << "tarski query of " << p3*p3 << " and " << z << ": ";
    taq = tarskiQuery<Rational>(p3*p3, z);
    std::cout << taq << std::endl;
    EXPECT_EQ(taq, 3);
    
    std::cout << "tarski query of " << p2 << " and " << z << ": ";
    taq = tarskiQuery<Rational>(p2, z);
    std::cout << taq << std::endl;
    EXPECT_EQ(taq, 1);
    
    std::cout << "tarski query of " << p2*p2 << " and " << z << ": ";
    taq = tarskiQuery<Rational>(p2*p2, z);
    std::cout << taq << std::endl;
    EXPECT_EQ(taq, 3);
    
    std::cout << "tarski query of " << p2*p3 << " and " << z << ": ";
    taq = tarskiQuery<Rational>(p2*p3, z);
    std::cout << taq << std::endl;
    EXPECT_EQ(taq, 3);
    
    std::cout << "tarski query of " << p1 << " and " << z << ": ";
    taq = tarskiQuery<Rational>(p1, z);
    std::cout << taq << std::endl;
    EXPECT_EQ(taq, -1);
    
    std::cout << "tarski query of " << p1*p1 << " and " << z << ": ";
    taq = tarskiQuery<Rational>(p1*p1, z);
    std::cout << taq << std::endl;
    EXPECT_EQ(taq, 3);
    
    std::cout << "tarski query of " << p1*p3 << " and " << z << ": ";
    taq = tarskiQuery<Rational>(p1*p3, z);
    std::cout << taq << std::endl;
    EXPECT_EQ(taq, 1);
    
    std::cout << "tarski query of " << p1*p2 << " and " << z << ": ";
    taq = tarskiQuery<Rational>(p1*p2, z);
    std::cout << taq << std::endl;
    //EXPECT_EQ(taq, 1);
    
    // TODO add some more tests here
}

TEST(Thom, SignDetermination) {
    Variable x = freshRealVariable("x");
    
    // Real algebraic geometry, example page 393
    UnivariatePolynomial<Rational> p1(x, {(Rational)-2, (Rational)1});
    UnivariatePolynomial<Rational> p2(x, {(Rational)1, (Rational)1});
    UnivariatePolynomial<Rational> p3(x, {(Rational)0, (Rational)1});
    UnivariatePolynomial<Rational> p4(x, {(Rational)50, (Rational)-50});
    UnivariatePolynomial<Rational> p5(x, {(Rational)-50});
    UnivariatePolynomial<Rational> z(x, {(Rational)9, (Rational)0, (Rational)-1, (Rational)-9, (Rational)0, (Rational)1});
    
    std::vector<UnivariatePolynomial<Rational>> p({p1, p2, p3}); //p4, p5});
    //signDetermination(p, z);
    
    UnivariatePolynomial<Rational> a(x, {(Rational)-2, (Rational)0, (Rational)1});
    UnivariatePolynomial<Rational> b(x, {(Rational)2, (Rational)1});
    UnivariatePolynomial<Rational> c = a*b;
    std::vector<UnivariatePolynomial<Rational>> deriv({c.derivative(), c.derivative(2), c.derivative(3), c.derivative(4), c.derivative(4)});
    signDetermination(deriv, c);
}
