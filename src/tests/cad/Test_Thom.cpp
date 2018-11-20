#include "gtest/gtest.h"
#include "../Common.h"

#include "carl/core/VariablePool.h"
#include "carl/core/polynomialfunctions/Derivative.h"
#include "carl/util/stringparser.h"

#include "carl/thom/SignDetermination/SignDetermination.h"
#include "carl/thom/ThomRootFinder.h"
#include "carl/thom/ThomEvaluation.h"

using namespace carl;

TEST(Thom, SignDetermination) {
        typedef UnivariatePolynomial<Rational> UPolynomial;
        typedef MultivariatePolynomial<Rational> MPolynomial;
        Variable x = freshRealVariable("x");

        // Real algebraic geometry, example page 393
        UPolynomial p1(x, {(Rational)-2, (Rational)1});
        UPolynomial p2(x, {(Rational)1, (Rational)1});
        UPolynomial p3(x, {(Rational)0, (Rational)1});
        UPolynomial z(x, {(Rational)9, (Rational)0, (Rational)-1, (Rational)-9, (Rational)0, (Rational)1});

        std::vector<MPolynomial> polys = {MPolynomial(p1), MPolynomial(p2), MPolynomial(p3)};
        std::vector<MPolynomial> zeroSet = {MPolynomial(z)};
        
        SignDetermination<Rational> sd(zeroSet.begin(), zeroSet.end());
        sd.getSigns(MPolynomial(p3));
        sd.getSignsAndAdd(MPolynomial(p3));
        sd.getSigns(MPolynomial(p2));
        sd.getSignsAndAdd(MPolynomial(p2));
        sd.getSigns(MPolynomial(p1));
        sd.getSignsAndAdd(MPolynomial(p1));

        UPolynomial f1(x, {(Rational)-5, (Rational)1});
        
        MPolynomial product(Rational(1));
        for(int i = 0; i < 10; i++) {
                f1 = f1 + Rational(1);
                product = product * MPolynomial(f1); 
        }
        zeroSet = {product};
        
        SignDetermination<Rational> sd2(zeroSet.begin(), zeroSet.end());
        sd2.getSignsAndAdd(MPolynomial(p3));
        sd2.getSignsAndAdd(MPolynomial(p2));
        sd2.getSignsAndAdd(MPolynomial(p1));
        sd2.getSignsAndAdd(MPolynomial(p1));
        sd2.getSignsAndAdd(MPolynomial(p1));
        
        Variable y = freshRealVariable("y");
        UPolynomial f2(x, {(Rational)-2, (Rational)1});
        MPolynomial fourZeros(Rational(1));
        for(int i = 0; i < 5; i++) {
                f2 = f2 + Rational(1);
                fourZeros = fourZeros * MPolynomial(f2); 
        }
        
        MPolynomial ellipse({Rational(8)*x*x, Rational(9)*y*y, Rational(-8)*x, Rational(-36)*y, Term<Rational>(Rational(20))});
        zeroSet = {ellipse, fourZeros};
        SignDetermination<Rational> sd3(zeroSet.begin(), zeroSet.end());
        
        sd3.getSignsAndAdd(carl::derivative(fourZeros, x, 4));
        sd3.getSignsAndAdd(carl::derivative(fourZeros, x, 3));
        sd3.getSignsAndAdd(carl::derivative(fourZeros, x, 2));
        sd3.getSignsAndAdd(carl::derivative(fourZeros, x, 1));
        sd3.getSignsAndAdd(carl::derivative(ellipse, y, 2));
        sd3.getSignsAndAdd(carl::derivative(ellipse, y, 1));
}


TEST(Thom, RootFinder) {
        typedef MultivariatePolynomial<Rational> Polynomial;
        typedef ThomEncoding<Rational> TE;
        Variable x = freshRealVariable("x");
        Variable y = freshRealVariable("y");
        Variable z = freshRealVariable("z");
        
        Polynomial poly1({Rational(1)*x*x*x*x*x, Rational(-1)*x*x*x, Rational(1)*x*x, Rational(-2)*x, Term<Rational>(Rational(-2))});
        realRootsThom(poly1, x);
        
        std::cout << "\n\n";
        
        Polynomial poly2({Rational(1)*x*x*x*x*x, Rational(-1)*x});
        std::list<TE> realRootsPoly2 = realRootsThom(poly2, x);
        
        std::cout << "\n\n";
        
        //x^2-0.8 x+y^2-0.2
        Polynomial poly3({Rational(5)*x*x, Rational(-4)*x, Rational(5)*y*y, Term<Rational>(Rational(-1))});
        std::map<Variable, TE> m = {std::make_pair(x, realRootsPoly2.back())};
        std::list<TE> realRootsPoly3 = realRootsThom(poly3, y, m);
        EXPECT_TRUE(realRootsPoly3.size() == 1);
        
        std::cout << "\n\n";
        
        
        Polynomial poly4({Rational(1)*x, Rational(1)*y, Rational(1)*z});
        std::map<Variable, TE> m_xy = {std::make_pair(x, realRootsPoly2.back()),
                                        std::make_pair(y, realRootsPoly3.front())};
        std::list<TE> realRootsPoly4 = realRootsThom(poly4, z, m_xy);
        EXPECT_TRUE(realRootsPoly4.size() == 1);
        
}


TEST(Thom, Comparison) {
        typedef MultivariatePolynomial<Rational> Polynomial;
        typedef ThomEncoding<Rational> TE;
        Variable x = freshRealVariable("x");
        Variable y = freshRealVariable("y");
        
        
        
        Polynomial poly1({Rational(1)*x*x*x*x, Rational(-3)*x*x*x, Rational(1)*x, Term<Rational>(Rational(-2))});       // x⁴ - 3x³ + x - 2
        Polynomial poly2({Rational(1)*x*x, Term<Rational>(Rational(-2))});                                              // x² - 2
        
        std::list<TE> realRootsPoly1 = realRootsThom(poly1, x);
        std::list<TE> realRootsPoly2 = realRootsThom(poly2, x);
        EXPECT_EQ(realRootsPoly1.size(), 2);
        EXPECT_EQ(realRootsPoly2.size(), 2);
        EXPECT_TRUE(realRootsPoly1.front() > realRootsPoly2.front());
        EXPECT_TRUE(realRootsPoly2.front() < realRootsPoly1.front());
        EXPECT_TRUE(realRootsPoly1.back() > realRootsPoly2.front());
        EXPECT_TRUE(realRootsPoly2.front() < realRootsPoly1.back());
        EXPECT_TRUE(realRootsPoly1.back() > realRootsPoly2.back());
        EXPECT_TRUE(realRootsPoly2.back() < realRootsPoly1.back());

        
        
        
        Polynomial poly3({Rational(1)*x, Term<Rational>(Rational(-2))});                // x - 2
        Polynomial poly4({Rational(1)*x*x, Rational(-2)*x});                            // x² - 2x
        
        std::list<TE> realRootsPoly3 = realRootsThom(poly3, x);
        std::list<TE> realRootsPoly4 = realRootsThom(poly4, x);
        EXPECT_EQ(realRootsPoly3.size(), 1);
        EXPECT_EQ(realRootsPoly4.size(), 2);
        EXPECT_TRUE(realRootsPoly4.front() < realRootsPoly4.back());
        EXPECT_TRUE(realRootsPoly3.front() == realRootsPoly4.back());
        EXPECT_TRUE(realRootsPoly4.back() == realRootsPoly3.front());
        EXPECT_TRUE(realRootsPoly3.front() > realRootsPoly4.front());
        EXPECT_TRUE(realRootsPoly4.front() < realRootsPoly3.front());
        
        
        
        
        Polynomial poly5({Rational(1)*x, Term<Rational>(Rational(-1))});                         // x - 1
        Polynomial poly6({Rational(1)*x*x*x, Rational(5)*y*y, Term<Rational>(Rational(-2))});    // x³ + 5y² - 2
        Polynomial poly7({Rational(1)*y, Term<Rational>(Rational(-1))});                         // y - 1
        Polynomial poly8({Rational(1)*y*y, Rational(-1)*x, Term<Rational>(Rational(-1))});       // y² - x - 1
        
        std::list<TE> realRootsPoly5 = realRootsThom(poly5, x);
        EXPECT_EQ(realRootsPoly5.size(), 1);
        
        std::map<Variable, TE> m = {std::make_pair(x, realRootsPoly5.front())};
        std::list<TE> realRootsPoly6 = realRootsThom(poly6, y, m);
        EXPECT_EQ(realRootsPoly6.size(), 2);
        
        std::list<TE> realRootsPoly8 = realRootsThom(poly8, y, m);
        EXPECT_EQ(realRootsPoly8.size(), 2);
        
        std::list<TE> realRootsPoly7 = realRootsThom(poly7, y);
        EXPECT_EQ(realRootsPoly7.size(), 1);
        
        EXPECT_TRUE(realRootsPoly8.front() < realRootsPoly6.front());
        EXPECT_TRUE(realRootsPoly8.front() < realRootsPoly6.back());
        EXPECT_TRUE(realRootsPoly6.front() > realRootsPoly8.front());
        EXPECT_TRUE(realRootsPoly6.front() < realRootsPoly8.back());
        EXPECT_TRUE(realRootsPoly6.back() > realRootsPoly8.front());
        EXPECT_TRUE(realRootsPoly6.back() < realRootsPoly8.back());
        EXPECT_TRUE(realRootsPoly8.back() > realRootsPoly6.front());
        EXPECT_TRUE(realRootsPoly8.back() > realRootsPoly6.back());
        
        EXPECT_TRUE(realRootsPoly6.front() < Rational(-2/5));
        EXPECT_TRUE(realRootsPoly6.back() < Rational(1));
        EXPECT_TRUE(realRootsPoly6.back() > Rational(0));
        EXPECT_TRUE(realRootsPoly8.front() > Rational(-2));
        
        EXPECT_TRUE(realRootsPoly7.front() < realRootsPoly8.back());
        EXPECT_TRUE(realRootsPoly7.front() > realRootsPoly6.back());
        
        
        
}

TEST(Thom, Samples) {
        typedef MultivariatePolynomial<Rational> Polynomial;
        typedef ThomEncoding<Rational> TE;
        Variable x = freshRealVariable("x");
        Variable y = freshRealVariable("y");
        
        Polynomial poly1({Rational(1)*x*x, Term<Rational>(Rational(-2))});      // x² - 2
        Polynomial poly2({Rational(1)*x*y, Term<Rational>(Rational(-2))});      // xy - 2
        Polynomial poly3({Rational(1)*x*x*x, Term<Rational>(Rational(-2))});    // x³ - 2
        Polynomial poly4({Rational(1)*y*y, Term<Rational>(Rational(-1))});      // y² - 1
        
        std::list<TE> realRootsPoly1 = realRootsThom(poly1, x);
        EXPECT_EQ(realRootsPoly1.size(), 2);
        TE sqrt2 = realRootsPoly1.back();
        TE sqrt2plus1 = sqrt2 + Rational(1);
        std::cout << sqrt2plus1 << std::endl;
        EXPECT_TRUE(sqrt2 < sqrt2plus1);
        EXPECT_TRUE(sqrt2plus1 > sqrt2);
        TE sqrt2plus0 = sqrt2 + Rational(0);
        std::cout << sqrt2plus0 << std::endl;
        EXPECT_TRUE(sqrt2plus0 == sqrt2);
        
        std::map<Variable, TE> m = {std::make_pair(x, sqrt2)};
        std::list<TE> realRootsPoly2 = realRootsThom(poly2, y, m);
        EXPECT_EQ(realRootsPoly2.size(), 1);
        TE minus5 = realRootsPoly2.front() + Rational(-5);
        EXPECT_TRUE(minus5 < realRootsPoly2.front());
        
        std::list<TE> realRootsPoly3 = realRootsThom(poly3, x);
        EXPECT_EQ(realRootsPoly3.size(), 1);
        TE cub2 = realRootsPoly3.front();
        EXPECT_TRUE(cub2 < sqrt2);
        
        TE intermediate =  TE::intermediatePoint(cub2, sqrt2);
        EXPECT_TRUE(cub2 < intermediate && intermediate < sqrt2);
        
        std::list<TE> realRootsPoly4 = realRootsThom(poly4, y);
        EXPECT_EQ(realRootsPoly4.size(), 2);
        EXPECT_TRUE(realRootsPoly2.front() > realRootsPoly4.back());
        TE intermediate2 = TE::intermediatePoint(realRootsPoly4.back(), realRootsPoly2.front());
        EXPECT_TRUE(realRootsPoly4.back() < intermediate2 && intermediate2 < realRootsPoly2.front());
        
        Rational intermediate3 = TE::intermediatePoint(sqrt2, Rational(2));
        EXPECT_TRUE(sqrt2 < intermediate3 && intermediate3 < Rational(2));
        std::cout << intermediate3 << std::endl;
        
        
}

TEST(Thom, Evaluation) {
        typedef MultivariatePolynomial<Rational> Polynomial;
        typedef ThomEncoding<Rational> TE;
        typedef RealAlgebraicNumber<Rational> RAN;
        Variable x = freshRealVariable("x");
        Variable y = freshRealVariable("y");
        Variable z = freshRealVariable("z");
        
        Polynomial poly1({Rational(1)*x*x, Term<Rational>(Rational(-2))});      // x² - 2
        Polynomial poly2({Rational(1)*y*y, Term<Rational>(Rational(-2))});      // y² - 2
        Polynomial poly3({Rational(1)*z*z, Term<Rational>(Rational(-2))});      // z² - 2
        Polynomial poly4({Rational(1)*x*x,Rational(1)*y*y, Rational(1)*z*z, Term<Rational>(Rational(-6))});     // x² + y² +z² - 6
        Polynomial poly5({Rational(1)*x*x, Rational(1)*y*y, Term<Rational>(Rational(-2))});     // x² + y² - 2
        
        std::list<TE> realRootsPoly1 = realRootsThom(poly1, x);
        std::list<TE> realRootsPoly2 = realRootsThom(poly2, y);
        std::list<TE> realRootsPoly3 = realRootsThom(poly3, z);
        EXPECT_EQ(realRootsPoly1.size(), 2);
        EXPECT_EQ(realRootsPoly2.size(), 2);
        EXPECT_EQ(realRootsPoly3.size(), 2);
        TE sqrt2x = realRootsPoly1.back();
        TE sqrt2y = realRootsPoly2.back();
        TE sqrt2z = realRootsPoly3.back();
        
        std::map<Variable, RAN> m1 = {std::make_pair(x, RAN(sqrt2x))};
        EXPECT_TRUE(evaluateTE(poly1, m1).sgn() == Sign::ZERO);
        m1 = {std::make_pair(x, RAN(sqrt2x)), std::make_pair(y, RAN(sqrt2y))};
        EXPECT_TRUE(evaluateTE(poly1, m1).sgn() == Sign::ZERO);
        
        std::map<Variable, RAN> m2 = {std::make_pair(x, RAN(sqrt2x)), std::make_pair(y, RAN(sqrt2y)), std::make_pair(z, RAN(sqrt2z))};
        EXPECT_TRUE(evaluateTE(poly4, m2).sgn() == Sign::ZERO);
        
        std::map<Variable, TE> m3 = {std::make_pair(x, sqrt2x)};
        std::list<TE> realRootsPoly5 = realRootsThom(poly5, y, m3);
        EXPECT_EQ(realRootsPoly5.size(), 1);
        std::map<Variable, RAN> m4 = {std::make_pair(x, RAN(sqrt2x)), std::make_pair(y, RAN(realRootsPoly5.front())), std::make_pair(z, RAN(sqrt2z))};
        EXPECT_TRUE(evaluateTE(poly4, m4).sgn() == Sign::NEGATIVE);
}
