#include <boost/concept_check.hpp>

#include "gtest/gtest.h"
#include "../numbers/config.h"
#include "carl/core/Variable.h"
#include "carl/core/Monomial.h"
#include "carl/core/Term.h"
#include "carl/core/MultivariatePolynomial.h"

using namespace carl;

template<typename T, typename TT>
void require_type(const TT& tt) {
	T t = tt;
	(void)(t);
}

template<typename T1, typename T2>
struct Comparison {
private:
	T1 lhs;
	T2 rhs;
public:
	BOOST_CONCEPT_USAGE(Comparison) {
		require_type<bool>(lhs == rhs);
		require_type<bool>(lhs == rhs);
		require_type<bool>(lhs != rhs);
		require_type<bool>(lhs < rhs);
		require_type<bool>(lhs <= rhs);
		require_type<bool>(lhs > rhs);
		require_type<bool>(lhs >= rhs);
	}
};

template<typename T1, typename T2, typename R>
struct Addition {
private:
	T1 lhs;
	T2 rhs;
public:
	BOOST_CONCEPT_USAGE(Addition) {
		require_type<R>(lhs + rhs);
		require_type<R>(lhs - rhs);
	}
};

template<typename T1>
struct UnaryMinus {
private:
	T1 lhs;
public:
	BOOST_CONCEPT_USAGE(UnaryMinus) {
		require_type<T1>(-lhs);
	}
};

template<typename T1, typename T2, typename R>
struct Multiplication {
private:
	T1 lhs;
	T2 rhs;
public:
	BOOST_CONCEPT_USAGE(Multiplication) {
		require_type<R>(lhs * rhs);
	}
};

template<typename T1, typename T2>
struct InplaceAddition {
private:
	T1 lhs;
	T2 rhs;
public:
	BOOST_CONCEPT_USAGE(InplaceAddition) {
		require_type<T1>(lhs += rhs);
		require_type<T1>(lhs -= rhs);
	}
};

template<typename T1, typename T2>
struct InplaceMultiplication {
private:
	T1 lhs;
	T2 rhs;
public:
	BOOST_CONCEPT_USAGE(InplaceMultiplication) {
		require_type<T1>(lhs *= rhs);
	}
};

template<typename T>
class Operators: public testing::Test {};

TYPED_TEST_CASE(Operators, RationalTypes);

TYPED_TEST(Operators, Comparison)
{
	BOOST_CONCEPT_ASSERT((Comparison<TypeParam, TypeParam>));
	
    BOOST_CONCEPT_ASSERT((Comparison<carl::Variable, carl::Variable>));
	
	BOOST_CONCEPT_ASSERT((Comparison<carl::Monomial, carl::Monomial>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Monomial, carl::Variable>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Variable, carl::Monomial>));
	
	BOOST_CONCEPT_ASSERT((Comparison<carl::Term<TypeParam>, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Term<TypeParam>, carl::Monomial>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Term<TypeParam>, carl::Variable>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Term<TypeParam>, TypeParam>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Monomial, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Variable, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Comparison<TypeParam, carl::Term<TypeParam>>));
	
	BOOST_CONCEPT_ASSERT((Comparison<carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::MultivariatePolynomial<TypeParam>, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::MultivariatePolynomial<TypeParam>, carl::Monomial>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::MultivariatePolynomial<TypeParam>, carl::Variable>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::MultivariatePolynomial<TypeParam>, TypeParam>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Term<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Monomial, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Variable, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Comparison<TypeParam, carl::MultivariatePolynomial<TypeParam>>));
}

TYPED_TEST(Operators, Addition)
{
	BOOST_CONCEPT_ASSERT((Addition<TypeParam, TypeParam, TypeParam>));
	
	BOOST_CONCEPT_ASSERT((Addition<carl::Variable, TypeParam, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<TypeParam, carl::Variable, carl::MultivariatePolynomial<TypeParam>>));
	
	BOOST_CONCEPT_ASSERT((Addition<carl::Monomial, TypeParam, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<TypeParam, carl::Monomial, carl::MultivariatePolynomial<TypeParam>>));
	
	BOOST_CONCEPT_ASSERT((Addition<carl::Term<TypeParam>, carl::Term<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::Term<TypeParam>, carl::Monomial, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::Term<TypeParam>, carl::Variable, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::Term<TypeParam>, TypeParam, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::Monomial, carl::Term<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::Variable, carl::Term<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<TypeParam, carl::Term<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	
	BOOST_CONCEPT_ASSERT((Addition<carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::MultivariatePolynomial<TypeParam>, carl::Term<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::MultivariatePolynomial<TypeParam>, carl::Monomial, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::MultivariatePolynomial<TypeParam>, carl::Variable, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::MultivariatePolynomial<TypeParam>, TypeParam, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::Term<TypeParam>, carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::Monomial, carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::Variable, carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<TypeParam, carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
}

TYPED_TEST(Operators, UnaryMinus)
{
	BOOST_CONCEPT_ASSERT((UnaryMinus<TypeParam>));
	BOOST_CONCEPT_ASSERT((UnaryMinus<carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((UnaryMinus<carl::MultivariatePolynomial<TypeParam>>));
}

TYPED_TEST(Operators, Multiplication)
{
    BOOST_CONCEPT_ASSERT((Multiplication<carl::Variable, carl::Variable, carl::Monomial>));
	
	BOOST_CONCEPT_ASSERT((Multiplication<carl::Monomial, carl::Monomial, carl::Monomial>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::Monomial, carl::Variable, carl::Monomial>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::Variable, carl::Monomial, carl::Monomial>));
	
	BOOST_CONCEPT_ASSERT((Multiplication<carl::Term<TypeParam>, carl::Term<TypeParam>, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::Term<TypeParam>, carl::Variable, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::Term<TypeParam>, carl::Monomial, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::Variable, carl::Term<TypeParam>, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::Monomial, carl::Term<TypeParam>, carl::Term<TypeParam>>));
	
	BOOST_CONCEPT_ASSERT((Multiplication<carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::MultivariatePolynomial<TypeParam>, carl::Variable, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::MultivariatePolynomial<TypeParam>, carl::Monomial, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::MultivariatePolynomial<TypeParam>, carl::Term<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::Variable, carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::Monomial, carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::Term<TypeParam>, carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
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