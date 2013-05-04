
#include "gtest/gtest.h"
#include "arithmetic/core/Variable.h"

using namespace arithmetic;

TEST(Variable, Constructor)
{
    
}

TEST(Variable, Operators)
{
    Variable v1(1,VT_INT);
    Variable v2(2,VT_REAL);
    EXPECT_EQ(VT_INT,v1.getType());
    EXPECT_EQ(1,v1.getId());
    EXPECT_EQ(VT_REAL,v2.getType());
    EXPECT_TRUE(v1 != v2);
    EXPECT_TRUE(v1 == v1);
}
