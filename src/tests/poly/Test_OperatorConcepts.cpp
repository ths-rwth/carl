
#include "../Common.h"

#include <carl-arith/core/Variable.h>
#include <carl-arith/poly/umvpoly/Monomial.h>
#include <carl-arith/poly/umvpoly/Term.h>
#include <carl-arith/poly/umvpoly/MultivariatePolynomial.h>

#include <boost/concept_check.hpp>
#include "gtest/gtest.h"

using namespace carl;

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

	BOOST_CONCEPT_ASSERT((Comparison<carl::Monomial::Arg, carl::Monomial::Arg>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Monomial::Arg, carl::Variable>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Variable, carl::Monomial::Arg>));

	BOOST_CONCEPT_ASSERT((Comparison<carl::Term<TypeParam>, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Term<TypeParam>, carl::Monomial::Arg>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Term<TypeParam>, carl::Variable>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Term<TypeParam>, TypeParam>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Monomial::Arg, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Variable, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Comparison<TypeParam, carl::Term<TypeParam>>));

	BOOST_CONCEPT_ASSERT((Comparison<carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::MultivariatePolynomial<TypeParam>, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::MultivariatePolynomial<TypeParam>, carl::Monomial::Arg>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::MultivariatePolynomial<TypeParam>, carl::Variable>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::MultivariatePolynomial<TypeParam>, TypeParam>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Term<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Monomial::Arg, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Variable, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Comparison<TypeParam, carl::MultivariatePolynomial<TypeParam>>));
}

TYPED_TEST(Operators, Addition)
{
	BOOST_CONCEPT_ASSERT((Addition<TypeParam, TypeParam, TypeParam>));

	BOOST_CONCEPT_ASSERT((Addition<carl::Variable, TypeParam, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<TypeParam, carl::Variable, carl::MultivariatePolynomial<TypeParam>>));

	BOOST_CONCEPT_ASSERT((Addition<carl::Monomial::Arg, TypeParam, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<TypeParam, carl::Monomial::Arg, carl::MultivariatePolynomial<TypeParam>>));

	BOOST_CONCEPT_ASSERT((Addition<carl::Term<TypeParam>, carl::Term<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::Term<TypeParam>, carl::Monomial::Arg, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::Term<TypeParam>, carl::Variable, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::Term<TypeParam>, TypeParam, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::Monomial::Arg, carl::Term<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::Variable, carl::Term<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<TypeParam, carl::Term<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));

	BOOST_CONCEPT_ASSERT((Addition<carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::MultivariatePolynomial<TypeParam>, carl::Term<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::MultivariatePolynomial<TypeParam>, carl::Monomial::Arg, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::MultivariatePolynomial<TypeParam>, carl::Variable, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::MultivariatePolynomial<TypeParam>, TypeParam, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::Term<TypeParam>, carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::Monomial::Arg, carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
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
	BOOST_CONCEPT_ASSERT((Multiplication<TypeParam, TypeParam, TypeParam>));

    BOOST_CONCEPT_ASSERT((Multiplication<carl::Variable, carl::Variable, carl::Monomial::Arg>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::Variable, TypeParam, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<TypeParam, carl::Variable, carl::Term<TypeParam>>));

	BOOST_CONCEPT_ASSERT((Multiplication<carl::Monomial::Arg, carl::Monomial::Arg, carl::Monomial::Arg>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::Monomial::Arg, carl::Variable, carl::Monomial::Arg>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::Monomial::Arg, TypeParam, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::Variable, carl::Monomial::Arg, carl::Monomial::Arg>));
	BOOST_CONCEPT_ASSERT((Multiplication<TypeParam, carl::Monomial::Arg, carl::Term<TypeParam>>));

	BOOST_CONCEPT_ASSERT((Multiplication<carl::Term<TypeParam>, carl::Term<TypeParam>, carl::Term<TypeParam>>));
	//BOOST_CONCEPT_ASSERT((Multiplication<carl::Term<TypeParam>, carl::Monomial::Arg, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::Term<TypeParam>, carl::Variable, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::Term<TypeParam>, TypeParam, carl::Term<TypeParam>>));
	//BOOST_CONCEPT_ASSERT((Multiplication<carl::Monomial::Arg, carl::Term<TypeParam>, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::Variable, carl::Term<TypeParam>, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<TypeParam, carl::Term<TypeParam>, carl::Term<TypeParam>>));

	BOOST_CONCEPT_ASSERT((Multiplication<carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::MultivariatePolynomial<TypeParam>, carl::Term<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	//BOOST_CONCEPT_ASSERT((Multiplication<carl::MultivariatePolynomial<TypeParam>, carl::Monomial::Arg, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::MultivariatePolynomial<TypeParam>, carl::Variable, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::MultivariatePolynomial<TypeParam>, TypeParam, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::Term<TypeParam>, carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	//BOOST_CONCEPT_ASSERT((Multiplication<carl::Monomial::Arg, carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::Variable, carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<TypeParam, carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
}

TYPED_TEST(Operators, InplaceAddition)
{
	BOOST_CONCEPT_ASSERT((InplaceAddition<TypeParam, TypeParam>));

	BOOST_CONCEPT_ASSERT((InplaceAddition<carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((InplaceAddition<carl::MultivariatePolynomial<TypeParam>, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((InplaceAddition<carl::MultivariatePolynomial<TypeParam>, carl::Monomial::Arg>));
	BOOST_CONCEPT_ASSERT((InplaceAddition<carl::MultivariatePolynomial<TypeParam>, carl::Variable>));
	BOOST_CONCEPT_ASSERT((InplaceAddition<carl::MultivariatePolynomial<TypeParam>, TypeParam>));
}

TYPED_TEST(Operators, InplaceMultiplication)
{
	//BOOST_CONCEPT_ASSERT((InplaceMultiplication<carl::Monomial::Arg, carl::Monomial::Arg>));
	//BOOST_CONCEPT_ASSERT((InplaceMultiplication<carl::Monomial::Arg, carl::Variable>));

	BOOST_CONCEPT_ASSERT((InplaceMultiplication<carl::Term<TypeParam>, carl::Term<TypeParam>>));
	//BOOST_CONCEPT_ASSERT((InplaceMultiplication<carl::Term<TypeParam>, carl::Monomial::Arg>));
	BOOST_CONCEPT_ASSERT((InplaceMultiplication<carl::Term<TypeParam>, carl::Variable>));
	BOOST_CONCEPT_ASSERT((InplaceMultiplication<carl::Term<TypeParam>, TypeParam>));

	BOOST_CONCEPT_ASSERT((InplaceMultiplication<carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((InplaceMultiplication<carl::MultivariatePolynomial<TypeParam>, carl::Term<TypeParam>>));
	//BOOST_CONCEPT_ASSERT((InplaceMultiplication<carl::MultivariatePolynomial<TypeParam>, carl::Monomial::Arg>));
	BOOST_CONCEPT_ASSERT((InplaceMultiplication<carl::MultivariatePolynomial<TypeParam>, carl::Variable>));
	BOOST_CONCEPT_ASSERT((InplaceMultiplication<carl::MultivariatePolynomial<TypeParam>, TypeParam>));
}

template<typename T>
class OperatorsInterval: public testing::Test {};

TYPED_TEST_CASE(OperatorsInterval, IntervalTypes);

TYPED_TEST(OperatorsInterval, Comparison)
{
	BOOST_CONCEPT_ASSERT((Comparison<TypeParam, TypeParam>));

	BOOST_CONCEPT_ASSERT((Comparison<carl::Variable, carl::Variable>));

	BOOST_CONCEPT_ASSERT((Comparison<carl::Monomial::Arg, carl::Monomial::Arg>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Monomial::Arg, carl::Variable>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Variable, carl::Monomial::Arg>));

	BOOST_CONCEPT_ASSERT((Comparison<carl::Term<TypeParam>, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Term<TypeParam>, carl::Monomial::Arg>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Term<TypeParam>, carl::Variable>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Term<TypeParam>, TypeParam>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Monomial::Arg, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Variable, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Comparison<TypeParam, carl::Term<TypeParam>>));

	BOOST_CONCEPT_ASSERT((Comparison<carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::MultivariatePolynomial<TypeParam>, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::MultivariatePolynomial<TypeParam>, carl::Monomial::Arg>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::MultivariatePolynomial<TypeParam>, carl::Variable>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::MultivariatePolynomial<TypeParam>, TypeParam>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Term<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Monomial::Arg, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Comparison<carl::Variable, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Comparison<TypeParam, carl::MultivariatePolynomial<TypeParam>>));
}

TYPED_TEST(OperatorsInterval, Addition)
{
	BOOST_CONCEPT_ASSERT((Addition<TypeParam, TypeParam, TypeParam>));

	BOOST_CONCEPT_ASSERT((Addition<carl::Variable, TypeParam, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<TypeParam, carl::Variable, carl::MultivariatePolynomial<TypeParam>>));

	BOOST_CONCEPT_ASSERT((Addition<carl::Monomial::Arg, TypeParam, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<TypeParam, carl::Monomial::Arg, carl::MultivariatePolynomial<TypeParam>>));

	BOOST_CONCEPT_ASSERT((Addition<carl::Term<TypeParam>, carl::Term<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::Term<TypeParam>, carl::Monomial::Arg, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::Term<TypeParam>, carl::Variable, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::Term<TypeParam>, TypeParam, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::Monomial::Arg, carl::Term<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::Variable, carl::Term<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<TypeParam, carl::Term<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));

	BOOST_CONCEPT_ASSERT((Addition<carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::MultivariatePolynomial<TypeParam>, carl::Term<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::MultivariatePolynomial<TypeParam>, carl::Monomial::Arg, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::MultivariatePolynomial<TypeParam>, carl::Variable, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::MultivariatePolynomial<TypeParam>, TypeParam, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::Term<TypeParam>, carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::Monomial::Arg, carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<carl::Variable, carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Addition<TypeParam, carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
}

TYPED_TEST(OperatorsInterval, UnaryMinus)
{
	BOOST_CONCEPT_ASSERT((UnaryMinus<TypeParam>));
	BOOST_CONCEPT_ASSERT((UnaryMinus<carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((UnaryMinus<carl::MultivariatePolynomial<TypeParam>>));
}

TYPED_TEST(OperatorsInterval, Multiplication)
{
	BOOST_CONCEPT_ASSERT((Multiplication<TypeParam, TypeParam, TypeParam>));

    BOOST_CONCEPT_ASSERT((Multiplication<carl::Variable, carl::Variable, carl::Monomial::Arg>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::Variable, TypeParam, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<TypeParam, carl::Variable, carl::Term<TypeParam>>));

	BOOST_CONCEPT_ASSERT((Multiplication<carl::Monomial::Arg, carl::Monomial::Arg, carl::Monomial::Arg>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::Monomial::Arg, carl::Variable, carl::Monomial::Arg>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::Monomial::Arg, TypeParam, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::Variable, carl::Monomial::Arg, carl::Monomial::Arg>));
	BOOST_CONCEPT_ASSERT((Multiplication<TypeParam, carl::Monomial::Arg, carl::Term<TypeParam>>));

	BOOST_CONCEPT_ASSERT((Multiplication<carl::Term<TypeParam>, carl::Term<TypeParam>, carl::Term<TypeParam>>));
	//BOOST_CONCEPT_ASSERT((Multiplication<carl::Term<TypeParam>, carl::Monomial::Arg, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::Term<TypeParam>, carl::Variable, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::Term<TypeParam>, TypeParam, carl::Term<TypeParam>>));
	//BOOST_CONCEPT_ASSERT((Multiplication<carl::Monomial::Arg, carl::Term<TypeParam>, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::Variable, carl::Term<TypeParam>, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<TypeParam, carl::Term<TypeParam>, carl::Term<TypeParam>>));

	BOOST_CONCEPT_ASSERT((Multiplication<carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::MultivariatePolynomial<TypeParam>, carl::Term<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	//BOOST_CONCEPT_ASSERT((Multiplication<carl::MultivariatePolynomial<TypeParam>, carl::Monomial::Arg, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::MultivariatePolynomial<TypeParam>, carl::Variable, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::MultivariatePolynomial<TypeParam>, TypeParam, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::Term<TypeParam>, carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	//BOOST_CONCEPT_ASSERT((Multiplication<carl::Monomial::Arg, carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<carl::Variable, carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((Multiplication<TypeParam, carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
}

TYPED_TEST(OperatorsInterval, InplaceAddition)
{
	BOOST_CONCEPT_ASSERT((InplaceAddition<TypeParam, TypeParam>));

	BOOST_CONCEPT_ASSERT((InplaceAddition<carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((InplaceAddition<carl::MultivariatePolynomial<TypeParam>, carl::Term<TypeParam>>));
	BOOST_CONCEPT_ASSERT((InplaceAddition<carl::MultivariatePolynomial<TypeParam>, carl::Monomial::Arg>));
	BOOST_CONCEPT_ASSERT((InplaceAddition<carl::MultivariatePolynomial<TypeParam>, carl::Variable>));
	BOOST_CONCEPT_ASSERT((InplaceAddition<carl::MultivariatePolynomial<TypeParam>, TypeParam>));
}

TYPED_TEST(OperatorsInterval, InplaceMultiplication)
{
	//BOOST_CONCEPT_ASSERT((InplaceMultiplication<carl::Monomial::Arg, carl::Monomial::Arg>));
	//BOOST_CONCEPT_ASSERT((InplaceMultiplication<carl::Monomial::Arg, carl::Variable>));

	BOOST_CONCEPT_ASSERT((InplaceMultiplication<carl::Term<TypeParam>, carl::Term<TypeParam>>));
	//BOOST_CONCEPT_ASSERT((InplaceMultiplication<carl::Term<TypeParam>, carl::Monomial::Arg>));
	BOOST_CONCEPT_ASSERT((InplaceMultiplication<carl::Term<TypeParam>, carl::Variable>));
	BOOST_CONCEPT_ASSERT((InplaceMultiplication<carl::Term<TypeParam>, TypeParam>));

	BOOST_CONCEPT_ASSERT((InplaceMultiplication<carl::MultivariatePolynomial<TypeParam>, carl::MultivariatePolynomial<TypeParam>>));
	BOOST_CONCEPT_ASSERT((InplaceMultiplication<carl::MultivariatePolynomial<TypeParam>, carl::Term<TypeParam>>));
	//BOOST_CONCEPT_ASSERT((InplaceMultiplication<carl::MultivariatePolynomial<TypeParam>, carl::Monomial::Arg>));
	BOOST_CONCEPT_ASSERT((InplaceMultiplication<carl::MultivariatePolynomial<TypeParam>, carl::Variable>));
	BOOST_CONCEPT_ASSERT((InplaceMultiplication<carl::MultivariatePolynomial<TypeParam>, TypeParam>));
}
