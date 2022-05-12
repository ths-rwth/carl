#include <gtest/gtest.h>

#include <carl/core/Relation.h>

#include "../get_output.h"
#include "../invalid_value.h"

TEST(Relation, Values)
{
	EXPECT_EQ(carl::Relation(0), carl::Relation::EQ);
	EXPECT_EQ(carl::Relation(1), carl::Relation::NEQ);
	EXPECT_EQ(carl::Relation(2), carl::Relation::LESS);
	EXPECT_EQ(carl::Relation(3), carl::Relation::GREATER);
	EXPECT_EQ(carl::Relation(4), carl::Relation::LEQ);
	EXPECT_EQ(carl::Relation(5), carl::Relation::GEQ);
}

TEST(Relation, Output)
{
	EXPECT_EQ("=", get_output(carl::Relation::EQ));
	EXPECT_EQ("!=", get_output(carl::Relation::NEQ));
	EXPECT_EQ("<", get_output(carl::Relation::LESS));
	EXPECT_EQ(">", get_output(carl::Relation::GREATER));
	EXPECT_EQ("<=", get_output(carl::Relation::LEQ));
	EXPECT_EQ(">=", get_output(carl::Relation::GEQ));
#ifdef CARL_BUILD_DEBUG
	EXPECT_DEATH(get_output(invalid_value<carl::Relation>()), "Invalid relation");
#endif
}

TEST(Relation, inverse)
{
	EXPECT_EQ(carl::Relation::NEQ, carl::inverse(carl::Relation::EQ));
	EXPECT_EQ(carl::Relation::EQ, carl::inverse(carl::Relation::NEQ));
	EXPECT_EQ(carl::Relation::GEQ, carl::inverse(carl::Relation::LESS));
	EXPECT_EQ(carl::Relation::LEQ, carl::inverse(carl::Relation::GREATER));
	EXPECT_EQ(carl::Relation::GREATER, carl::inverse(carl::Relation::LEQ));
	EXPECT_EQ(carl::Relation::LESS, carl::inverse(carl::Relation::GEQ));
#ifdef CARL_BUILD_DEBUG
	EXPECT_DEATH(carl::inverse(invalid_value<carl::Relation>()), "Invalid relation");
#endif
}

TEST(Relation, is_strict)
{
	EXPECT_FALSE(carl::is_strict(carl::Relation::EQ));
	EXPECT_TRUE(carl::is_strict(carl::Relation::NEQ));
	EXPECT_TRUE(carl::is_strict(carl::Relation::LESS));
	EXPECT_TRUE(carl::is_strict(carl::Relation::GREATER));
	EXPECT_FALSE(carl::is_strict(carl::Relation::LEQ));
	EXPECT_FALSE(carl::is_strict(carl::Relation::GEQ));
}

TEST(Relation, is_weak)
{
	EXPECT_TRUE(carl::is_weak(carl::Relation::EQ));
	EXPECT_FALSE(carl::is_weak(carl::Relation::NEQ));
	EXPECT_FALSE(carl::is_weak(carl::Relation::LESS));
	EXPECT_FALSE(carl::is_weak(carl::Relation::GREATER));
	EXPECT_TRUE(carl::is_weak(carl::Relation::LEQ));
	EXPECT_TRUE(carl::is_weak(carl::Relation::GEQ));
}

TEST(Relation, evaluate)
{
	EXPECT_FALSE(carl::evaluate(carl::Sign::POSITIVE, carl::Relation::EQ));
	EXPECT_TRUE(carl::evaluate(carl::Sign::POSITIVE, carl::Relation::NEQ));
	EXPECT_FALSE(carl::evaluate(carl::Sign::POSITIVE, carl::Relation::LESS));
	EXPECT_TRUE(carl::evaluate(carl::Sign::POSITIVE, carl::Relation::GREATER));
	EXPECT_FALSE(carl::evaluate(carl::Sign::POSITIVE, carl::Relation::LEQ));
	EXPECT_TRUE(carl::evaluate(carl::Sign::POSITIVE, carl::Relation::GEQ));
	
	EXPECT_TRUE(carl::evaluate(carl::Sign::ZERO, carl::Relation::EQ));
	EXPECT_FALSE(carl::evaluate(carl::Sign::ZERO, carl::Relation::NEQ));
	EXPECT_FALSE(carl::evaluate(carl::Sign::ZERO, carl::Relation::LESS));
	EXPECT_FALSE(carl::evaluate(carl::Sign::ZERO, carl::Relation::GREATER));
	EXPECT_TRUE(carl::evaluate(carl::Sign::ZERO, carl::Relation::LEQ));
	EXPECT_TRUE(carl::evaluate(carl::Sign::ZERO, carl::Relation::GEQ));
	
	EXPECT_FALSE(carl::evaluate(carl::Sign::NEGATIVE, carl::Relation::EQ));
	EXPECT_TRUE(carl::evaluate(carl::Sign::NEGATIVE, carl::Relation::NEQ));
	EXPECT_TRUE(carl::evaluate(carl::Sign::NEGATIVE, carl::Relation::LESS));
	EXPECT_FALSE(carl::evaluate(carl::Sign::NEGATIVE, carl::Relation::GREATER));
	EXPECT_TRUE(carl::evaluate(carl::Sign::NEGATIVE, carl::Relation::LEQ));
	EXPECT_FALSE(carl::evaluate(carl::Sign::NEGATIVE, carl::Relation::GEQ));

#ifdef CARL_BUILD_DEBUG
	EXPECT_DEATH(carl::evaluate(invalid_value<carl::Sign>(), carl::Relation::EQ), "Invalid sign");
	EXPECT_DEATH(carl::evaluate(invalid_value<carl::Sign>(), carl::Relation::NEQ), "Invalid sign");
	EXPECT_DEATH(carl::evaluate(invalid_value<carl::Sign>(), carl::Relation::LESS), "Invalid sign");
	EXPECT_DEATH(carl::evaluate(invalid_value<carl::Sign>(), carl::Relation::GREATER), "Invalid sign");
	EXPECT_DEATH(carl::evaluate(invalid_value<carl::Sign>(), carl::Relation::LEQ), "Invalid sign");
	EXPECT_DEATH(carl::evaluate(invalid_value<carl::Sign>(), carl::Relation::GEQ), "Invalid sign");
#endif
}
