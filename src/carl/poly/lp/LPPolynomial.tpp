#pragma once

#include "LPPolynomial.h"

#if defined(USE_LIBPOLY) || defined(RAN_USE_LIBPOLY)

namespace carl {

LPPolynomial::LPPolynomial(const LPPolynomial& rhs)
	: mPoly(rhs.mPoly) {
	assert(lp_polynomial_check_order(mPoly.get_internal()));
}

LPPolynomial::LPPolynomial(LPPolynomial&& rhs)
	: mPoly(std::move(rhs.mPoly)) {
	assert(lp_polynomial_check_order(mPoly.get_internal()));
}

LPPolynomial& LPPolynomial::operator=(const LPPolynomial& rhs) {
	mPoly = rhs.mPoly;
	assert(lp_polynomial_check_order(mPoly.get_internal()));
	return *this;
}

LPPolynomial& LPPolynomial::operator=(LPPolynomial&& rhs) {
	mPoly = std::move(rhs.mPoly);
	assert(lp_polynomial_check_order(mPoly.get_internal()));
	return *this;
}

LPPolynomial::LPPolynomial(const LPContext& context)
	: mPoly(context.getContext()) {
	assert(lp_polynomial_check_order(mPoly.get_internal()));
}

LPPolynomial::LPPolynomial(const poly::Polynomial& p)
	: mPoly(p) {
	assert(lp_polynomial_check_order(mPoly.get_internal()));
}

LPPolynomial::LPPolynomial(poly::Polynomial&& p)
	: mPoly(std::move(p)) {
	assert(lp_polynomial_check_order(mPoly.get_internal()));
}

LPPolynomial::LPPolynomial(long val, const LPContext& context) {
	lp_polynomial_construct_simple(mPoly.get_internal(), context.getContext(), poly::Integer(val).get_internal(), 0, 0);
	assert(lp_polynomial_check_order(mPoly.get_internal()));
}

LPPolynomial::LPPolynomial(const LPContext& context, const Variable& var, const mpz_class& coeff, std::size_t degree) {
	lp_polynomial_construct_simple(mPoly.get_internal(), context.getContext(), poly::Integer(coeff).get_internal(), VariableMapper::getInstance().getLibpolyVariable(var).get_internal(), degree);
	assert(lp_polynomial_check_order(mPoly.get_internal()));
}

LPPolynomial::LPPolynomial(const LPContext& context, const Variable& var) {
	lp_polynomial_construct_simple(mPoly.get_internal(), context.getContext(), poly::Integer(1).get_internal(), VariableMapper::getInstance().getLibpolyVariable(var).get_internal(), 1);
	assert(lp_polynomial_check_order(mPoly.get_internal()));
}

LPPolynomial::LPPolynomial(const Variable& mainVar, const LPContext& context, const std::initializer_list<mpz_class>& coefficients)
	: mPoly(context.getContext()) {

	assert(context.has(mainVar));
	auto var = VariableMapper::getInstance().getLibpolyVariable(mainVar).get_internal();
	auto pow = coefficients.size();

	for (const mpz_class& coeff : coefficients) {
		pow--;
		poly::Polynomial temp;
		lp_polynomial_construct_simple(temp.get_internal(), context.getContext(), poly::Integer(coeff).get_internal(), var, pow);
		mPoly += temp;
	}
}

LPPolynomial::LPPolynomial(const Variable& mainVar, const LPContext& context, const std::vector<mpz_class>& coefficients)
	: mPoly(context.getContext()) {
	assert(context.has(mainVar));
	auto var = VariableMapper::getInstance().getLibpolyVariable(mainVar).get_internal();
	auto pow = coefficients.size();

	for (const mpz_class& coeff : coefficients) {
		pow--;
		poly::Polynomial temp;
		lp_polynomial_construct_simple(temp.get_internal(), context.getContext(), poly::Integer(coeff).get_internal(), var, pow);
		mPoly += temp;
	}
}

LPPolynomial::LPPolynomial(const Variable& mainVar, const LPContext& context, std::vector<mpz_class>&& coefficients)
	: mPoly(context.getContext()) {
	assert(context.has(mainVar));
	auto var = VariableMapper::getInstance().getLibpolyVariable(mainVar).get_internal();
	auto pow = coefficients.size();

	for (const mpz_class& coeff : coefficients) {
		pow--;
		poly::Polynomial temp;
		lp_polynomial_construct_simple(temp.get_internal(), context.getContext(), poly::Integer(std::move(coeff)).get_internal(), var, pow);
		mPoly += temp;
	}
}

LPPolynomial::LPPolynomial(const Variable& mainVar, const LPContext& context, const std::map<uint, mpz_class>& coefficients)
	: mPoly(context.getContext()) {
	assert(context.has(mainVar));
	auto var = VariableMapper::getInstance().getLibpolyVariable(mainVar).get_internal();

	for (const auto& coef : coefficients) {
		poly::Polynomial temp ; 
		lp_polynomial_construct_simple(temp.get_internal(), context.getContext(), poly::Integer(coef.second).get_internal(), var, coef.first);
		mPoly += temp;
	}
}

std::ostream& operator<<(std::ostream& os, const LPPolynomial& p) {
	os << p.mPoly;
	return os;
}

} // namespace carl

#endif
