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

TEST(CoCoA, Adaptor)
{
	using Poly = MultivariatePolynomial<mpq_class>;
	Variable x = freshRealVariable("x");
	Poly p1 = x*x - mpq_class(1);
	Poly p2 = (x+mpq_class(1)) * (x-mpq_class(2));
	Poly p3 = x+mpq_class(1);
	
	CoCoA::GlobalManager CoCoAFoundations;
	
	CoCoAAdaptor<Poly> c(p1, p2);
	Poly q = c.gcd(p1, p2);
	EXPECT_EQ(p3, q);
}

#endif
