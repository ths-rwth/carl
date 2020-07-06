
#include <gtest/gtest.h>

#include <carl/formula/model/ran/real_roots.h>
#include <carl/core/UnivariatePolynomial.h>
#include <carl/core/polynomialfunctions/Chebyshev.h>
#include <carl/formula/model/ran/interval/LazardEvaluation.h>

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

TEST(RootFinder, realRoots)
{
	carl::Variable x = freshRealVariable("x");
	carl::Variable y = freshRealVariable("y");

	{
		UPolynomial p(x, {(Rational)-1, (Rational)0, (Rational)0, (Rational)1});
		auto roots = carl::realRoots(p);
		EXPECT_TRUE(roots.size() == 1);
		EXPECT_TRUE(represents(roots.front(), (Rational)1));
	}
	{
		UPolynomial p(x, {(Rational)0, (Rational)-1, (Rational)0, (Rational)0, (Rational)1});
		auto roots = carl::realRoots(p);
		EXPECT_TRUE(roots.size() == 2);
	}

	{
		UMPolynomial p(x, {MPolynomial(-1), MPolynomial(0), MPolynomial(0), MPolynomial(1)});
		auto roots = carl::realRoots(p);
		EXPECT_TRUE(roots.size() == 1);
		EXPECT_TRUE(represents(roots.front(), (Rational)1));
	}

	{
		UMPolynomial p(x, {MPolynomial(-1), MPolynomial(0), MPolynomial(1)});
		auto roots = carl::realRoots(p);
		EXPECT_TRUE(roots.size() == 2);
		EXPECT_TRUE(represents(roots.front(), (Rational)-1));
		EXPECT_TRUE(represents(roots.back(), (Rational)1));
	}

	{
		UMPolynomial p(x, {MPolynomial(y), MPolynomial(0), MPolynomial(1)});
		std::map<carl::Variable, carl::RealAlgebraicNumber<Rational>> m;
		m.emplace(y, carl::RealAlgebraicNumber<Rational>(Rational(-1)));
		std::cout << "Map = " << m << std::endl;
		auto roots = carl::realRoots(p, m);
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
		auto roots = carl::realRoots(p, m);
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
	auto roots = carl::realRoots(p, m);
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
	auto roots = carl::realRoots(p, m);
	std::cout << "-> " << roots << std::endl;

	#ifdef USE_COCOA
	carl::LazardEvaluation<Rational,MPolynomial> le((MPolynomial(p)));
	le.substitute(y, yval);
	le.substitute(z, zval);
	std::cout << "Lazard: " << le.getLiftingPoly() << std::endl;
	{
		auto roots = carl::realRoots(carl::to_univariate_polynomial(le.getLiftingPoly(), x), m);
		std::cout << "-> " << roots << std::endl;
	}
	#endif
}

TEST(RootFinder, Chebyshev)
{
	carl::Chebyshev<Rational> chebyshev(freshRealVariable("x"));
	std::size_t n = 50;
	auto roots = realRoots(chebyshev(n));
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

	auto ran1 = carl::realRoots(p, carl::Interval<mpq_class>::unboundedInterval());

	std::cout << ran1 << std::endl;
}

using MPoly = carl::MultivariatePolynomial<mpq_class>;
TEST(RootFinder, FactorizationBug)
{
	carl::Variable x = carl::freshRealVariable("x");
	Poly p = Poly(x, {2,-7,7,-2});
	// 2 + -7*x + 7*x^2 + -2*x^3 < 0
	std::cout << p << std::endl;

	auto ran1 = carl::realRoots(p);
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


TEST(RootFinder, AnotherBug)
{
	carl::Variable x = carl::freshRealVariable("x");
	carl::Variable y = carl::freshRealVariable("y");
	carl::Variable r = carl::freshRealVariable("r");

	// -4*x + -2*x*y + 2*x^3 + x^3*y
	MPolynomial p = MPolynomial(Rational(-4))*x + Rational(-2)*x*y + Rational(2)*x*x*x + x*x*x*y;
	std::cout << p << std::endl;

	// x =(IR ]66148871326817845/2305843009213693952, 66148871326817845/1152921504606846976[, (10)*__r^5 + (-1)*__r^4 + (-40)*__r^3 + (3)*__r^2 + (40)*__r^1 + -2 R)
	Poly ranp(r, {-2, 40, 3,-40, -1, 10});
	Interval<Rational> rani(66148871326817845_mpq/2305843009213693952_mpq, BoundType::STRICT, 66148871326817845_mpq/1152921504606846976_mpq, BoundType::STRICT);
	auto ran = RealAlgebraicNumber<Rational>(ranp, rani);
	carl::Model<mpq_class,MPoly> m;
	m.assign(x, ran);

	// proove that y=-2 is a root
	{
		carl::Model<mpq_class,MPolynomial> m2;
		m2.assign(y, Rational(-2));
		auto f = Formula<MPolynomial>(Constraint<MPolynomial>(p, Relation::EQ));
		auto res = carl::model::evaluate(f, m2);
		EXPECT_TRUE(res.isBool());
		EXPECT_TRUE(res.asBool());
	}

	auto roots = carl::model::tryRealRoots(p, y, m);
	std::cout << roots << std::endl;
	EXPECT_TRUE(roots);
	EXPECT_TRUE(roots->size() > 0);
	EXPECT_TRUE(std::find(roots->begin(), roots->end(), RealAlgebraicNumber<Rational>(Rational(-2))) != roots->end());
}