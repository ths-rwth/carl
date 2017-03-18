#include "../Common.h"

#include <carl/core/Relation.h>

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
	EXPECT_EQ("=", getOutput(carl::Relation::EQ));
	EXPECT_EQ("!=", getOutput(carl::Relation::NEQ));
	EXPECT_EQ("<", getOutput(carl::Relation::LESS));
	EXPECT_EQ(">", getOutput(carl::Relation::GREATER));
	EXPECT_EQ("<=", getOutput(carl::Relation::LEQ));
	EXPECT_EQ(">=", getOutput(carl::Relation::GEQ));
#ifdef CARL_BUILD_DEBUG
	EXPECT_DEATH(getOutput(invalid_value<carl::Relation>()), "Invalid relation");
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

TEST(Relation, isStrict)
{
	EXPECT_FALSE(carl::isStrict(carl::Relation::EQ));
	EXPECT_TRUE(carl::isStrict(carl::Relation::NEQ));
	EXPECT_TRUE(carl::isStrict(carl::Relation::LESS));
	EXPECT_TRUE(carl::isStrict(carl::Relation::GREATER));
	EXPECT_FALSE(carl::isStrict(carl::Relation::LEQ));
	EXPECT_FALSE(carl::isStrict(carl::Relation::GEQ));
}

TEST(Relation, isWeak)
{
	EXPECT_TRUE(carl::isWeak(carl::Relation::EQ));
	EXPECT_FALSE(carl::isWeak(carl::Relation::NEQ));
	EXPECT_FALSE(carl::isWeak(carl::Relation::LESS));
	EXPECT_FALSE(carl::isWeak(carl::Relation::GREATER));
	EXPECT_TRUE(carl::isWeak(carl::Relation::LEQ));
	EXPECT_TRUE(carl::isWeak(carl::Relation::GEQ));
}

TEST(Relation, evaluate)
{
	EXPECT_FALSE(carl::evaluate(carl::Sign::POSITIVE, carl::Relation::EQ));
	EXPECT_TRUE(carl::evaluate(carl::Sign::ZERO, carl::Relation::EQ));
	EXPECT_FALSE(carl::evaluate(carl::Sign::NEGATIVE, carl::Relation::EQ));
	
	EXPECT_TRUE(carl::evaluate(carl::Sign::POSITIVE, carl::Relation::NEQ));
	EXPECT_FALSE(carl::evaluate(carl::Sign::ZERO, carl::Relation::NEQ));
	EXPECT_TRUE(carl::evaluate(carl::Sign::NEGATIVE, carl::Relation::NEQ));
	
	EXPECT_FALSE(carl::evaluate(carl::Sign::POSITIVE, carl::Relation::LESS));
	EXPECT_FALSE(carl::evaluate(carl::Sign::ZERO, carl::Relation::LESS));
	EXPECT_TRUE(carl::evaluate(carl::Sign::NEGATIVE, carl::Relation::LESS));
	
	EXPECT_TRUE(carl::evaluate(carl::Sign::POSITIVE, carl::Relation::GREATER));
	EXPECT_FALSE(carl::evaluate(carl::Sign::ZERO, carl::Relation::GREATER));
	EXPECT_FALSE(carl::evaluate(carl::Sign::NEGATIVE, carl::Relation::GREATER));
	
	EXPECT_FALSE(carl::evaluate(carl::Sign::POSITIVE, carl::Relation::LEQ));
	EXPECT_TRUE(carl::evaluate(carl::Sign::ZERO, carl::Relation::LEQ));
	EXPECT_TRUE(carl::evaluate(carl::Sign::NEGATIVE, carl::Relation::LEQ));
	
	EXPECT_TRUE(carl::evaluate(carl::Sign::POSITIVE, carl::Relation::GEQ));
	EXPECT_TRUE(carl::evaluate(carl::Sign::ZERO, carl::Relation::GEQ));
	EXPECT_FALSE(carl::evaluate(carl::Sign::NEGATIVE, carl::Relation::GEQ));
}
