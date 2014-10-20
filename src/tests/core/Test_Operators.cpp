#include <boost/concept_check.hpp>

#include "gtest/gtest.h"
#include "../numbers/config.h"
#include "carl/core/Variable.h"
#include "carl/core/Monomial.h"
#include "carl/core/Term.h"
#include "carl/core/MultivariatePolynomial.h"

using namespace carl;

template<typename T1, typename T2>
struct Comparison {
private:
	T1 lhs;
	T2 rhs;
public:
	BOOST_CONCEPT_USAGE(Comparison) {
		boost::require_boolean_expr(lhs == rhs);
		boost::require_boolean_expr(lhs != rhs);
		boost::require_boolean_expr(lhs < rhs);
		boost::require_boolean_expr(lhs <= rhs);
		boost::require_boolean_expr(lhs > rhs);
		boost::require_boolean_expr(lhs >= rhs);
	}
};

template<typename T1, typename T2>
struct Arithmetic {
private:
	T1 lhs;
	T2 rhs;
public:
	BOOST_CONCEPT_USAGE(Arithmetic) {
		boost::require_boolean_expr(lhs + rhs);
		boost::require_boolean_expr(lhs - rhs);
		boost::require_boolean_expr(-lhs);
		boost::require_boolean_expr(-rhs);
		boost::require_boolean_expr(lhs * rhs);
	}
};

template<typename T1, typename T2>
struct InplaceAddition {
private:
	T1 lhs;
	T2 rhs;
public:
	BOOST_CONCEPT_USAGE(InplaceAddition) {
		boost::require_boolean_expr(lhs += rhs);
		boost::require_boolean_expr(lhs -= rhs);
	}
};

template<typename T1, typename T2>
struct InplaceMultiplication {
private:
	T1 lhs;
	T2 rhs;
public:
	BOOST_CONCEPT_USAGE(InplaceMultiplication) {
		boost::require_boolean_expr(lhs *= rhs);
	}
};

template<typename T>
class Operators: public testing::Test {};

TYPED_TEST_CASE(Operators, RationalTypes);

TYPED_TEST(Operators, Comparison)
{
    BOOST_CONCEPT_ASSERT((Comparison<carl::Variable, carl::Variable>));
	
	BOOST_CONCEPT_ASSERT((Comparison<carl::Monomial, carl::Monomial>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Monomial, carl::Variable>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Variable, carl::Monomial>));
	
	BOOST_CONCEPT_ASSERT((Comparison<carl::Term<TypeParam>, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Term<TypeParam>, carl::Variable>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Term<TypeParam>, carl::Monomial>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Variable, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Monomial, carl::Term<TypeParam>>));
	
	BOOST_CONCEPT_ASSERT((Comparison<carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::MultivariatePolynomial<TypeParam>, carl::Variable>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::MultivariatePolynomial<TypeParam>, carl::Monomial>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::MultivariatePolynomial<TypeParam>, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Variable, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Monomial, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Term<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
}

TYPED_TEST(Operators, Arithmetic)
{
    BOOST_CONCEPT_ASSERT((Arithmetic<carl::Variable, carl::Variable>));
	
	BOOST_CONCEPT_ASSERT((Arithmetic<carl::Monomial, carl::Monomial>));
	BOOST_CONCEPT_ASSERT((Arithmetic<carl::Monomial, carl::Variable>));
	BOOST_CONCEPT_ASSERT((Arithmetic<carl::Variable, carl::Monomial>));
	
	BOOST_CONCEPT_ASSERT((Arithmetic<carl::Term<TypeParam>, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Arithmetic<carl::Term<TypeParam>, carl::Variable>));
	BOOST_CONCEPT_ASSERT((Arithmetic<carl::Term<TypeParam>, carl::Monomial>));
	BOOST_CONCEPT_ASSERT((Arithmetic<carl::Variable, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Arithmetic<carl::Monomial, carl::Term<TypeParam>>));
	
	BOOST_CONCEPT_ASSERT((Arithmetic<carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Arithmetic<carl::MultivariatePolynomial<TypeParam>, carl::Variable>));
	BOOST_CONCEPT_ASSERT((Arithmetic<carl::MultivariatePolynomial<TypeParam>, carl::Monomial>));
	BOOST_CONCEPT_ASSERT((Arithmetic<carl::MultivariatePolynomial<TypeParam>, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Arithmetic<carl::Variable, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Arithmetic<carl::Monomial, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Arithmetic<carl::Term<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
}

TYPED_TEST(Operators, InplaceAddition)
{	
	BOOST_CONCEPT_ASSERT((InplaceAddition<carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((InplaceAddition<carl::MultivariatePolynomial<TypeParam>, carl::Variable>));
	BOOST_CONCEPT_ASSERT((InplaceAddition<carl::MultivariatePolynomial<TypeParam>, carl::Monomial>));
	BOOST_CONCEPT_ASSERT((InplaceAddition<carl::MultivariatePolynomial<TypeParam>, carl::Term<TypeParam>>));
}

TYPED_TEST(Operators, InplaceMultiplication)
{
	BOOST_CONCEPT_ASSERT((InplaceMultiplication<carl::Monomial, carl::Monomial>));
	BOOST_CONCEPT_ASSERT((InplaceMultiplication<carl::Monomial, carl::Variable>));
	
	BOOST_CONCEPT_ASSERT((InplaceMultiplication<carl::Term<TypeParam>, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((InplaceMultiplication<carl::Term<TypeParam>, carl::Variable>));
	BOOST_CONCEPT_ASSERT((InplaceMultiplication<carl::Term<TypeParam>, carl::Monomial>));
	
	BOOST_CONCEPT_ASSERT((InplaceMultiplication<carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((InplaceMultiplication<carl::MultivariatePolynomial<TypeParam>, carl::Variable>));
	BOOST_CONCEPT_ASSERT((InplaceMultiplication<carl::MultivariatePolynomial<TypeParam>, carl::Monomial>));
	BOOST_CONCEPT_ASSERT((InplaceMultiplication<carl::MultivariatePolynomial<TypeParam>, carl::Term<TypeParam>>));
}