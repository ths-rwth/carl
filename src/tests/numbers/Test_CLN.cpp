#include "gtest/gtest.h"
#include "../../carl/numbers/numbers.h"

#ifndef THREAD_SAFE
#ifdef USE_CLN_NUMBERS
TEST(CLN, Rationalize)
{
    EXPECT_TRUE( carl::rationalize<cln::cl_RA>(carl::toDouble(cln::cl_RA(1)/cln::cl_RA(3))) != cln::cl_RA(1)/cln::cl_RA(3) );
    EXPECT_TRUE( carl::rationalize<cln::cl_RA>(carl::toDouble(cln::cl_RA(1)/cln::cl_RA(20))) != cln::cl_RA(1)/cln::cl_RA(20) );
    
    EXPECT_TRUE(carl::parse<cln::cl_RA>("123") == cln::cl_RA(123));
    EXPECT_TRUE(carl::parse<cln::cl_RA>("1e3") == cln::cl_RA(1000));
}
#endif
#endif
