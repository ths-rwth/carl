
#include <gtest/gtest.h>

#include <carl/core/rootfinder/RootFinder.h>
#include <carl/core/UnivariatePolynomial.h>
#include <carl/core/polynomialfunctions/Chebyshev.h>
#include "carl/core/polynomialfunctions/LazardEvaluation.h"

#include "../Common.h"

typedef carl::UnivariatePolynomial<Rational> UPolynomial;
typedef carl::MultivariatePolynomial<Rational> MPolynomial;
typedef carl::UnivariatePolynomial<MPolynomial> UMPolynomial;

using namespace carl;

template<typename Number>
bool represents(const carl::RealAlgebraicNumber<Number> root, const Number& exact) {
	if (root.isNumeric()) {
		return root.value() == exact;
	} else {
		return root.getInterval().contains(exact);
	}
}

TEST(RootFinder, realRoots)
{
	carl::Variable x = freshRealVariable("x");
	carl::Variable y = freshRealVariable("y");

	{
		UPolynomial p(x, {(Rational)-1, (Rational)0, (Rational)0, (Rational)1});
		auto roots = carl::rootfinder::realRoots(p);
		EXPECT_TRUE(roots.size() == 1);
		EXPECT_TRUE(represents(roots.front(), (Rational)1));
	}
	{
		UPolynomial p(x, {(Rational)0, (Rational)-1, (Rational)0, (Rational)0, (Rational)1});
		auto roots = carl::rootfinder::realRoots(p);
		EXPECT_TRUE(roots.size() == 2);
	}

	{
		UMPolynomial p(x, {MPolynomial(-1), MPolynomial(0), MPolynomial(0), MPolynomial(1)});
		auto roots = carl::rootfinder::realRoots(p);
		EXPECT_TRUE(roots.size() == 1);
		EXPECT_TRUE(represents(roots.front(), (Rational)1));
	}

	{
		UMPolynomial p(x, {MPolynomial(-1), MPolynomial(0), MPolynomial(1)});
		auto roots = carl::rootfinder::realRoots(p);
		EXPECT_TRUE(roots.size() == 2);
		EXPECT_TRUE(represents(roots.front(), (Rational)-1));
		EXPECT_TRUE(represents(roots.back(), (Rational)1));
	}

	{
		UMPolynomial p(x, {MPolynomial(y), MPolynomial(0), MPolynomial(1)});
		std::map<carl::Variable, carl::RealAlgebraicNumber<Rational>> m;
		m.emplace(y, carl::RealAlgebraicNumber<Rational>(Rational(-1)));
		std::cout << "Map = " << m << std::endl;
		auto roots = carl::rootfinder::realRoots(p, m);
		EXPECT_TRUE(roots.size() == 2);
		EXPECT_TRUE(represents(roots.front(), (Rational)-1));
		EXPECT_TRUE(represents(roots.back(), (Rational)1));
	}
	{
		MPolynomial c1 = (y + (Rational)(1));
		MPolynomial c2 = (y + (Rational)(1))* (Rational)(-2);
		UMPolynomial p(x, {c2,c1});
		std::map<carl::Variable, carl::RealAlgebraicNumber<Rational>> m;
		m.emplace(y, carl::RealAlgebraicNumber<Rational>(Rational(-1)));
		auto roots = carl::rootfinder::realRoots(p, m);
		EXPECT_TRUE(roots.empty());
	}
}

TEST(RootFinder, evalRoots)
{
	carl::Variable x = freshRealVariable("x");
	carl::Variable y = freshRealVariable("y");
	
	UPolynomial xpoly(x, {-2, 0, 1});
	carl::Interval<Rational> xint(Rational(5)/4, carl::BoundType::STRICT, Rational(3)/2, carl::BoundType::STRICT);
	carl::RealAlgebraicNumber<Rational> xval(xpoly, xint);
	
	UMPolynomial p = UMPolynomial(y, {MPolynomial(x), MPolynomial(-1)});
	std::map<carl::Variable, carl::RealAlgebraicNumber<Rational>> m;
	m.emplace(x, xval);
	auto roots = carl::rootfinder::realRoots(p, m);
	EXPECT_TRUE(roots.size() == 1);
	EXPECT_TRUE(roots.front() == xval);
}

TEST(RootFinder, tryRealRoots)
{
	carl::Variable x = freshRealVariable("x");
	carl::Variable y = freshRealVariable("y");
	carl::Variable z = freshRealVariable("z");

	//UMPolynomial p(x, { MPolynomial(0), -MPolynomial(2)*MPolynomial(y)-z });
	UMPolynomial p(x, { MPolynomial(0), MPolynomial(y)+z });

	UPolynomial ypoly(y, {-2, 0, 1});
	carl::Interval<Rational> yint(Rational(1), carl::BoundType::STRICT, Rational(2), carl::BoundType::STRICT);
	carl::RealAlgebraicNumber<Rational> yval(ypoly, yint);

	//UPolynomial zpoly(y, {-32, 0, 1});
	//carl::Interval<Rational> zint(Rational(38)/12, carl::BoundType::STRICT, Rational(10)/3, carl::BoundType::STRICT);
	//carl::RealAlgebraicNumber<Rational> zval(zpoly, zint);
	UPolynomial zpoly(y, {-2, 0, 1});
	carl::Interval<Rational> zint(Rational(1), carl::BoundType::STRICT, Rational(2), carl::BoundType::STRICT);
	carl::RealAlgebraicNumber<Rational> zval(zpoly, zint);

	{
		UPolynomial py(y, { -8, 0, 3 });
		UPolynomial pz(z, { -8, 0, 3 });

		//yval = carl::realRootsZ3(py, Interval<Rational>::unboundedInterval())[1];
		//zval = carl::realRootsZ3(pz, Interval<Rational>::unboundedInterval())[1];
	}

    //-1*x*y + -1*x*z < 0
	//{
	//	y = (IR ]209/128, 2509/1536[, (3)*__r^2 + -8 R),
	//	z = (IR ]19/12, 5/3[, __r^2 + -8/3 R)
	//}

	std::map<carl::Variable, carl::RealAlgebraicNumber<Rational>> m = {
		{ y, yval }, {z, zval }
	};
	std::cout << "y = z? " << (yval == zval) << std::endl;
	std::cout << p << std::endl;
	std::cout << m << std::endl;
	auto roots = carl::rootfinder::realRoots(p, m);
	std::cout << "-> " << roots << std::endl;

	#ifdef USE_COCOA
	carl::LazardEvaluation<Rational,MPolynomial> le((MPolynomial(p)));
	le.substitute(y, yval);
	le.substitute(z, zval);
	std::cout << "Lazard: " << le.getLiftingPoly() << std::endl;
	{
		auto roots = carl::rootfinder::realRoots(le.getLiftingPoly().toUnivariatePolynomial(x), m);
		std::cout << "-> " << roots << std::endl;
	}
	#endif
}

TEST(RootFinder, Chebyshev)
{
	carl::Chebyshev<Rational> chebyshev(freshRealVariable("x"));
	std::size_t n = 50;
	auto roots = rootfinder::realRoots(chebyshev(n));
	EXPECT_TRUE(roots.size() == n);
	carl::RealAlgebraicNumber<Rational> mone(Rational(-1));
	carl::RealAlgebraicNumber<Rational> pone(Rational(1));
	for (const auto& r: roots) {
		EXPECT_TRUE(mone <= r && r <= pone);
	}
}
