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
