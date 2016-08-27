#include "gtest/gtest.h"
#include "../Common.h"

#include "carl/core/VariablePool.h"
#include "carl/util/stringparser.h"

#include "carl/thom2/SignDetermination/SignDetermination.h"
#include "carl/thom2/ThomRootFinder.h"

using namespace carl;

TEST(Thom2, SignDetermination) {
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
        std::cout << "sd.getSigns(MPolynomial(p3)): " << sd.getSigns(MPolynomial(p3)) << std::endl;;
        std::cout << "sd.getSignsAndAdd(MPolynomial(p3)): " << sd.getSignsAndAdd(MPolynomial(p3)) << std::endl; 
        std::cout <<  "sd.getSigns(MPolynomial(p2)): " << sd.getSigns(MPolynomial(p2)) << std::endl;
        std::cout << "sd.getSignsAndAdd(MPolynomial(p2)): " << sd.getSignsAndAdd(MPolynomial(p2)) << std::endl;
        std::cout << "sd.getSigns(MPolynomial(p1)): " << sd.getSigns(MPolynomial(p1)) << std::endl;
        std::cout << "sd.getSignsAndAdd(MPolynomial(p1)): " << sd.getSignsAndAdd(MPolynomial(p1)) << std::endl;
        
        std::cout << "\n\n\n" << std::endl;
        
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
        
        std::cout << "\n\n\n" << std::endl;
        
        Variable y = freshRealVariable("y");
        UPolynomial f2(x, {(Rational)-2, (Rational)1});
        MPolynomial fourZeros(Rational(1));
        for(int i = 0; i < 4; i++) {
                f2 = f2 + Rational(1);
                fourZeros = fourZeros * MPolynomial(f2); 
        }
        
        MPolynomial ellipse({Rational(8)*x*x, Rational(9)*y*y, Rational(-8)*x, Rational(-36)*y, Term<Rational>(Rational(20))});
        zeroSet = {ellipse, fourZeros};
        SignDetermination<Rational> sd3(zeroSet.begin(), zeroSet.end());
        
        sd3.getSignsAndAdd(fourZeros.derivative(x).derivative(x).derivative(x).derivative(x));
        sd3.getSignsAndAdd(fourZeros.derivative(x).derivative(x).derivative(x));
        sd3.getSignsAndAdd(fourZeros.derivative(x).derivative(x));
        sd3.getSignsAndAdd(fourZeros.derivative(x));
        sd3.getSignsAndAdd(ellipse.derivative(y).derivative(y));
        sd3.getSignsAndAdd(ellipse.derivative(y));
        
}

/*
TEST(Thom2, RootFinder) {
        typedef MultivariatePolynomial<Rational> Polynomial;
        typedef ThomEncoding<Rational> TE;
        Variable x = freshRealVariable("x");
        Variable y = freshRealVariable("y");
        Variable z = freshRealVariable("z");
        
        Polynomial poly1({Rational(1)*x*x*x*x*x, Rational(-1)*x*x*x, Rational(1)*x*x, Rational(-2)*x, Term<Rational>(Rational(-2))});
        //realRootsThom(poly1);
        
        
        Polynomial poly2({Rational(1)*x*x*x*x*x, Rational(-1)*x});
        std::list<TE> realRootsPoly2 = realRootsThom(poly2);
        std::cout << "real roots of poly2: " << realRootsPoly2 << std::endl;
        
        
        //x^2-0.8 x+y^2-0.2
        Polynomial poly3({Rational(5)*x*x, Rational(-4)*x, Rational(5)*y*y, Term<Rational>(Rational(-1))});
        std::map<Variable, TE> m = {std::make_pair(x, realRootsPoly2.back())};
        std::list<TE> realRootsPoly3 = realRootsThom(poly3, m);
        EXPECT_TRUE(realRootsPoly3.size() == 1);
        std::cout << "real roots of poly3: " << realRootsPoly3 << std::endl;
        
        Polynomial poly4({Rational(1)*x, Rational(1)*y, Rational(1)*z});
        std::map<Variable, TE> m_xy = {std::make_pair(x, realRootsPoly2.back()),
                                        std::make_pair(y, realRootsPoly3.front())};
        std::list<TE> realRootsPoly4 = realRootsThom(poly4, m_xy);
        std::cout << "real roots of poly4: " << realRootsPoly4 << std::endl;
        EXPECT_TRUE(realRootsPoly4.size() == 1);
        
}

TEST(Thom2, Comparison) {
        typedef MultivariatePolynomial<Rational> Polynomial;
        typedef ThomEncoding<Rational> TE;
        Variable x = freshRealVariable("x");
        
        Polynomial poly1({Rational(1)*x*x*x*x, Rational(-3)*x*x*x, Rational(1)*x, Term<Rational>(Rational(-2))});
        Polynomial poly2({Rational(1)*x*x, Term<Rational>(Rational(-2))});
        
        std::list<TE> realRootsPoly1 = realRootsThom(poly1);
        std::list<TE> realRootsPoly2 = realRootsThom(poly2);
        
        std::cout << "realRootsPoly1 = " << realRootsPoly1 << std::endl;
        std::cout << "realRootsPoly2 = " << realRootsPoly2 << std::endl;
        
        compare<Rational>(realRootsPoly1.front(), realRootsPoly2.front()); // rhs is greater than lhs here
        
        
        compare<Rational>(realRootsPoly1.back(), realRootsPoly2.front()); // lhs is greater than rhs here
        
        compare<Rational>(realRootsPoly1.back(), realRootsPoly2.back());
}
 */