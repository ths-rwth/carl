#include <gtest/gtest.h>

#include <carl/core/Variable.h>

#include "../get_output.h"

TEST(VariableType, SpecialValues)
{
	EXPECT_EQ(0, int(carl::VariableType::MIN_TYPE));
	EXPECT_EQ(4, int(carl::VariableType::MAX_TYPE));
	EXPECT_EQ(5, int(carl::VariableType::TYPE_SIZE));
}

TEST(VariableType, Output)
{
	EXPECT_EQ("Bool", get_output(carl::VariableType::VT_BOOL));
	EXPECT_EQ("Real", get_output(carl::VariableType::VT_REAL));
	EXPECT_EQ("Int", get_output(carl::VariableType::VT_INT));
	EXPECT_EQ("Uninterpreted", get_output(carl::VariableType::VT_UNINTERPRETED));
	EXPECT_EQ("Bitvector", get_output(carl::VariableType::VT_BITVECTOR));
}
