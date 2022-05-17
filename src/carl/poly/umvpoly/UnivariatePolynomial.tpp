/** 
 * @file:   UnivariatePolynomial.tpp
 * @author: Sebastian Junges
 *
 * @since August 26, 2013
 */

#pragma once

#include <carl-common/debug/debug.h>
#include <carl-common/meta/platform.h>
#include <carl-common/meta/SFINAE.h>
#include <carl-logging/carl-logging.h>
#include "MultivariatePolynomial.h"
#include <carl/core/Sign.h>

#include "functions/Derivative.h"
#include "functions/Division.h"

#include <algorithm>
#include <iomanip>

namespace carl
{

template<typename Coeff>
UnivariatePolynomial<Coeff>::UnivariatePolynomial(const UnivariatePolynomial& p):
	mMainVar(p.mMainVar), mCoefficients(p.mCoefficients)
{
	assert(this->isConsistent());
}

template<typename Coeff>
UnivariatePolynomial<Coeff>::UnivariatePolynomial(UnivariatePolynomial&& p) noexcept:
	mMainVar(p.mMainVar), mCoefficients()
{
	mCoefficients = std::move(p.mCoefficients);
	assert(isConsistent());
}

template<typename Coeff>
UnivariatePolynomial<Coeff>& UnivariatePolynomial<Coeff>::operator=(const UnivariatePolynomial& p) {
	mMainVar = p.mMainVar;
	mCoefficients = p.mCoefficients;
	assert(isConsistent());
	return *this;
}

template<typename Coeff>
UnivariatePolynomial<Coeff>& UnivariatePolynomial<Coeff>::operator=(UnivariatePolynomial&& p) noexcept {
	mMainVar = p.mMainVar;
	mCoefficients = std::move(p.mCoefficients);
	assert(isConsistent());
	return *this;
}

template<typename Coeff>
UnivariatePolynomial<Coeff>::UnivariatePolynomial(Variable mainVar)
: mMainVar(mainVar), mCoefficients()
{
	assert(this->isConsistent());
}
template<typename Coeff>
UnivariatePolynomial<Coeff>::UnivariatePolynomial(Variable mainVar, const Coeff& coeff, std::size_t degree) :
mMainVar(mainVar),
mCoefficients(degree+1, Coeff(0)) // We would like to use 0 here, but Coeff(0) is not always constructable (some methods need more parameter)
{
	if(coeff != Coeff(0))
	{
		mCoefficients[degree] = coeff;
	}
	else
	{
		mCoefficients.clear();
	}
	stripLeadingZeroes();
 	assert(isConsistent());
}

template<typename Coeff>
UnivariatePolynomial<Coeff>::UnivariatePolynomial(Variable mainVar, std::initializer_list<Coeff> coefficients)
: mMainVar(mainVar), mCoefficients(coefficients)
{
	this->stripLeadingZeroes();
	assert(this->isConsistent());
}

template<typename Coeff>
template<typename C, DisableIf<std::is_same<C, typename UnderlyingNumberType<C>::type>>>
UnivariatePolynomial<Coeff>::UnivariatePolynomial(Variable mainVar, std::initializer_list<typename UnderlyingNumberType<C>::type> coefficients)
: mMainVar(mainVar), mCoefficients()
{
	for (auto c: coefficients) {
		this->mCoefficients.push_back(Coeff(c));
	}
	this->stripLeadingZeroes();
	assert(this->isConsistent());
}

template<typename Coeff>
UnivariatePolynomial<Coeff>::UnivariatePolynomial(Variable mainVar, const std::vector<Coeff>& coefficients)
: mMainVar(mainVar), mCoefficients(coefficients)
{
	this->stripLeadingZeroes();
	assert(this->isConsistent());
}

template<typename Coeff>
UnivariatePolynomial<Coeff>::UnivariatePolynomial(Variable mainVar, std::vector<Coeff>&& coefficients)
: mMainVar(mainVar), mCoefficients(coefficients)
{
	this->stripLeadingZeroes();
	assert(this->isConsistent());
}

template<typename Coeff>
UnivariatePolynomial<Coeff>::UnivariatePolynomial(Variable mainVar, const std::map<uint, Coeff>& coefficients)
: mMainVar(mainVar)
{
	mCoefficients.reserve(coefficients.rbegin()->first);
	for (const auto& expAndCoeff : coefficients)
	{
		if(expAndCoeff.first != mCoefficients.size())
		{
			mCoefficients.resize(expAndCoeff.first, Coeff(0));
		}
		mCoefficients.push_back(expAndCoeff.second);
	}
	this->stripLeadingZeroes();
	assert(this->isConsistent());
}

template<typename Coeff>
Coeff UnivariatePolynomial<Coeff>::evaluate(const Coeff& value) const 
{
	Coeff result(0);
	Coeff var(1);
	for(const Coeff& coeff : mCoefficients)
	{
		result += (coeff * var);
		var *= value;
	}
	return result;
}

template<typename Coeff>
bool UnivariatePolynomial<Coeff>::isNormal() const
{
	return unitPart() == Coeff(1);
}

template<typename Coefficient>
UnivariatePolynomial<Coefficient>& UnivariatePolynomial<Coefficient>::mod(const Coefficient& modulus)
{
	for(Coefficient& coeff : mCoefficients)
	{
		coeff = carl::mod(coeff, modulus);
	}
	return *this;
}

template<typename Coefficient>
UnivariatePolynomial<Coefficient> UnivariatePolynomial<Coefficient>::mod(const Coefficient& modulus) const
{
	UnivariatePolynomial<Coefficient> result;
	result.mCoefficients.reserve(mCoefficients.size());
	for(const Coefficient& coeff : mCoefficients)
	{
		result.mCoefficients.push_back(mod(coeff, modulus));
	}
	result.stripLeadingZeroes();
	return result;
}

template<typename Coeff>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::pow(std::size_t exp) const
{
	if (exp == 0) return UnivariatePolynomial(mMainVar, constant_one<Coeff>::get());
	if (carl::is_zero(*this)) return UnivariatePolynomial(mMainVar);
	UnivariatePolynomial<Coeff> res(mMainVar, constant_one<Coeff>::get());
	UnivariatePolynomial<Coeff> mult(*this);
	while(exp > 0) {
		if ((exp & 1) != 0) res *= mult;
		exp /= 2;
		if(exp > 0) mult = mult * mult;
	}
	return res;
}

template<typename Coeff>
template<typename C, DisableIf<is_number_type<C>>>
UnivariatePolynomial<typename UnivariatePolynomial<Coeff>::NumberType> UnivariatePolynomial<Coeff>::toNumberCoefficients() const {
	std::vector<NumberType> coeffs;
	coeffs.reserve(this->mCoefficients.size());
	for (auto c: this->mCoefficients) {
		assert(c.isConstant());
		coeffs.push_back(c.constantPart());
	}
	return UnivariatePolynomial<NumberType>(this->mMainVar, coeffs);
}

template<typename Coeff>
template<typename NewCoeff>
UnivariatePolynomial<NewCoeff> UnivariatePolynomial<Coeff>::convert() const {
	std::vector<NewCoeff> coeffs;
	coeffs.resize(this->mCoefficients.size());
	for (std::size_t i = 0; i < this->mCoefficients.size(); i++) {
		coeffs[i] = NewCoeff(this->mCoefficients[i]);
	}
	return UnivariatePolynomial<NewCoeff>(this->mMainVar, coeffs);
}

template<typename Coeff>
template<typename NewCoeff>
UnivariatePolynomial<NewCoeff> UnivariatePolynomial<Coeff>::convert(const std::function<NewCoeff(const Coeff&)>& f) const {
	std::vector<NewCoeff> coeffs;
	coeffs.resize(this->mCoefficients.size());
	for (std::size_t i = 0; i < this->mCoefficients.size(); i++) {
		coeffs[i] = f(this->mCoefficients[i]);
	}
	return UnivariatePolynomial<NewCoeff>(this->mMainVar, coeffs);
}

template<typename Coeff>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::normalized() const
{
	if(carl::is_zero(*this))
	{
		return *this;
	}
	return *this/unitPart();
}

template<typename Coeff>
Coeff UnivariatePolynomial<Coeff>::unitPart() const {
	if constexpr (is_field_type<Coeff>::value) {
		// Coeffs from a field
		return lcoeff();
	} else if constexpr (is_number_type<Coeff>::value) {
		// Coeffs from a number ring
		if (carl::is_zero(*this) || lcoeff() > Coeff(0)) {
			return Coeff(1);
		} else {
			return Coeff(-1);
		}
	} else {
		// Coeffs from a polynomial ring
		if (carl::is_zero(*this) || carl::is_zero(lcoeff()) || lcoeff().lcoeff() > NumberType(0)) {
			return Coeff(1);
		} else {
			return Coeff(-1);
		}
	}
}

template<typename Coeff>
template<typename C, EnableIf<is_subset_of_rationals_type<C>>>
Coeff UnivariatePolynomial<Coeff>::coprimeFactor() const
{
	assert(!carl::is_zero(*this));
	auto it = mCoefficients.begin();
	IntNumberType num = get_num(*it);
	IntNumberType den = get_denom(*it);
	for (++it; it != mCoefficients.end(); ++it) {
		num = carl::gcd(num, get_num(*it));
		den = carl::lcm(den, get_denom(*it));
	}
	return Coeff(den)/Coeff(num);
}

template<typename Coeff>
template<typename C, DisableIf<is_subset_of_rationals_type<C>>>
typename UnderlyingNumberType<Coeff>::type UnivariatePolynomial<Coeff>::coprimeFactor() const
{
	assert(!carl::is_zero(*this));
	auto it = mCoefficients.begin();
	typename UnderlyingNumberType<Coeff>::type factor = it->coprimeFactor();
	for (++it; it != mCoefficients.end(); ++it) {
		factor = carl::lcm(factor, it->coprimeFactor());
	}
	return factor;
}

template<typename Coeff>
template<typename C, EnableIf<is_subset_of_rationals_type<C>>>
UnivariatePolynomial<typename IntegralType<Coeff>::type> UnivariatePolynomial<Coeff>::coprimeCoefficients() const
{
	CARL_LOG_TRACE("carl.core", *this << " .coprimeCoefficients()");
	// Notice that even if factor is 1, we create a new polynomial
	UnivariatePolynomial<typename IntegralType<Coeff>::type> result(mMainVar);
	if (carl::is_zero(*this)) {
		return result;
	}
	result.mCoefficients.reserve(mCoefficients.size());
	Coeff factor = this->coprimeFactor();
	for (const Coeff& coeff: mCoefficients) {
		assert(get_denom(coeff * factor) == 1);
		result.mCoefficients.push_back(get_num(coeff * factor));
	}
	return result;
}

template<typename Coeff>
template<typename C, DisableIf<is_subset_of_rationals_type<C>>>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::coprimeCoefficients() const {
	if (carl::is_zero(*this)) return *this;
	UnivariatePolynomial<Coeff> result(mMainVar);
	result.mCoefficients.reserve(mCoefficients.size());
	auto factor = this->coprimeFactor();
	for (const Coeff& c: mCoefficients) {
		result.mCoefficients.push_back(factor * c);
	}
	return result;
}

template<typename Coeff>
template<typename C, EnableIf<is_subset_of_rationals_type<C>>>
UnivariatePolynomial<typename IntegralType<Coeff>::type> UnivariatePolynomial<Coeff>::coprimeCoefficientsSignPreserving() const
{
	CARL_LOG_TRACE("carl.core", *this << " .coprimeCoefficientsSignPreserving()");
	// Notice that even if factor is 1, we create a new polynomial
	UnivariatePolynomial<typename IntegralType<Coeff>::type> result(mMainVar);
	if (carl::is_zero(*this)) {
		return result;
	}
	result.mCoefficients.reserve(mCoefficients.size());
	Coeff factor = carl::abs(this->coprimeFactor());
	for (const Coeff& coeff: mCoefficients) {
		assert(get_denom(coeff * factor) == 1);
		result.mCoefficients.push_back(get_num(coeff * factor));
	}
	return result;
}

template<typename Coeff>
template<typename C, DisableIf<is_subset_of_rationals_type<C>>>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::coprimeCoefficientsSignPreserving() const {
	if (this->is_zero()) return *this;
	UnivariatePolynomial<Coeff> result(mMainVar);
	result.mCoefficients.reserve(mCoefficients.size());
	auto factor = carl::abs(this->coprimeFactor());
	for (const Coeff& c: mCoefficients) {
		result.mCoefficients.push_back(factor * c);
	}
	return result;
}

template<typename Coeff>
bool UnivariatePolynomial<Coeff>::divides(const UnivariatePolynomial& divisor) const
{
	///@todo Is this correct?
	return carl::is_zero(divisor.divideBy(*this).remainder);
}

template<typename Coeff>
template<typename C, EnableIf<is_instantiation_of<GFNumber, C>>>
UnivariatePolynomial<typename IntegralType<Coeff>::type> UnivariatePolynomial<Coeff>::toIntegerDomain() const
{
	UnivariatePolynomial<typename IntegralType<Coeff>::type> res(mMainVar);
	res.mCoefficients.reserve(mCoefficients.size());
	for(const Coeff& c : mCoefficients)
	{
		assert(carl::is_integer(c));
		res.mCoefficients.push_back(c.representing_integer());
	}
	res.stripLeadingZeroes();
	return res;
}

template<typename Coeff>
template<typename C, DisableIf<is_instantiation_of<GFNumber, C>>>
UnivariatePolynomial<typename IntegralType<Coeff>::type> UnivariatePolynomial<Coeff>::toIntegerDomain() const
{
	UnivariatePolynomial<typename IntegralType<Coeff>::type> res(mMainVar);
	res.mCoefficients.reserve(mCoefficients.size());
	for(const Coeff& c : mCoefficients)
	{
		assert(carl::is_integer(c));
		res.mCoefficients.push_back(get_num(c));
	}
	res.stripLeadingZeroes();
	return res;
}

template<typename Coeff>
//template<typename T = Coeff, EnableIf<!std::is_same<IntegralType<Coeff>, bool>::value>>
UnivariatePolynomial<GFNumber<typename IntegralType<Coeff>::type>> UnivariatePolynomial<Coeff>::toFiniteDomain(const GaloisField<typename IntegralType<Coeff>::type>* galoisField) const
{
	UnivariatePolynomial<GFNumber<typename IntegralType<Coeff>::type>> res(mMainVar);
	res.mCoefficients.reserve(mCoefficients.size());
	for(const Coeff& c : mCoefficients)
	{
		assert(carl::is_integer(c));
		res.mCoefficients.push_back(GFNumber<typename IntegralType<Coeff>::type>(c,galoisField));
	}
	res.stripLeadingZeroes();
	return res;
	
}

template<typename Coeff>
template<typename N, EnableIf<is_subset_of_rationals_type<N>>>
typename UnivariatePolynomial<Coeff>::NumberType UnivariatePolynomial<Coeff>::numericContent() const
{
	if (carl::is_zero(*this)) return NumberType(0);
	// Obtain main denominator for all coefficients.
	IntNumberType mainDenom = this->mainDenom();
	
	// now, some coefficient * mainDenom is always integral.
	// we convert such a product to an integral data type by get_num()
	UnivariatePolynomial<Coeff>::NumberType c = this->numericContent(0) * mainDenom;
	assert(get_denom(c) == 1);
	IntNumberType res = get_num(c);
	for (std::size_t i = 1; i < this->mCoefficients.size(); i++) {
		c = this->numericContent(i) * mainDenom;
		assert(get_denom(c) == 1);
		res = carl::gcd(get_num(c), res);
	}
	return NumberType(res) / mainDenom;
}

template<typename Coeff>
template<typename C, EnableIf<is_number_type<C>>>
typename UnivariatePolynomial<Coeff>::IntNumberType UnivariatePolynomial<Coeff>::mainDenom() const
{
	IntNumberType denom = 1;
	for (const auto& c: mCoefficients) {
		denom = carl::lcm(denom, get_denom(c));
	}
	CARL_LOG_TRACE("carl.core", "mainDenom of " << *this << " is " << denom);
	return denom;
}
template<typename Coeff>
template<typename C, DisableIf<is_number_type<C>>>
typename UnivariatePolynomial<Coeff>::IntNumberType UnivariatePolynomial<Coeff>::mainDenom() const
{
	IntNumberType denom = 1;
	for (const auto& c: mCoefficients) {
		denom = carl::lcm(denom, c.mainDenom());
	}
	return denom;
}

template<typename Coeff>
Coeff UnivariatePolynomial<Coeff>::syntheticDivision(const Coeff& zeroOfDivisor)
{
	if(coefficients().empty()) return Coeff(0);
	if(coefficients().size() == 1) return coefficients().back();
	std::vector<Coeff> secondRow;
	secondRow.reserve(coefficients().size());
	secondRow.push_back(Coeff(0));
	std::vector<Coeff> thirdRow(coefficients().size(), Coeff(0));
	size_t posThirdRow = coefficients().size()-1; 
	auto coeff = coefficients().rbegin();
	thirdRow[posThirdRow] = (*coeff) + secondRow.front();
	++coeff;
	while(coeff != coefficients().rend())
	{
		secondRow.push_back(zeroOfDivisor*thirdRow[posThirdRow]);
		--posThirdRow;
		thirdRow[posThirdRow] = (*coeff) + secondRow.back();
		++coeff;
	}
	assert(posThirdRow == 0);
	CARL_LOG_TRACE("carl.core.upoly", "UnivSynDiv: (" << *this << ")[x -> " << zeroOfDivisor << "]  =  " << thirdRow.front());
	if(thirdRow.front() == 0)
	{
		thirdRow.erase(thirdRow.begin());
		this->mCoefficients.swap(thirdRow);
		CARL_LOG_TRACE("carl.core.upoly", "UnivSynDiv: reduced by ((" << carl::abs(get_denom(thirdRow.front())) << ")*" << mainVar() << " + (" << (thirdRow.front()<0 ? "-" : "") << carl::abs(get_num(thirdRow.front())) << "))  ->  " << *this);
		return Coeff(0);
	}
	return thirdRow.front();
}

template<typename Coeff>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::operator -() const
{
	UnivariatePolynomial result(mMainVar);
	result.mCoefficients.reserve(mCoefficients.size());
	for(const auto& c : mCoefficients) {
		result.mCoefficients.push_back(-c);
	}

	return result;		 
}

template<typename Coefficient>
UnivariatePolynomial<Coefficient>& UnivariatePolynomial<Coefficient>::operator+=(const Coefficient& rhs)
{
	if(rhs == Coefficient(0)) return *this;
	if(mCoefficients.empty())
	{
		// Adding non-zero rhs to zero.
		mCoefficients.resize(1, rhs);
	}
	else
	{
		mCoefficients.front() += rhs;
		if(mCoefficients.size() == 1 && mCoefficients.front() == Coefficient(0)) 
		{
			// Result is zero.
			mCoefficients.clear();
		}
	}
	return *this;
}

template<typename Coeff>
UnivariatePolynomial<Coeff>& UnivariatePolynomial<Coeff>::operator+=(const UnivariatePolynomial& rhs)
{
	assert(mMainVar == rhs.mMainVar);
	
	if(carl::is_zero(rhs))
	{
		return *this;
	}
	
	if(mCoefficients.size() < rhs.mCoefficients.size())
	{
		for(std::size_t i = 0; i < mCoefficients.size(); ++i)
		{
			mCoefficients[i] += rhs.mCoefficients[i];
		}
		mCoefficients.insert(mCoefficients.end(), rhs.mCoefficients.end() - sint(rhs.mCoefficients.size() - mCoefficients.size()), rhs.mCoefficients.end());
	}
	else
	{
		for(std::size_t i = 0; i < rhs.mCoefficients.size(); ++i)
		{
			mCoefficients[i] += rhs.mCoefficients[i]; 
		}
	}
	stripLeadingZeroes();
	return *this;
}

template<typename C>
UnivariatePolynomial<C> operator+(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs)
{
	UnivariatePolynomial<C> res(lhs);
	res += rhs;
	return res;
}

template<typename C>
UnivariatePolynomial<C> operator+(const UnivariatePolynomial<C>& lhs, const C& rhs)
{
	UnivariatePolynomial<C> res(lhs);
	res += rhs;
	return res;
}

template<typename C>
UnivariatePolynomial<C> operator+(const C& lhs, const UnivariatePolynomial<C>& rhs)
{
	return rhs + lhs;
}
	

template<typename Coefficient>
UnivariatePolynomial<Coefficient>& UnivariatePolynomial<Coefficient>::operator-=(const Coefficient& rhs)
{
//	CARL_LOG_INEFFICIENT();
	return *this += -rhs;
}

template<typename Coeff>
UnivariatePolynomial<Coeff>& UnivariatePolynomial<Coeff>::operator-=(const UnivariatePolynomial& rhs)
{
//	CARL_LOG_INEFFICIENT();
	return *this += -rhs;
}


template<typename C>
UnivariatePolynomial<C> operator-(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs)
{
	UnivariatePolynomial<C> res(lhs);
	res -= rhs;
	return res;
}

template<typename C>
UnivariatePolynomial<C> operator-(const UnivariatePolynomial<C>& lhs, const C& rhs)
{
	UnivariatePolynomial<C> res(lhs);
	res -= rhs;
	return res;
}

template<typename C>
UnivariatePolynomial<C> operator-(const C& lhs, const UnivariatePolynomial<C>& rhs)
{
	return rhs - lhs;
}

template<typename Coefficient>
template<typename C, EnableIf<is_number_type<C>>>
UnivariatePolynomial<Coefficient>& UnivariatePolynomial<Coefficient>::operator*=(Variable rhs) {
	if (rhs == this->mMainVar) {
		this->mCoefficients.insert(this->mCoefficients.begin(), Coefficient(0));
		return *this;
	}
	assert(!carl::is_number_type<Coefficient>::value);
	return *this;
}
template<typename Coefficient>
template<typename C, DisableIf<is_number_type<C>>>
UnivariatePolynomial<Coefficient>& UnivariatePolynomial<Coefficient>::operator*=(Variable rhs) {
	if (rhs == this->mMainVar) {
		this->mCoefficients.insert(this->mCoefficients.begin(), Coefficient(0));
		return *this;
	}
	assert(!carl::is_number_type<Coefficient>::value);
	for (auto& c: this->mCoefficients) c *= rhs;
	return *this;
}

template<typename Coefficient>
UnivariatePolynomial<Coefficient>& UnivariatePolynomial<Coefficient>::operator*=(const Coefficient& rhs)
{
	if(rhs == Coefficient(0))
	{
		mCoefficients.clear();
		return *this;
	}
	for(Coefficient& c : mCoefficients)
	{
		c *= rhs;
	}
	
	if(is_finite_type<Coefficient>::value)
	{
		stripLeadingZeroes();
	}
	
	return *this;		
}


template<typename Coeff>
template<typename I, DisableIf<std::is_same<Coeff, I>>...>
UnivariatePolynomial<Coeff>& UnivariatePolynomial<Coeff>::operator*=(const typename IntegralType<Coeff>::type& rhs)
{
	static_assert(std::is_same<Coeff, I>::value, "Do not provide template parameters");
	if(rhs == I(0))
	{
		mCoefficients.clear();
		return *this;
	}
	for(Coeff& c : mCoefficients)
	{
		c *= rhs;
	}
	return *this;		
}

template<typename Coeff>
UnivariatePolynomial<Coeff>& UnivariatePolynomial<Coeff>::operator*=(const UnivariatePolynomial& rhs)
{
	assert(mMainVar == rhs.mMainVar);
	if(carl::is_zero(rhs))
	{
		mCoefficients.clear();
		return *this;
	}
	
	std::vector<Coeff> newCoeffs; 
	newCoeffs.reserve(mCoefficients.size() + rhs.mCoefficients.size());
	for(std::size_t e = 0; e < mCoefficients.size() + rhs.degree(); ++e)
	{
		newCoeffs.push_back(Coeff(0));
		for(std::size_t i = 0; i < mCoefficients.size() && i <= e; ++i)
		{
			if(e - i < rhs.mCoefficients.size())
			{
				newCoeffs.back() += mCoefficients[i] * rhs.mCoefficients[e-i];
			}
		}
	}
	mCoefficients.swap(newCoeffs);
	stripLeadingZeroes();
	return *this;
}


template<typename C>
UnivariatePolynomial<C> operator*(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs)
{
	UnivariatePolynomial<C> res(lhs);
	res *= rhs;
	return res;
}

template<typename C>
UnivariatePolynomial<C> operator*(const UnivariatePolynomial<C>& lhs, Variable rhs) {
	return std::move(UnivariatePolynomial<C>(lhs) *= rhs);
}
template<typename C>
UnivariatePolynomial<C> operator*(Variable lhs, const UnivariatePolynomial<C>& rhs) {
	return std::move(UnivariatePolynomial<C>(rhs) *= lhs);
}

template<typename C>
UnivariatePolynomial<C> operator*(const UnivariatePolynomial<C>& lhs, const C& rhs)
{
	UnivariatePolynomial<C> res(lhs);
	res *= rhs;
	return res;
}

template<typename C>
UnivariatePolynomial<C> operator*(const C& lhs, const UnivariatePolynomial<C>& rhs)
{
	return rhs * lhs;
}

template<typename C>
UnivariatePolynomial<C> operator*(const UnivariatePolynomial<C>& lhs, const IntegralTypeIfDifferent<C>& rhs)
{
	UnivariatePolynomial<C> res(lhs);
	res *= rhs;
	return res;
}

template<typename C>
UnivariatePolynomial<C> operator*(const IntegralTypeIfDifferent<C>& lhs, const UnivariatePolynomial<C>& rhs)
{
	return rhs * lhs;
}



template<typename Coeff>
template<typename C, EnableIf<is_field_type<C>>>
UnivariatePolynomial<Coeff>& UnivariatePolynomial<Coeff>::operator/=(const Coeff& rhs)
{
	assert(rhs != Coeff(0));
	for(Coeff& c : mCoefficients)
	{
		c /= rhs;
	}
	return *this;		
}

template<typename Coeff>
template<typename C, DisableIf<is_field_type<C>>>
UnivariatePolynomial<Coeff>& UnivariatePolynomial<Coeff>::operator/=(const Coeff& rhs)
{
	assert(rhs != Coeff(0));
	for(Coeff& c : mCoefficients)
	{
		c = quotient(c, rhs);
	}
	/// TODO not fully sure whether this is necessary
	this->stripLeadingZeroes();
	return *this;		
}



template<typename C>
UnivariatePolynomial<C> operator/(const UnivariatePolynomial<C>& lhs, const C& rhs)
{
	assert(rhs != C(0));
	if(carl::is_zero(lhs)) return lhs;
	UnivariatePolynomial<C> res(lhs);
	return res /= rhs;
}

template<typename C>
bool operator==(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs)
{
	assert(lhs.isConsistent());
	assert(rhs.isConsistent());
	if(lhs.mMainVar == rhs.mMainVar)
	{
		return lhs.mCoefficients == rhs.mCoefficients;
	}
	else
	{
		// in different variables, polynomials can still be equal if constant.
		if(carl::is_zero(lhs) && carl::is_zero(rhs)) return true;
		if ((lhs.mCoefficients.size() == 1) && (rhs.mCoefficients.size() == 1) && (lhs.mCoefficients == rhs.mCoefficients)) return true;
		// Convert to multivariate and compare that.
		return MultivariatePolynomial<typename carl::UnderlyingNumberType<C>::type>(lhs) == MultivariatePolynomial<typename carl::UnderlyingNumberType<C>::type>(rhs);
	}
}
template<typename C>
bool operator==(const UnivariatePolynomialPtr<C>& lhs, const UnivariatePolynomialPtr<C>& rhs)
{
	if (lhs == nullptr && rhs == nullptr) return true;
	if (lhs == nullptr || rhs == nullptr) return false;
	return *lhs == *rhs;
}

template<typename C>
bool operator==(const UnivariatePolynomial<C>& lhs, const C& rhs)
{	
	if (lhs.coefficients().size() == 0) {
		return carl::is_zero(rhs);
	}
	return (lhs.coefficients().size() == 1) && lhs.lcoeff() == rhs;
}

template<typename C>
bool operator==(const C& lhs, const UnivariatePolynomial<C>& rhs)
{
	return rhs == lhs;
}


template<typename C>
bool operator!=(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs)
{
	return !(lhs == rhs);
}
template<typename C>
bool operator!=(const UnivariatePolynomialPtr<C>& lhs, const UnivariatePolynomialPtr<C>& rhs)
{
	if (lhs == nullptr && rhs == nullptr) return false;
	if (lhs == nullptr || rhs == nullptr) return true;
	return *lhs != *rhs;
}

template<typename C>
bool UnivariatePolynomial<C>::less(const UnivariatePolynomial<C>& rhs, const PolynomialComparisonOrder& order) const {
	switch (order) {
		case PolynomialComparisonOrder::CauchyBound: /*{
			C a = this->cauchyBound();
			C b = rhs.cauchyBound();
			if (a < b) return true;
			return *this < rhs;
		}*/
		case PolynomialComparisonOrder::LowDegree:
			if (carl::is_zero(rhs)) return false;
			if (carl::is_zero(*this) || this->degree() < rhs.degree()) return true;
			return *this < rhs;
		case PolynomialComparisonOrder::Memory:
			return this < &rhs;
	}
	return false;
}

template<typename C>
bool operator<(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs)
{
	if(lhs.mMainVar == rhs.mMainVar)
	{
		if(lhs.coefficients().size() == rhs.coefficients().size())
		{
			auto iterLhs = lhs.coefficients().rbegin();
			auto iterRhs = rhs.coefficients().rbegin();
			while(iterLhs != lhs.coefficients().rend())
			{
				assert(iterRhs != rhs.coefficients().rend());
				if(*iterLhs == *iterRhs)
				{
					++iterLhs;
					++iterRhs;
				}
				else
				{
					return *iterLhs < *iterRhs;
				}
			}
		}
		return lhs.coefficients().size() < rhs.coefficients().size();
	}
	return lhs.mMainVar < rhs.mMainVar;
}

template<typename C>
std::ostream& operator<<(std::ostream& os, const UnivariatePolynomial<C>& rhs)
{
	if(carl::is_zero(rhs)) return os << "0";
	for(size_t i = 0; i < rhs.mCoefficients.size()-1; ++i )
	{
		const C& c = rhs.mCoefficients[rhs.mCoefficients.size()-i-1];
		if(!(c == 0))
		{
			if (!(c == 1)) os << "(" << c << ")*";
			os << rhs.mMainVar << "^" << rhs.mCoefficients.size()-i-1 << " + ";
		}
	}
	os << rhs.mCoefficients[0];
	return os;
}

template<typename Coefficient>
template<typename C, EnableIf<is_number_type<C>>>
bool UnivariatePolynomial<Coefficient>::isConsistent() const {
	if (!mCoefficients.empty()) {
		assert(!carl::is_zero(lcoeff()));
	}
	return true;
}

template<typename Coefficient>
template<typename C, DisableIf<is_number_type<C>>>
bool UnivariatePolynomial<Coefficient>::isConsistent() const {
	if (!mCoefficients.empty()) {
		assert(!carl::is_zero(lcoeff()));
	}
	for (const auto& c: mCoefficients) {
		assert(!c.has(mainVar()));
		assert(c.isConsistent());
	}
	return true;
}

}
