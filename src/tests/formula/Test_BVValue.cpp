#include <carl/formula/bitvector/BVValue.h>
#include <gtest/gtest.h>

#include "../get_output.h"
#include "../number_types.h"
#include "../util.h"

using BDB = carl::BVValue::Base;

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
	EXPECT_EQ(BDB(32, 0), BDB(this->bv32_0));
	EXPECT_EQ(BDB(32, 1), BDB(this->bv32_1));
	EXPECT_EQ(BDB(32, 1073741823), BDB(this->bv32_e30));
}

TYPED_TEST(BVValueTest, width)
{
	EXPECT_EQ(32, this->bv32_0.width());
	EXPECT_EQ(32, this->bv32_1.width());
	EXPECT_EQ(32, this->bv32_e30.width());	
}

TYPED_TEST(BVValueTest, toString)
{
	EXPECT_EQ("#b00000000000000000000000000000000", this->bv32_0.toString());
	EXPECT_EQ("#b00000000000000000000000000000000", get_output(this->bv32_0));
	EXPECT_EQ("#b00000000000000000000000000000001", this->bv32_1.toString());
	EXPECT_EQ("#b00000000000000000000000000000001", get_output(this->bv32_1));
	EXPECT_EQ("#b00111111111111111111111111111111", this->bv32_e30.toString());
	EXPECT_EQ("#b00111111111111111111111111111111", get_output(this->bv32_e30));
}

TYPED_TEST(BVValueTest, isZero)
{
	EXPECT_TRUE(this->bv32_0.isZero());
	EXPECT_FALSE(this->bv32_1.isZero());
	EXPECT_FALSE(this->bv32_e30.isZero());
}

TYPED_TEST(BVValueTest, UnaryMinus)
{
	EXPECT_EQ(carl::BVValue(32, 0), -this->bv32_0);
	EXPECT_EQ(carl::BVValue(32, -1), -this->bv32_1);
	EXPECT_EQ(carl::BVValue(32, -1073741823), -this->bv32_e30);
}

TYPED_TEST(BVValueTest, BinaryInversion)
{
	EXPECT_EQ(carl::BVValue(32, 4294967295), ~this->bv32_0);
	EXPECT_EQ(carl::BVValue(32, 4294967294), ~this->bv32_1);
	EXPECT_EQ(carl::BVValue(32, 3221225472), ~this->bv32_e30);
}

TYPED_TEST(BVValueTest, rotateLeft)
{
	EXPECT_EQ(carl::BVValue(32, 0), this->bv32_0.rotateLeft(1));
	EXPECT_EQ(carl::BVValue(32, 0), this->bv32_0.rotateLeft(5));
	EXPECT_EQ(carl::BVValue(32, 2), this->bv32_1.rotateLeft(1));
	EXPECT_EQ(carl::BVValue(32, 32), this->bv32_1.rotateLeft(5));
	EXPECT_EQ(carl::BVValue(32, 2147483646), this->bv32_e30.rotateLeft(1));
	EXPECT_EQ(carl::BVValue(32, 4294967271), this->bv32_e30.rotateLeft(5));
}

TYPED_TEST(BVValueTest, rotateRight)
{
	EXPECT_EQ(carl::BVValue(32, 0), this->bv32_0.rotateRight(1));
	EXPECT_EQ(carl::BVValue(32, 0), this->bv32_0.rotateRight(5));
	EXPECT_EQ(carl::BVValue(32, 2147483648), this->bv32_1.rotateRight(1));
	EXPECT_EQ(carl::BVValue(32, 134217728), this->bv32_1.rotateRight(5));
	EXPECT_EQ(carl::BVValue(32, 2684354559), this->bv32_e30.rotateRight(1));
	EXPECT_EQ(carl::BVValue(32, 4194303999), this->bv32_e30.rotateRight(5));
}

TYPED_TEST(BVValueTest, repeat)
{
	EXPECT_EQ(this->bv32_0, this->bv32_0.repeat(1));
	EXPECT_EQ(carl::BVValue(64, 0), this->bv32_0.repeat(2));
	EXPECT_EQ(this->bv32_1, this->bv32_1.repeat(1));
	EXPECT_EQ(carl::BVValue(64, 4294967297), this->bv32_1.repeat(2));
	EXPECT_EQ(this->bv32_e30, this->bv32_e30.repeat(1));
	EXPECT_EQ(carl::BVValue(64, 4611686015206162431), this->bv32_e30.repeat(2));
}

TYPED_TEST(BVValueTest, extendUnsignedBy)
{
	EXPECT_EQ(this->bv32_0, this->bv32_0.extendUnsignedBy(0));
	EXPECT_EQ(carl::BVValue(34, 0), this->bv32_0.extendUnsignedBy(2));
	EXPECT_EQ(this->bv32_1, this->bv32_1.extendUnsignedBy(0));
	EXPECT_EQ(carl::BVValue(34, 1), this->bv32_1.extendUnsignedBy(2));
	EXPECT_EQ(this->bv32_e30, this->bv32_e30.extendUnsignedBy(0));
	EXPECT_EQ(carl::BVValue(34, 1073741823), this->bv32_e30.extendUnsignedBy(2));
}

TYPED_TEST(BVValueTest, extendSignedBy)
{
	EXPECT_EQ(this->bv32_0, this->bv32_0.extendSignedBy(0));
	EXPECT_EQ(carl::BVValue(34, 0), this->bv32_0.extendSignedBy(2));
	EXPECT_EQ(this->bv32_1, this->bv32_1.extendSignedBy(0));
	EXPECT_EQ(carl::BVValue(34, 1), this->bv32_1.extendSignedBy(2));
	EXPECT_EQ(this->bv32_e30, this->bv32_e30.extendSignedBy(0));
	EXPECT_EQ(carl::BVValue(34, 1073741823), this->bv32_e30.extendSignedBy(2));
}

TYPED_TEST(BVValueTest, Comparison)
{
	checkEqual({this->bv32_0, this->bv32_1, this->bv32_e30});
	checkLess({this->bv32_0, this->bv32_1, this->bv32_e30});
}

TYPED_TEST(BVValueTest, Addition)
{
	EXPECT_EQ(carl::BVValue(32, 0), this->bv32_0 + this->bv32_0);
	EXPECT_EQ(carl::BVValue(32, 1), this->bv32_0 + this->bv32_1);
	EXPECT_EQ(carl::BVValue(32, 1073741823), this->bv32_0 + this->bv32_e30);
	EXPECT_EQ(carl::BVValue(32, 1), this->bv32_1 + this->bv32_0);
	EXPECT_EQ(carl::BVValue(32, 2), this->bv32_1 + this->bv32_1);
	EXPECT_EQ(carl::BVValue(32, 1073741824), this->bv32_1 + this->bv32_e30);
	EXPECT_EQ(carl::BVValue(32, 1073741823), this->bv32_e30 + this->bv32_0);
	EXPECT_EQ(carl::BVValue(32, 1073741824), this->bv32_e30 + this->bv32_1);
	EXPECT_EQ(carl::BVValue(32, 2147483646), this->bv32_e30 + this->bv32_e30);
}

TYPED_TEST(BVValueTest, Subtraction)
{
	EXPECT_EQ(carl::BVValue(32, 0), this->bv32_0 - this->bv32_0);
	EXPECT_EQ(carl::BVValue(32, -1), this->bv32_0 - this->bv32_1);
	EXPECT_EQ(carl::BVValue(32, -1073741823), this->bv32_0 - this->bv32_e30);
	EXPECT_EQ(carl::BVValue(32, 1), this->bv32_1 - this->bv32_0);
	EXPECT_EQ(carl::BVValue(32, 0), this->bv32_1 - this->bv32_1);
	EXPECT_EQ(carl::BVValue(32, -1073741822), this->bv32_1 - this->bv32_e30);
	EXPECT_EQ(carl::BVValue(32, 1073741823), this->bv32_e30 - this->bv32_0);
	EXPECT_EQ(carl::BVValue(32, 1073741822), this->bv32_e30 - this->bv32_1);
	EXPECT_EQ(carl::BVValue(32, 0), this->bv32_e30 - this->bv32_e30);
}

TYPED_TEST(BVValueTest, BitwiseAnd)
{
	EXPECT_EQ(carl::BVValue(32, 0), this->bv32_0 & this->bv32_0);
	EXPECT_EQ(carl::BVValue(32, 0), this->bv32_0 & this->bv32_1);
	EXPECT_EQ(carl::BVValue(32, 0), this->bv32_0 & this->bv32_e30);
	EXPECT_EQ(carl::BVValue(32, 0), this->bv32_1 & this->bv32_0);
	EXPECT_EQ(carl::BVValue(32, 1), this->bv32_1 & this->bv32_1);
	EXPECT_EQ(carl::BVValue(32, 1), this->bv32_1 & this->bv32_e30);
	EXPECT_EQ(carl::BVValue(32, 0), this->bv32_e30 & this->bv32_0);
	EXPECT_EQ(carl::BVValue(32, 1), this->bv32_e30 & this->bv32_1);
	EXPECT_EQ(carl::BVValue(32, 1073741823), this->bv32_e30 & this->bv32_e30);
}

TYPED_TEST(BVValueTest, BitwiseOr)
{
	EXPECT_EQ(carl::BVValue(32, 0), this->bv32_0 | this->bv32_0);
	EXPECT_EQ(carl::BVValue(32, 1), this->bv32_0 | this->bv32_1);
	EXPECT_EQ(carl::BVValue(32, 1073741823), this->bv32_0 | this->bv32_e30);
	EXPECT_EQ(carl::BVValue(32, 1), this->bv32_1 | this->bv32_0);
	EXPECT_EQ(carl::BVValue(32, 1), this->bv32_1 | this->bv32_1);
	EXPECT_EQ(carl::BVValue(32, 1073741823), this->bv32_1 | this->bv32_e30);
	EXPECT_EQ(carl::BVValue(32, 1073741823), this->bv32_e30 | this->bv32_0);
	EXPECT_EQ(carl::BVValue(32, 1073741823), this->bv32_e30 | this->bv32_1);
	EXPECT_EQ(carl::BVValue(32, 1073741823), this->bv32_e30 | this->bv32_e30);
}

TYPED_TEST(BVValueTest, BitwiseXor)
{
	EXPECT_EQ(carl::BVValue(32, 0), this->bv32_0 ^ this->bv32_0);
	EXPECT_EQ(carl::BVValue(32, 1), this->bv32_0 ^ this->bv32_1);
	EXPECT_EQ(carl::BVValue(32, 1073741823), this->bv32_0 ^ this->bv32_e30);
	EXPECT_EQ(carl::BVValue(32, 1), this->bv32_1 ^ this->bv32_0);
	EXPECT_EQ(carl::BVValue(32, 0), this->bv32_1 ^ this->bv32_1);
	EXPECT_EQ(carl::BVValue(32, 1073741822), this->bv32_1 ^ this->bv32_e30);
	EXPECT_EQ(carl::BVValue(32, 1073741823), this->bv32_e30 ^ this->bv32_0);
	EXPECT_EQ(carl::BVValue(32, 1073741822), this->bv32_e30 ^ this->bv32_1);
	EXPECT_EQ(carl::BVValue(32, 0), this->bv32_e30 ^ this->bv32_e30);
}

TYPED_TEST(BVValueTest, concat)
{
	EXPECT_EQ(carl::BVValue(64, 0), this->bv32_0.concat(this->bv32_0));
	EXPECT_EQ(carl::BVValue(64, 1), this->bv32_0.concat(this->bv32_1));
	EXPECT_EQ(carl::BVValue(64, 1073741823), this->bv32_0.concat(this->bv32_e30));
	EXPECT_EQ(carl::BVValue(64, 4294967296), this->bv32_1.concat(this->bv32_0));
	EXPECT_EQ(carl::BVValue(64, 4294967297), this->bv32_1.concat(this->bv32_1));
	EXPECT_EQ(carl::BVValue(64, 5368709119), this->bv32_1.concat(this->bv32_e30));
	EXPECT_EQ(carl::BVValue(64, 4611686014132420608), this->bv32_e30.concat(this->bv32_0));
	EXPECT_EQ(carl::BVValue(64, 4611686014132420609), this->bv32_e30.concat(this->bv32_1));
	EXPECT_EQ(carl::BVValue(64, 4611686015206162431), this->bv32_e30.concat(this->bv32_e30));
}

TYPED_TEST(BVValueTest, Multiplication)
{
	EXPECT_EQ(carl::BVValue(32, 0), this->bv32_0 * this->bv32_0);
	EXPECT_EQ(carl::BVValue(32, 0), this->bv32_0 * this->bv32_1);
	EXPECT_EQ(carl::BVValue(32, 0), this->bv32_0 * this->bv32_e30);
	EXPECT_EQ(carl::BVValue(32, 0), this->bv32_1 * this->bv32_0);
	EXPECT_EQ(carl::BVValue(32, 1), this->bv32_1 * this->bv32_1);
	EXPECT_EQ(carl::BVValue(32, 1073741823), this->bv32_1 * this->bv32_e30);
	EXPECT_EQ(carl::BVValue(32, 0), this->bv32_e30 * this->bv32_0);
	EXPECT_EQ(carl::BVValue(32, 1073741823), this->bv32_e30 * this->bv32_1);
	EXPECT_EQ(carl::BVValue(32, 2147483649), this->bv32_e30 * this->bv32_e30);
}
