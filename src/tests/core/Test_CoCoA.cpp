#include "gtest/gtest.h"

#include "../Common.h"

#include "CoCoA/library.H"

using namespace carl;

TEST(CoCoA, Basic)
{
	CoCoA::GlobalManager CoCoAFoundations;
	
	CoCoA::ring Q = CoCoA::RingQQ();
	
	CoCoA::SparsePolyRing polyRing = CoCoA::NewPolyRing(Q, CoCoA::SymbolRange("x",0,0), CoCoA::StdDegLex);
	
	
}
