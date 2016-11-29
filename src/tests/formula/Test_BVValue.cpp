#include "gtest/gtest.h"

#include <carl/formula/bitvector/BVValue.h>

#include <boost/dynamic_bitset.hpp>

#include "../Common.h"

using BDB = boost::dynamic_bitset<>;

template<typename T>
class BVValueTest: public testing::Test {
protected:
	
	carl::BVValue bv;
	carl::BVValue bv32_0;
	carl::BVValue bv32_1;
	carl::BVValue bv32_e30;
	
	BVValueTest():
		bv32_0(32, T(0)),
		bv32_1(32, T(1)),
		bv32_e30(32, T((1 << 30) - 1))
	{}
};

TYPED_TEST_CASE(BVValueTest, IntegerTypes);


TYPED_TEST(BVValueTest, Constructor)
{
	BDB bdb_0(32, 0);
	BDB bdb_1(32, 1);
	BDB bdb_e30(32, (1 << 30) - 1);
	EXPECT_EQ(bdb_0, BDB(this->bv32_0));
	EXPECT_EQ(bdb_1, BDB(this->bv32_1));
	EXPECT_EQ(bdb_e30, BDB(this->bv32_e30));
	
	EXPECT_EQ(32, this->bv32_0.width());
	EXPECT_EQ(32, this->bv32_1.width());
	EXPECT_EQ(32, this->bv32_e30.width());
	
	EXPECT_TRUE(this->bv32_0.isZero());
	EXPECT_FALSE(this->bv32_1.isZero());
	EXPECT_FALSE(this->bv32_e30.isZero());
}
