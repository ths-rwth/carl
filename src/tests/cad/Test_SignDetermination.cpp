#include "gtest/gtest.h"
#include "../Common.h"

#include "carl/core/VariablePool.h"
#include "carl/util/stringparser.h"

#include "carl/thom/SignDetermination/SignDetermination.h"


using namespace carl;


TEST(SignDetermination, Univariate) {
    typedef UnivariatePolynomial<Rational> Polynomial;
    Variable x = freshRealVariable("x");
    
    // Real algebraic geometry, example page 393
    Polynomial p1(x, {(Rational)-2, (Rational)1});
    Polynomial p2(x, {(Rational)1, (Rational)1});
    Polynomial p3(x, {(Rational)0, (Rational)1});
    Polynomial p4(x, {(Rational)50, (Rational)-50});
    Polynomial p5(x, {(Rational)-50});
    Polynomial z(x, {(Rational)9, (Rational)0, (Rational)-1, (Rational)-9, (Rational)0, (Rational)1});
    
    std::vector<Polynomial> p = {p1, p2, p3};
    std::vector<Polynomial> zeroSet = {z};
    signDetermination(p, zeroSet);
    
    UnivariatePolynomial<Rational> a(x, {(Rational)-2, (Rational)0, (Rational)1});
    UnivariatePolynomial<Rational> b(x, {(Rational)2, (Rational)1});
    UnivariatePolynomial<Rational> c = a*a*b;
    std::vector<UnivariatePolynomial<Rational>> deriv({c.derivative(), c.derivative(2), c.derivative(3), c.derivative(4), c.derivative(5)});
    //signDetermination(deriv, c);
    
    z = UnivariatePolynomial<Rational>(x, {(Rational)0, (Rational)-1, (Rational)0, (Rational)1});
    p1 = UnivariatePolynomial<Rational>(x, {(Rational)2, (Rational)1});
    p2 = UnivariatePolynomial<Rational>(x, {(Rational)-2, (Rational)1});
    p3 = UnivariatePolynomial<Rational>(x, {(Rational)0, (Rational)-1});
    p4 = z.one();
    p = std::vector<UnivariatePolynomial<Rational>>({p4, p3, p2, p1});
    //signDetermination(p, z, true);
}

TEST(SignDetermination, Multivariate) {
    typedef MultivariatePolynomial<Rational> Polynomial;
    
    StringParser sp;
    sp.setVariables({"x","y"});
    
    Polynomial circle = sp.parseMultivariatePolynomial<Rational>("x^2 + y^2 + -1");
    Polynomial ellipse = sp.parseMultivariatePolynomial<Rational>("x^2 + 4*y^2 + x*y + -2");
    std::vector<Polynomial> zeroSet = {circle, ellipse};
    
    Polynomial p1 = sp.parseMultivariatePolynomial<Rational>("x");
    Polynomial p2 = sp.parseMultivariatePolynomial<Rational>("y");
    std::vector<Polynomial> p = {p1*p1};
    
    signDetermination(p, zeroSet);
    
    
    Polynomial lines = sp.parseMultivariatePolynomial<Rational>("x^2 + -1");
    zeroSet = {lines, ellipse};
    std::map<std::string, Variable> vars = sp.variables();
    Variable x = vars["x"];
    Variable y = vars["y"];
    std::vector<Polynomial> der = {lines.derivative(x), lines.derivative(x).derivative(x),
                                   ellipse.derivative(y), ellipse.derivative(y).derivative(y)};
    signDetermination(der, zeroSet);
}