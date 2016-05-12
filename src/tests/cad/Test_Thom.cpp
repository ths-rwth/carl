#include "gtest/gtest.h"

#include "carl/thom/ThomEncoding.h"
#include "carl/thom/TarskiQuery.h"
#include "carl/thom/ThomRootFinder.h"
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
    UnivariatePolynomial<Rational> c = a*a*b;
    std::vector<UnivariatePolynomial<Rational>> deriv({c.derivative(), c.derivative(2), c.derivative(3), c.derivative(4), c.derivative(5)});
    //signDetermination(deriv, c);
    
    z = UnivariatePolynomial<Rational>(x, {(Rational)0, (Rational)-1, (Rational)0, (Rational)1});
    p1 = UnivariatePolynomial<Rational>(x, {(Rational)2, (Rational)1});
    p2 = UnivariatePolynomial<Rational>(x, {(Rational)-2, (Rational)1});
    p3 = UnivariatePolynomial<Rational>(x, {(Rational)0, (Rational)-1});
    p4 = z.one();
    p = std::vector<UnivariatePolynomial<Rational>>({p4, p3, p2, p1});
    signDetermination(p, z, true);
}

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
