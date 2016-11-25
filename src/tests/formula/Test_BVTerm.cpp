#include "gtest/gtest.h"

#include <carl/formula/bitvector/BVTerm.h>
#include <carl/formula/bitvector/BVTermPool.h>

#include "../Common.h"

TEST(BVTerm, Construction)
{
	carl::Variable a = carl::freshBitvectorVariable("a");
	carl::Variable b = carl::freshBitvectorVariable("b");
	carl::BVValue val(8, 0);
	carl::BVTerm bvt;
	carl::BVTerm bvt_constant(carl::BVTermType::CONSTANT, val);
	
}
