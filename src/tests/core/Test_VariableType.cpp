
#include "gtest/gtest.h"
#include <carl/core/Variable.h>

#include "../Common.h"

TEST(VariableType, SpecialValues)
{
	EXPECT_EQ(0, int(carl::VariableType::MIN_TYPE));
	EXPECT_EQ(4, int(carl::VariableType::MAX_TYPE));
	EXPECT_EQ(5, int(carl::VariableType::TYPE_SIZE));
}

TEST(VariableType, Output)
{
	EXPECT_EQ("Bool", getOutput(carl::VariableType::VT_BOOL));
	EXPECT_EQ("Real", getOutput(carl::VariableType::VT_REAL));
	EXPECT_EQ("Int", getOutput(carl::VariableType::VT_INT));
	EXPECT_EQ("Uninterpreted", getOutput(carl::VariableType::VT_UNINTERPRETED));
	EXPECT_EQ("Bitvector", getOutput(carl::VariableType::VT_BITVECTOR));
}
