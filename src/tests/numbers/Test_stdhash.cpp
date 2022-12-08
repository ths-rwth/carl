#include <boost/concept_check.hpp>
#include <carl-arith/numbers/numbers.h>
#include <gtest/gtest.h>

#include "../number_types.h"
#include "../require_type.h"

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

TYPED_TEST_SUITE(NumberHash, NumberTypes);

TYPED_TEST(NumberHash, Hash) {
	BOOST_CONCEPT_ASSERT((HashCheck<TypeParam>));
}
