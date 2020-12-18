
#include <gtest/gtest.h>

#include <carl/ran/real_roots.h>
#include <carl/core/UnivariatePolynomial.h>
#include <carl/core/polynomialfunctions/Chebyshev.h>
#include <carl/ran/interval/LazardEvaluation.h>

#include <carl/formula/Formula.h>
#include <carl-model/Model.h>
#include <carl-model/evaluation/ModelEvaluation.h>

#include <boost/optional/optional_io.hpp>

#include "../Common.h"

typedef carl::UnivariatePolynomial<Rational> UPolynomial;
typedef carl::MultivariatePolynomial<Rational> MPolynomial;
typedef carl::UnivariatePolynomial<MPolynomial> UMPolynomial;

using namespace carl;

template<typename Number>
bool represents(const carl::RealAlgebraicNumber<Number> root, const Number& exact) {
	if (root.is_numeric()) {
		return root.value() == exact;
	} else {
		return root.interval().contains(exact);
	}
}

TEST(RootFinder, real_roots)
{
	carl::Variable x = freshRealVariable("x");
	carl::Variable y = freshRealVariable("y");

	{
		UPolynomial p(x, {(Rational)-1, (Rational)0, (Rational)0, (Rational)1});
		auto roots = carl::real_roots(p).roots();
		EXPECT_TRUE(roots.size() == 1);
		EXPECT_TRUE(represents(roots.front(), (Rational)1));
	}
	{
		UPolynomial p(x, {(Rational)0, (Rational)-1, (Rational)0, (Rational)0, (Rational)1});
		auto roots = carl::real_roots(p).roots();
		EXPECT_TRUE(roots.size() == 2);
	}

	{
		UMPolynomial p(x, {MPolynomial(-1), MPolynomial(0), MPolynomial(0), MPolynomial(1)});
		auto roots = carl::real_roots(p).roots();
		EXPECT_TRUE(roots.size() == 1);
		EXPECT_TRUE(represents(roots.front(), (Rational)1));
	}

	{
		UMPolynomial p(x, {MPolynomial(-1), MPolynomial(0), MPolynomial(1)});
		auto roots = carl::real_roots(p);
		EXPECT_TRUE(roots.roots().size() == 2);
		EXPECT_TRUE(represents(roots.roots().front(), (Rational)-1));
		EXPECT_TRUE(represents(roots.roots().back(), (Rational)1));
	}

	{
		UMPolynomial p(x, {MPolynomial(y), MPolynomial(0), MPolynomial(1)});
		std::map<carl::Variable, carl::RealAlgebraicNumber<Rational>> m;
		m.emplace(y, carl::RealAlgebraicNumber<Rational>(Rational(-1)));
		std::cout << "Map = " << m << std::endl;
		auto roots = carl::real_roots(p, m);
		EXPECT_TRUE(roots.roots().size() == 2);
		EXPECT_TRUE(represents(roots.roots().front(), (Rational)-1));
		EXPECT_TRUE(represents(roots.roots().back(), (Rational)1));
	}
	{
		MPolynomial c1 = (y + (Rational)(1));
		MPolynomial c2 = (y + (Rational)(1))* (Rational)(-2);
		UMPolynomial p(x, {c2,c1});
		std::map<carl::Variable, carl::RealAlgebraicNumber<Rational>> m;
		m.emplace(y, carl::RealAlgebraicNumber<Rational>(Rational(-1)));
		auto roots = carl::real_roots(p, m);
		EXPECT_TRUE(roots.is_nullified());
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
	auto roots = carl::real_roots(p, m);
	EXPECT_TRUE(roots.roots().size() == 1);
	EXPECT_TRUE(roots.roots().front() == xval);
}

TEST(RootFinder, Chebyshev)
{
	carl::Chebyshev<Rational> chebyshev(freshRealVariable("x"));
	std::size_t n = 50;
	auto roots = real_roots(chebyshev(n)).roots();
	EXPECT_TRUE(roots.size() == n);
	carl::RealAlgebraicNumber<Rational> mone(Rational(-1));
	carl::RealAlgebraicNumber<Rational> pone(Rational(1));
	for (const auto& r: roots) {
		EXPECT_TRUE(mone <= r && r <= pone);
	}
}

using Poly = carl::UnivariatePolynomial<mpq_class>;
TEST(RootFinder, Comparison)
{
	carl::Variable x = carl::freshRealVariable("x");
	Poly p = Poly(x, {-66864570625487788604261524190527488000000000000000000000000000000000000000000000000_mpq, 29854830731431634049898015056382132224000000000000000000000000000000000000000000000_mpq,
	-505416087363206530647584374267211415552000000000000000000000000000000000000000000_mpq, 4175633824067780996197090619870479908864000000000000000000000000000000000000000_mpq,
	-22123075889900509025587890607637359755264000000000000000000000000000000000000_mpq, 84105767440004379479396048598450604867584000000000000000000000000000000000_mpq,
	-243736416758800137976000425396107778981888000000000000000000000000000000_mpq, 557927276756885723844248187821195727470592000000000000000000000000000_mpq,
	-1028655240376238080386120074233535281496064000000000000000000000000_mpq,
	1537205882314544520383371309153190311624704000000000000000000000_mpq, -1850245296283620589500378012241541801705472000000000000000000_mpq,
	1759094245770532463939207746344508321431552000000000000000_mpq, -1274862311787402732031743433553883636424704000000000000_mpq,
	662971140168469203991742914981670449102848000000000_mpq, -221093370976852468477758191983112847898368000000_mpq,
	35982863100180637437263547995778211974592000_mpq, -222040198421806427031529186994264976123_mpq});

	auto ran1 = carl::real_roots(p, carl::Interval<mpq_class>::unboundedInterval()).roots();

	std::cout << ran1 << std::endl;
}

using MPoly = carl::MultivariatePolynomial<mpq_class>;
TEST(RootFinder, FactorizationBug)
{
	carl::Variable x = carl::freshRealVariable("x");
	Poly p = Poly(x, {2,-7,7,-2});
	// 2 + -7*x + 7*x^2 + -2*x^3 < 0
	std::cout << p << std::endl;

	auto ran1 = carl::real_roots(p).roots();
	std::cout << ran1 << std::endl;
	// should be: 1/2, 1, 2

	for (const auto& root: ran1) {
		carl::Model<mpq_class,MPoly> m;
		m.assign(x, root);
		auto f = Formula<MPoly>(Constraint<MPoly>(MPoly(p), Relation::EQ));
		auto res = carl::model::evaluate(f, m);
		EXPECT_TRUE(res.asBool());
	}
}