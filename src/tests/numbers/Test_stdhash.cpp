#include "../Common.h"

#include <carl/numbers/numbers.h>

#include <boost/concept_check.hpp>
#include <gtest/gtest.h>


template<typename T>
struct HashCheck {
private:
	T lhs;
public:
	BOOST_CONCEPT_USAGE(HashCheck) {
		require_type<std::size_t>(std::hash<T>()(lhs));
	}
};

template<typename T>
class NumberHash: public testing::Test {};

TYPED_TEST_CASE(NumberHash, NumberTypes);

TYPED_TEST(NumberHash, Hash) {
	BOOST_CONCEPT_ASSERT((HashCheck<TypeParam>));
}
