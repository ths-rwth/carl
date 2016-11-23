#include "gtest/gtest.h"

#include <carl/formula/bitvector/BVTerm.h>
#include <carl/formula/bitvector/BVTermPool.h>

#include "../Common.h"

using namespace carl;

TEST(BVTerm, TermTypeOutput)
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

TEST(BVTerm, Construction)
{
    Variable a = freshBitvectorVariable("a");
    Variable b = freshBitvectorVariable("b");

    
}
