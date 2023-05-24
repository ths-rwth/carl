#include "gtest/gtest.h"

#include "../Common.h"

#ifdef USE_COCOA

#include "CoCoA/library.H"

CoCoA::RingElem operator^(CoCoA::RingElem e, int n) {
	CoCoA::RingElem res = e;
	for (int i=1; i<n; i++) res = res * e;
	return res;
}

TEST(CoCoA, Bug) {
	auto xv = CoCoA::symbol("x");
	auto yv = CoCoA::symbol("y");
	CoCoA::ring Q = CoCoA::RingQQ();
	CoCoA::SparsePolyRing polyRing = CoCoA::NewPolyRing(Q, {yv, xv});
	auto x = CoCoA::indets(polyRing);
	auto r4 = x[1];
	auto r5 = x[0];

	auto p0 = 2*(r4^3) +9*(r4^2)*r5 +9*r4*(r5^2) +2*(r5^3) -24*(r4^2) -48*r4*r5 -24*(r5^2) +60*r4 +60*r5 -32;
	auto p1 = 1000*(r4^9) +1800*(r4^8)*r5 +3366*(r4^7)*(r5^2) +7806*(r4^6)*(r5^3) +10188*(r4^5)*(r5^4) +10188*(r4^4)*(r5^5) +7806*(r4^3)*(r5^6) +3366*(r4^2)*(r5^7) +1800*r4*(r5^8) +1000*(r5^9) -21600*(r4^7) -57888*(r4^6)*r5 -95904*(r4^5)*(r5^2) -149472*(r4^4)*(r5^3 )-149472*(r4^3)*(r5^4) -95904*(r4^2)*(r5^5) -57888*r4*(r5^6) -21600*(r5^7) +171936*(r4^5) +527904*(r4^4)*r5 +724032*(r4^3)*(r5^2) +724032*(r4^2)*(r5^3) +527904*r4*(r5^4) +171936*(r5^5) -594432*(r4^3) -1596672*(r4^2)*r5 -1596672*r4*(r5^2) -594432*(r5^3) +746496*r4 +746496*r5;
	
	// wrong result of factor(p1) after call of factor(p0)
	CoCoA::factor(p0);
	auto g1 = CoCoA::factor(p1);

	// correct result
	auto g2 = CoCoA::factor(p1);

	std::cout << g1 << std::endl;
	std::cout << g2 << std::endl;

	ASSERT_EQ(g1.myFactors().size(),g2.myFactors().size());
}


#endif
