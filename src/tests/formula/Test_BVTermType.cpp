#include "gtest/gtest.h"

#include <carl/formula/bitvector/BVTerm.h>
#include <carl/formula/bitvector/BVTermPool.h>

#include "../Common.h"

TEST(BVTermType, Output)
{
	EXPECT_TRUE(getOutput(carl::BVTermType::CONSTANT) == "CONSTANT");
	EXPECT_TRUE(getOutput(carl::BVTermType::VARIABLE) == "VARIABLE");
	EXPECT_TRUE(getOutput(carl::BVTermType::CONCAT) == "concat");
	EXPECT_TRUE(getOutput(carl::BVTermType::EXTRACT) == "extract");
	EXPECT_TRUE(getOutput(carl::BVTermType::NOT) == "bvnot");
	EXPECT_TRUE(getOutput(carl::BVTermType::NEG) == "bvneg");
	EXPECT_TRUE(getOutput(carl::BVTermType::AND) == "bvand");
	EXPECT_TRUE(getOutput(carl::BVTermType::OR) == "bvor");
	EXPECT_TRUE(getOutput(carl::BVTermType::XOR) == "bvxor");
	EXPECT_TRUE(getOutput(carl::BVTermType::NAND) == "bvnand");
	EXPECT_TRUE(getOutput(carl::BVTermType::NOR) == "bvnor");
	EXPECT_TRUE(getOutput(carl::BVTermType::XNOR) == "bvxnor");
	EXPECT_TRUE(getOutput(carl::BVTermType::ADD) == "bvadd");
	EXPECT_TRUE(getOutput(carl::BVTermType::SUB) == "bvsub");
	EXPECT_TRUE(getOutput(carl::BVTermType::MUL) == "bvmul");
	EXPECT_TRUE(getOutput(carl::BVTermType::DIV_U) == "bvudiv");
	EXPECT_TRUE(getOutput(carl::BVTermType::DIV_S) == "bvsdiv");
	EXPECT_TRUE(getOutput(carl::BVTermType::MOD_U) == "bvurem");
	EXPECT_TRUE(getOutput(carl::BVTermType::MOD_S1) == "bvsrem");
	EXPECT_TRUE(getOutput(carl::BVTermType::MOD_S2) == "bvsmod");
	EXPECT_TRUE(getOutput(carl::BVTermType::EQ) == "bvcomp");
	EXPECT_TRUE(getOutput(carl::BVTermType::LSHIFT) == "bvshl");
	EXPECT_TRUE(getOutput(carl::BVTermType::RSHIFT_LOGIC) == "bvlshr");
	EXPECT_TRUE(getOutput(carl::BVTermType::RSHIFT_ARITH) == "bvashr");
	EXPECT_TRUE(getOutput(carl::BVTermType::LROTATE) == "rotate_left");
	EXPECT_TRUE(getOutput(carl::BVTermType::RROTATE) == "rotate_right");
	EXPECT_TRUE(getOutput(carl::BVTermType::EXT_U) == "zero_extend");
	EXPECT_TRUE(getOutput(carl::BVTermType::EXT_S) == "sign_extend");
	EXPECT_TRUE(getOutput(carl::BVTermType::REPEAT) == "repeat");
}

TEST(BVTermType, typeIsUnary)
{
	EXPECT_FALSE(carl::typeIsUnary(carl::BVTermType::CONSTANT));
	EXPECT_FALSE(carl::typeIsUnary(carl::BVTermType::VARIABLE));
	EXPECT_FALSE(carl::typeIsUnary(carl::BVTermType::CONCAT));
	EXPECT_FALSE(carl::typeIsUnary(carl::BVTermType::EXTRACT));
	EXPECT_TRUE(carl::typeIsUnary(carl::BVTermType::NOT));
	EXPECT_TRUE(carl::typeIsUnary(carl::BVTermType::NEG));
	EXPECT_FALSE(carl::typeIsUnary(carl::BVTermType::AND));
	EXPECT_FALSE(carl::typeIsUnary(carl::BVTermType::OR));
	EXPECT_FALSE(carl::typeIsUnary(carl::BVTermType::XOR));
	EXPECT_FALSE(carl::typeIsUnary(carl::BVTermType::NAND));
	EXPECT_FALSE(carl::typeIsUnary(carl::BVTermType::NOR));
	EXPECT_FALSE(carl::typeIsUnary(carl::BVTermType::XNOR));
	EXPECT_FALSE(carl::typeIsUnary(carl::BVTermType::ADD));
	EXPECT_FALSE(carl::typeIsUnary(carl::BVTermType::SUB));
	EXPECT_FALSE(carl::typeIsUnary(carl::BVTermType::MUL));
	EXPECT_FALSE(carl::typeIsUnary(carl::BVTermType::DIV_U));
	EXPECT_FALSE(carl::typeIsUnary(carl::BVTermType::DIV_S));
	EXPECT_FALSE(carl::typeIsUnary(carl::BVTermType::MOD_U));
	EXPECT_FALSE(carl::typeIsUnary(carl::BVTermType::MOD_S1));
	EXPECT_FALSE(carl::typeIsUnary(carl::BVTermType::MOD_S2));
	EXPECT_FALSE(carl::typeIsUnary(carl::BVTermType::EQ));
	EXPECT_FALSE(carl::typeIsUnary(carl::BVTermType::LSHIFT));
	EXPECT_FALSE(carl::typeIsUnary(carl::BVTermType::RSHIFT_LOGIC));
	EXPECT_FALSE(carl::typeIsUnary(carl::BVTermType::RSHIFT_ARITH));
	EXPECT_TRUE(carl::typeIsUnary(carl::BVTermType::LROTATE));
	EXPECT_TRUE(carl::typeIsUnary(carl::BVTermType::RROTATE));
	EXPECT_TRUE(carl::typeIsUnary(carl::BVTermType::EXT_U));
	EXPECT_TRUE(carl::typeIsUnary(carl::BVTermType::EXT_S));
	EXPECT_TRUE(carl::typeIsUnary(carl::BVTermType::REPEAT));
}

TEST(BVTermType, typeIsBinary)
{
	EXPECT_FALSE(carl::typeIsBinary(carl::BVTermType::CONSTANT));
	EXPECT_FALSE(carl::typeIsBinary(carl::BVTermType::VARIABLE));
	EXPECT_TRUE(carl::typeIsBinary(carl::BVTermType::CONCAT));
	EXPECT_FALSE(carl::typeIsBinary(carl::BVTermType::EXTRACT));
	EXPECT_FALSE(carl::typeIsBinary(carl::BVTermType::NOT));
	EXPECT_FALSE(carl::typeIsBinary(carl::BVTermType::NEG));
	EXPECT_TRUE(carl::typeIsBinary(carl::BVTermType::AND));
	EXPECT_TRUE(carl::typeIsBinary(carl::BVTermType::OR));
	EXPECT_TRUE(carl::typeIsBinary(carl::BVTermType::XOR));
	EXPECT_TRUE(carl::typeIsBinary(carl::BVTermType::NAND));
	EXPECT_TRUE(carl::typeIsBinary(carl::BVTermType::NOR));
	EXPECT_TRUE(carl::typeIsBinary(carl::BVTermType::XNOR));
	EXPECT_TRUE(carl::typeIsBinary(carl::BVTermType::ADD));
	EXPECT_TRUE(carl::typeIsBinary(carl::BVTermType::SUB));
	EXPECT_TRUE(carl::typeIsBinary(carl::BVTermType::MUL));
	EXPECT_TRUE(carl::typeIsBinary(carl::BVTermType::DIV_U));
	EXPECT_TRUE(carl::typeIsBinary(carl::BVTermType::DIV_S));
	EXPECT_TRUE(carl::typeIsBinary(carl::BVTermType::MOD_U));
	EXPECT_TRUE(carl::typeIsBinary(carl::BVTermType::MOD_S1));
	EXPECT_TRUE(carl::typeIsBinary(carl::BVTermType::MOD_S2));
	EXPECT_TRUE(carl::typeIsBinary(carl::BVTermType::EQ));
	EXPECT_TRUE(carl::typeIsBinary(carl::BVTermType::LSHIFT));
	EXPECT_TRUE(carl::typeIsBinary(carl::BVTermType::RSHIFT_LOGIC));
	EXPECT_TRUE(carl::typeIsBinary(carl::BVTermType::RSHIFT_ARITH));
	EXPECT_FALSE(carl::typeIsBinary(carl::BVTermType::LROTATE));
	EXPECT_FALSE(carl::typeIsBinary(carl::BVTermType::RROTATE));
	EXPECT_FALSE(carl::typeIsBinary(carl::BVTermType::EXT_U));
	EXPECT_FALSE(carl::typeIsBinary(carl::BVTermType::EXT_S));
	EXPECT_FALSE(carl::typeIsBinary(carl::BVTermType::REPEAT));
}
