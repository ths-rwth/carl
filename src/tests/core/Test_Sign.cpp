#include "gtest/gtest.h"
#include "carl/core/Sign.h"
#include "carl/util/platform.h"

#include <list>
#include <initializer_list>
#include <functional>
#include <typeinfo>
#include "carl/core/Sign.h"

#include "../Common.h"

using namespace carl;

TEST(Sign, Output)
{
	EXPECT_EQ("NEGATIVE", getOutput(Sign::NEGATIVE));
	EXPECT_EQ("ZERO", getOutput(Sign::ZERO));
	EXPECT_EQ("POSITIVE", getOutput(Sign::POSITIVE));
#ifdef CARL_BUILD_DEBUG
	EXPECT_DEATH(getOutput(invalid_value<carl::Sign>()), "Invalid sign");
#endif
}

TEST(Sign, sgn)
{
	ASSERT_EQ(carl::sgn((int)-1), Sign::NEGATIVE);
	ASSERT_EQ(carl::sgn((int)0), Sign::ZERO);
	ASSERT_EQ(carl::sgn((int)1), Sign::POSITIVE);

	#ifdef USE_CLN_NUMBERS
	ASSERT_EQ(carl::sgn((cln::cl_I)-1), Sign::NEGATIVE);
	ASSERT_EQ(carl::sgn((cln::cl_I)0), Sign::ZERO);
	ASSERT_EQ(carl::sgn((cln::cl_I)1), Sign::POSITIVE);

	ASSERT_EQ(carl::sgn((cln::cl_RA)-1), Sign::NEGATIVE);
	ASSERT_EQ(carl::sgn((cln::cl_RA)0), Sign::ZERO);
	ASSERT_EQ(carl::sgn((cln::cl_RA)1), Sign::POSITIVE);
	#endif

	ASSERT_EQ(carl::sgn((mpz_class)-1), Sign::NEGATIVE);
	ASSERT_EQ(carl::sgn((mpz_class)0), Sign::ZERO);
	ASSERT_EQ(carl::sgn((mpz_class)1), Sign::POSITIVE);
}

// Helper function for next test case
template<typename Object, typename Function>
std::size_t signVar(const std::initializer_list<Object>& l, const Function& f) {
	return carl::signVariations(l.begin(), l.end(), f);
}

// Helper function for next test case
template<typename Object>
std::size_t signVar(const std::initializer_list<Object>& l) {
	return carl::signVariations(l.begin(), l.end());
}

TEST(Sign, signVariations)
// ASSERT_EQ fails due to template-lambda-fuckup here...
{
	// Base cases
	ASSERT_TRUE(signVar({Sign::POSITIVE, Sign::POSITIVE}) == 0);
	ASSERT_TRUE(signVar({Sign::POSITIVE, Sign::NEGATIVE}) == 1);
	ASSERT_TRUE(signVar({Sign::NEGATIVE, Sign::POSITIVE}) == 1);
	ASSERT_TRUE(signVar({Sign::NEGATIVE, Sign::NEGATIVE}) == 0);

	// Check for zero
	ASSERT_TRUE(signVar({Sign::POSITIVE, Sign::ZERO}) == 0);
	ASSERT_TRUE(signVar({Sign::ZERO, Sign::POSITIVE}) == 0);
	ASSERT_TRUE(signVar({Sign::NEGATIVE, Sign::ZERO}) == 0);
	ASSERT_TRUE(signVar({Sign::ZERO, Sign::NEGATIVE}) == 0);

	// Check all lists containing only N/P of length four.
	std::list<Sign> signs({Sign::NEGATIVE, Sign::POSITIVE});
	for (auto a: signs) {
		for (auto b: signs) {
			for (auto c: signs) {
				for (auto d: signs) {
					ASSERT_TRUE( signVar({a,b}) + signVar({b,c}) + signVar({c,d}) == signVar({a,b,c,d}) );
				}
			}
		}
	}

	// Now the same with integers and a conversion functions
	auto f = [](const int& i){return carl::sgn(i);};

	// Base cases
	ASSERT_TRUE(signVar({1, 1}, f) == 0);
	ASSERT_TRUE(signVar({1, -1}, f) == 1);
	ASSERT_TRUE(signVar({-1, 1}, f) == 1);
	ASSERT_TRUE(signVar({-1, -1}, f) == 0);

	// Check for zero
	ASSERT_TRUE(signVar({1, 0}, f) == 0);
	ASSERT_TRUE(signVar({0, 1}, f) == 0);
	ASSERT_TRUE(signVar({-1, 0}, f) == 0);
	ASSERT_TRUE(signVar({0, -1}, f) == 0);

	// Check all lists containing only N/P of length four.
	std::list<int> signs2({-1, 1});
	for (auto a: signs2) {
		for (auto b: signs2) {
			for (auto c: signs2) {
				for (auto d: signs2) {
					ASSERT_TRUE( signVar({a,b}, f) + signVar({b,c}, f) + signVar({c,d}, f) == signVar({a,b,c,d}, f) );
				}
			}
		}
	}
}
