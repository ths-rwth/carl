#include "gtest/gtest.h"
#include "../Common.h"

#include "carl/core/VariablePool.h"

#include "carl/thom/ThomEncoding.h"
#include "carl/thom/ThomRootFinder.h"



using namespace carl;

TEST(Thom, RootFinder) {
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
    for(Rational r(-5); r <= Rational(5); r = r + Rational(1)) {
        EXPECT_TRUE(roots[i].represents(r));
        i++;
    }
}

TEST(Thom, Comparison) {
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
