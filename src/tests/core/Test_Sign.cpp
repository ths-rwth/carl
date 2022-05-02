#include <carl/core/Sign.h>
#include <carl-common/meta/platform.h>
#include <gtest/gtest.h>

#include <functional>
#include <initializer_list>
#include <list>
#include <typeinfo>

#include "../Common.h"
#include "../get_output.h"

template<typename T>
class SignTest : public testing::Test {};
TYPED_TEST_CASE(SignTest, NumberTypes);

// Helper function for tests of signVariations
template<typename Object, typename Function>
std::size_t signVar(const std::initializer_list<Object>& l, const Function& f) {
	return carl::sign_variations(l.begin(), l.end(), f);
}

// Helper function for tests of signVariations
template<typename Object>
std::size_t signVar(const std::initializer_list<Object>& l) {
	return carl::sign_variations(l.begin(), l.end());
}

TEST(Sign, Output) {
	EXPECT_EQ("NEGATIVE", getOutput(carl::Sign::NEGATIVE));
	EXPECT_EQ("ZERO", getOutput(carl::Sign::ZERO));
	EXPECT_EQ("POSITIVE", getOutput(carl::Sign::POSITIVE));
#ifdef CARL_BUILD_DEBUG
	EXPECT_DEATH(getOutput(invalid_value<carl::Sign>()), "Invalid sign");
#endif
}

TYPED_TEST(SignTest, sgn) {
	EXPECT_EQ(carl::sgn(TypeParam(-1)), carl::Sign::NEGATIVE);
	EXPECT_EQ(carl::sgn(TypeParam(0)), carl::Sign::ZERO);
	EXPECT_EQ(carl::sgn(TypeParam(1)), carl::Sign::POSITIVE);
}

TEST(Sign, signVariations) {
	// Base cases
	EXPECT_TRUE(signVar({carl::Sign::POSITIVE, carl::Sign::POSITIVE}) == 0);
	EXPECT_TRUE(signVar({carl::Sign::POSITIVE, carl::Sign::NEGATIVE}) == 1);
	EXPECT_TRUE(signVar({carl::Sign::NEGATIVE, carl::Sign::POSITIVE}) == 1);
	EXPECT_TRUE(signVar({carl::Sign::NEGATIVE, carl::Sign::NEGATIVE}) == 0);

	// Check for zero
	EXPECT_TRUE(signVar({carl::Sign::POSITIVE, carl::Sign::ZERO}) == 0);
	EXPECT_TRUE(signVar({carl::Sign::ZERO, carl::Sign::POSITIVE}) == 0);
	EXPECT_TRUE(signVar({carl::Sign::NEGATIVE, carl::Sign::ZERO}) == 0);
	EXPECT_TRUE(signVar({carl::Sign::ZERO, carl::Sign::NEGATIVE}) == 0);

	// Check all lists containing only N/P of length four.
	std::vector<carl::Sign> signs({carl::Sign::NEGATIVE, carl::Sign::POSITIVE});
	for (auto a : signs) {
		for (auto b : signs) {
			for (auto c : signs) {
				for (auto d : signs) {
					EXPECT_TRUE(signVar({a, b}) + signVar({b, c}) + signVar({c, d}) == signVar({a, b, c, d}));
				}
			}
		}
	}
}

TYPED_TEST(SignTest, signVariations) {
	// Now the same with numbers and a conversion functions
	auto f = [](const auto& i) { return carl::sgn(i); };

	TypeParam pos = 1;
	TypeParam zer = 0;
	TypeParam neg = -1;

	// Base cases
	EXPECT_TRUE(signVar({pos, pos}, f) == 0);
	EXPECT_TRUE(signVar({pos, neg}, f) == 1);
	EXPECT_TRUE(signVar({neg, pos}, f) == 1);
	EXPECT_TRUE(signVar({neg, neg}, f) == 0);

	// Check for zero
	EXPECT_TRUE(signVar({pos, zer}, f) == 0);
	EXPECT_TRUE(signVar({zer, pos}, f) == 0);
	EXPECT_TRUE(signVar({neg, zer}, f) == 0);
	EXPECT_TRUE(signVar({zer, neg}, f) == 0);

	// Check all lists containing only N/P of length four.
	std::vector<TypeParam> signs({neg, pos});
	for (auto a : signs) {
		for (auto b : signs) {
			for (auto c : signs) {
				for (auto d : signs) {
					EXPECT_TRUE(signVar({a, b}, f) + signVar({b, c}, f) + signVar({c, d}, f) == signVar({a, b, c, d}, f));
				}
			}
		}
	}
}
