#include "gtest/gtest.h"

#include "carl/core/rootfinder/RootFinder.h"
#include "carl/core/UnivariatePolynomial.h"

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
		EXPECT_TRUE(bool(roots));
		EXPECT_TRUE(roots->size() == 2);
		EXPECT_TRUE(represents(roots->front(), (Rational)-1));
		EXPECT_TRUE(represents(roots->back(), (Rational)1));
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
	EXPECT_TRUE(bool(roots));
	std::cout << *roots << std::endl;
	EXPECT_TRUE(roots->size() == 1);
	EXPECT_TRUE(roots->front() == xval);
}
