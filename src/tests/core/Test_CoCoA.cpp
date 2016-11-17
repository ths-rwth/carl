#include "gtest/gtest.h"

#include "../Common.h"

#include <carl/converter/CoCoAAdaptor.h>
#include <carl/core/MultivariatePolynomial.h>

#ifdef USE_COCOA

#include "CoCoA/library.H"

using namespace carl;

TEST(CoCoA, Basic)
{
	CoCoA::GlobalManager CoCoAFoundations;
	
	auto xv = CoCoA::symbol("x");
	auto yv = CoCoA::symbol("y");
	CoCoA::ring Q = CoCoA::RingQQ();
	
	CoCoA::SparsePolyRing polyRing = CoCoA::NewPolyRing(Q, {xv,yv}, CoCoA::StdDegLex);
	auto x = CoCoA::indets(polyRing);
	
	auto p = x[0]*x[0] - 1;
	auto q = (x[0] + 1) * (x[0] - 2);
	
	auto g = CoCoA::gcd(p, q);
	EXPECT_EQ(x[0]+1, g);
}

TEST(CoCoA, GCD)
{
	using Poly = MultivariatePolynomial<mpq_class>;
	CoCoA::GlobalManager CoCoAFoundations;
	Variable x = freshRealVariable("x");
	
	Poly p1 = x*x - mpq_class(1);
	Poly p2 = (x+mpq_class(1)) * (x-mpq_class(2));
	Poly p3 = x+mpq_class(1);
	
	CoCoAAdaptor<Poly> c({p1, p2});
	Poly q = c.gcd(p1, p2);
	EXPECT_EQ(p3, q);
}

TEST(CoCoA, Factorize)
{
	using Poly = MultivariatePolynomial<mpq_class>;
	CoCoA::GlobalManager CoCoAFoundations;
	Variable x = freshRealVariable("x");
	
	Poly p1 = x*x - mpq_class(1);
	Poly p2 = (x+mpq_class(1)) * (x-mpq_class(2));
	Poly q1 = x+mpq_class(1);
	Poly q2 = x-mpq_class(1);
	Poly q3 = x-mpq_class(2);
	
	CoCoAAdaptor<Poly> c({p1, p2});
	{
		auto res = c.factorize(p1);
		auto it1 = res.find(q1);
		EXPECT_FALSE(it1 == res.end());
		EXPECT_EQ(it1->second, 1);
		auto it2 = res.find(q2);
		EXPECT_FALSE(it2 == res.end());
		EXPECT_EQ(it2->second, 1);
	}
	{
		auto res = c.factorize(p2);
		auto it1 = res.find(q1);
		EXPECT_FALSE(it1 == res.end());
		EXPECT_EQ(it1->second, 1);
		auto it2 = res.find(q3);
		EXPECT_FALSE(it2 == res.end());
		EXPECT_EQ(it2->second, 1);
	}
}
#endif
