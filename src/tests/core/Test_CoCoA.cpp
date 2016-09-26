#include "gtest/gtest.h"

#include "../Common.h"

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
