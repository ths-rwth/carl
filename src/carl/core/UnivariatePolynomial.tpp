/** 
 * @file:   UnivariatePolynomial.tpp
 * @author: Sebastian Junges
 *
 * @since August 26, 2013
 */

#pragma once

#include "../converter/OldGinacConverter.h"
#include "../util/debug.h"
#include "../util/platform.h"
#include "../util/SFINAE.h"
#include "polynomialfunctions/GCD_multivariate.h"
#include "logging.h"
#include "MultivariatePolynomial.h"
#include "Sign.h"

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
		if(expAndCoeff.first != mCoefficients.size() + 1)
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
template<typename C, EnableIf<is_number<C>>>
void UnivariatePolynomial<Coeff>::substituteIn(Variable var, const Coeff& value) {
	if (this->isZero()) return;
	if (var == this->mainVar()) {
		this->mCoefficients[0] = this->evaluate(value);
		this->mCoefficients.resize(1);
	}
	this->stripLeadingZeroes();
	assert(this->isConsistent());
}

template<typename Coeff>
template<typename C, DisableIf<is_number<C>>>
void UnivariatePolynomial<Coeff>::substituteIn(Variable var, const Coeff& value) {
	if (this->isZero()) return;
	if (var == this->mainVar()) {
		this->mCoefficients[0] = this->evaluate(value);
		this->mCoefficients.resize(1);
	} else {
		if (value.has(var)) {
			// Fall back to multivariate substitution.
			MultivariatePolynomial<NumberType> tmp(*this);
			tmp.substituteIn(var, value);
			*this = tmp.toUnivariatePolynomial(this->mMainVar);
		} else {
			for (auto& c: mCoefficients) {
				c.substituteIn(var, value);
			}
		}
	}
	this->stripLeadingZeroes();
	assert(this->isConsistent());
}

template<typename Coeff>
template<typename C, EnableIf<is_number<C>>>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::substitute(Variable var, const Coeff& value) const {
	if (var == this->mainVar()) {
		return UnivariatePolynomial<Coeff>(this->mainVar(), this->evaluate(value));
	}
	return *this;
}

template<typename Coeff>
template<typename C, DisableIf<is_number<C>>>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::substitute(Variable var, const Coeff& value) const {
	if (var == this->mainVar()) {
		UnivariatePolynomial<Coeff> res(this->mainVar());
		for (const auto& c: mCoefficients) {
			res += c.substitute(var, value);
		}
		CARL_LOG_TRACE("carl.core.uvpolynomial", *this << " [ " << var << " -> " << value << " ] = " << res);
		return res;
	} else {
			if (value.has(var)) {
				// Fall back to multivariate substitution.
				MultivariatePolynomial<NumberType> tmp(*this);
				tmp.substituteIn(var, value);
				return tmp.toUnivariatePolynomial(this->mMainVar);
			} else {
		std::vector<Coeff> res(this->mCoefficients.size());
		for (std::size_t i = 0; i < res.size(); i++) {
			res[i] = this->mCoefficients[i].substitute(var, value);
		}
		UnivariatePolynomial<Coeff> resp(this->mainVar(), res);
		resp.stripLeadingZeroes();
		CARL_LOG_TRACE("carl.core.uvpolynomial", *this << " [ " << var << " -> " << value << " ] = " << resp);
		return resp;
			}
	}
}

template<typename Coeff>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::derivative(uint nth) const
{
	if (nth == 0) return *this;

	UnivariatePolynomial<Coeff> result(mMainVar);
	if (this->isConstant()) {
		return result;
	}
	result.mCoefficients.reserve(mCoefficients.size()-nth);
	// nth == 1 is most common case and can be implemented more efficient.
	if (nth == 1) {
		auto it = std::next(mCoefficients.begin());
		for (std::size_t i = 1; it != mCoefficients.end(); it++, i++) {
			result.mCoefficients.push_back(Coeff(i) * *it);
		}
		CARL_LOG_DEBUG("carl.core", "1st derivative of " << *this << " = " << result);
		return result;
	}
	else
	{
		// here we handle nth > 1.
		std::size_t c = 1;
		for (std::size_t k = 2; k <= nth; ++k) {
			c *= k;
		}
		auto it = mCoefficients.begin();
		std::size_t i = nth;
		for(it += sint(nth); it != mCoefficients.end(); ++it)
		{
			result.mCoefficients.push_back(Coeff(c) * *it);
			++i;
			c /= (i - nth);
			c *= i;
		}
		CARL_LOG_DEBUG("carl.core", nth << " derivative of " << *this << " = " << result);
		return result;
	}
}

template<typename Coeff>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::remainder_helper(const UnivariatePolynomial<Coeff>& divisor, const Coeff* prefactor) const
{
	assert(!divisor.isZero());
	if(this->isZero()) {
		return *this;
	}
	if(this->degree() < divisor.degree()) return *this;
	assert(degree() >= divisor.degree());
	// Remainder in a field is zero by definition.
	if (is_field<Coeff>::value && divisor.isConstant()) {
		return UnivariatePolynomial<Coeff>(mMainVar);
	}

	Coeff factor(0); // We have to initialize it to prevent a compiler error.
	if(prefactor != nullptr)
	{
		factor = carl::quotient(Coeff(*prefactor * lcoeff()), divisor.lcoeff());
		// There should be no remainder.
		assert(factor * divisor.lcoeff() == *prefactor * lcoeff());
	}
	else
	{
		factor = carl::quotient(lcoeff(), divisor.lcoeff());
		// There should be no remainder.
		assert(factor * divisor.lcoeff() == lcoeff());
	}

	UnivariatePolynomial<Coeff> result(mMainVar);
	result.mCoefficients.reserve(mCoefficients.size()-1);
	uint degdiff = degree() - divisor.degree();
	if(degdiff > 0)
	{
		result.mCoefficients.assign(mCoefficients.begin(), mCoefficients.begin() + long(degdiff));
	}
	if(prefactor != nullptr)
	{
		for(Coeff& c : result.mCoefficients)
		{
			c *= *prefactor;
		}
	}
	
	// By construction, the leading coefficient will be zero.
	if(prefactor != nullptr)
	{
		for(std::size_t i = 0; i < mCoefficients.size() - degdiff -1; ++i)
		{
			result.mCoefficients.push_back(mCoefficients[i + degdiff] * *prefactor - factor * divisor.mCoefficients[i]);
		}
	}
	else
	{
		for(std::size_t i = 0; i < mCoefficients.size() - degdiff -1; ++i)
		{
			result.mCoefficients.push_back(mCoefficients[i + degdiff] - factor * divisor.mCoefficients[i]);
		}
	}
	// strip zeros from the end as we might have pushed zeros.
	result.stripLeadingZeroes();
	
	if(result.isZero() || result.degree() < divisor.degree())
	{
		return result;
	}
	else 
	{	
		return result.remainder_helper(divisor, nullptr);
	}
}

template<typename Coeff>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::remainder(const UnivariatePolynomial<Coeff>& divisor, const Coeff& prefactor) const
{
	return this->remainder_helper(divisor, &prefactor);
}

template<typename Coeff>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::remainder(const UnivariatePolynomial<Coeff>& divisor) const
{
	static_assert(is_field<Coeff>::value, "Reduce must be called with a prefactor if the Coefficients are not from a field.");
	return this->remainder_helper(divisor);
}

/**
 * pseudoremainder
 */
template<typename Coeff>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::prem_old(const UnivariatePolynomial<Coeff>& divisor) const
{
	assert(!divisor.isZero());
	if(isZero() || degree() < divisor.degree())
	{
		// According to definition.
		return *this;
	}
	Coeff b = divisor.lcoeff();
	uint d = degree() - divisor.degree() + 1;
	Coeff prefactor = carl::pow(b,d);
	return remainder(divisor, prefactor);
}

template<typename Coeff>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::prem(const UnivariatePolynomial<Coeff>& divisor) const
{
	assert(this->mainVar() == divisor.mainVar());
	Variable v = this->mainVar();
	if (divisor.degree() == 0) return UnivariatePolynomial<Coeff>(v);
	if (divisor.degree() > this->degree()) return *this;

	UnivariatePolynomial<Coeff> reduct = divisor;
	reduct.truncate();
	UnivariatePolynomial<Coeff> res = *this;

	std::size_t reductions = 0;
	while (true) {
		if (res.isZero()) {
			assert(res == this->prem_old(divisor));
			return res;
		}
		if (divisor.degree() > res.degree()) {
			std::size_t degdiff = this->degree() - divisor.degree() + 1;
			if (reductions < degdiff) {
				res *= carl::pow(divisor.lcoeff(), degdiff - reductions);
			}
			assert(res == this->prem_old(divisor));
			return res;
		}
		std::vector<Coeff> newR(res.degree());
		Coeff lc = res.lcoeff();
		for (std::size_t i = 0; i < res.degree(); i++) {
			newR[i] = res.coefficients()[i] * divisor.lcoeff();
			assert(!newR[i].has(v));
		}
		if (res.degree() == divisor.degree()) {
			if (!reduct.isZero()) {
				for (std::size_t i = 0; i <= reduct.degree(); i++) {
					newR[i] -= lc * reduct.coefficients()[i];
					assert(!newR[i].has(v));
				}
			}
		} else {
			assert(!lc.has(v));
			if (!reduct.isZero()) {
				for (std::size_t i = 0; i <= reduct.degree(); i++) {
					newR[res.degree() - divisor.degree() + i] -= lc * reduct.coefficients()[i];
					assert(!newR[res.degree() - divisor.degree() + i].has(v));
				}
			}
		}
		res = UnivariatePolynomial<Coeff>(v, std::move(newR));
		reductions++;
	}
}

/**
 * Signed pseudoremainder.
 * see 
 * @param divisor
 * @return 
 */
template<typename Coeff>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::sprem(const UnivariatePolynomial<Coeff>& divisor) const
{
	if(isZero() || degree() < divisor.degree())
	{
		// According to definition.
		return *this;
	}
	Coeff b = divisor.lcoeff();
	uint d = degree() - divisor.degree() + 1;
	if (d % 2 == 1) ++d;
	Coeff prefactor = pow(b,d);
	return remainder(divisor, &prefactor);
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
	if (isZero()) return UnivariatePolynomial(mMainVar);
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
template<typename C, DisableIf<is_number<C>>>
UnivariatePolynomial<typename UnivariatePolynomial<Coeff>::NumberType> UnivariatePolynomial<Coeff>::toNumberCoefficients(bool check) const {
	std::vector<NumberType> coeffs;
	coeffs.reserve(this->mCoefficients.size());
	for (auto c: this->mCoefficients) {
		if (check) assert(c.isConstant());
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
Coeff UnivariatePolynomial<Coeff>::cauchyBound() const
{
	// We could also use SFINAE, but this gives clearer error messages.
	// Just in case, if we want to use SFINAE, the right statement would be
	// template<typename t = Coefficient, typename std::enable_if<is_field<t>::value, int>::type = 0>
	static_assert(is_field<Coeff>::value, "Cauchy bounds are only defined for field-coefficients");
	// For constants, the cauchy bound is always zero.
	if (this->mCoefficients.size() <= 1) {
		return Coeff(0);
	}
	Coeff maxCoeff = carl::abs(mCoefficients.front());
	for(typename std::vector<Coeff>::const_iterator it = ++mCoefficients.begin(); it != --mCoefficients.end(); ++it)
	{
		Coeff absOfCoeff = carl::abs( *it );
		if(absOfCoeff > maxCoeff) 
		{
			maxCoeff = absOfCoeff;
		}
	}
	return 1 + maxCoeff/carl::abs(lcoeff());
}

template<typename Coeff>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::normalized() const
{
	if(isZero())
	{
		return *this;
	}
	return *this/unitPart();
}



template<typename Coeff>
template<typename C, EnableIf<is_field<C>>>
const Coeff& UnivariatePolynomial<Coeff>::unitPart() const
{
	return lcoeff();
}

#ifdef __VS
template<typename Coeff>
template<typename C, EnableIfBool<!is_number<C>::value>>
#else
template<typename Coeff>
template<typename C, EnableIf<Not<is_number<C>>>>
#endif
Coeff UnivariatePolynomial<Coeff>::unitPart() const
{
	if(isZero() || lcoeff().isZero() || lcoeff().lcoeff() > NumberType(0))
	{
		return Coeff(1);
	}	
	else
	{
		return Coeff(-1);
	}
}

#ifdef __VS
template<typename Coeff>
template<typename C, EnableIfBool<!is_field<C>::value && is_number<C>::value >>
#else
template<typename Coeff>
template<typename C, EnableIf<Not<is_field<C>>, is_number<C> >>
#endif
Coeff UnivariatePolynomial<Coeff>::unitPart() const
{
	if(isZero() || lcoeff() > Coeff(0))
	{
		return Coeff(1);
	}
	else
	{
		return Coeff(-1);
	}
}

template<typename Coeff>
template<typename C, EnableIf<is_subset_of_rationals<C>>>
Coeff UnivariatePolynomial<Coeff>::coprimeFactor() const
{
	assert(!this->isZero());
	auto it = mCoefficients.begin();
	IntNumberType num = getNum(*it);
	IntNumberType den = getDenom(*it);
	for (++it; it != mCoefficients.end(); ++it) {
		num = carl::gcd(num, getNum(*it));
		den = carl::lcm(den, getDenom(*it));
	}
	return Coeff(den)/Coeff(num);
}

template<typename Coeff>
template<typename C, DisableIf<is_subset_of_rationals<C>>>
typename UnderlyingNumberType<Coeff>::type UnivariatePolynomial<Coeff>::coprimeFactor() const
{
	assert(!this->isZero());
	auto it = mCoefficients.begin();
	typename UnderlyingNumberType<Coeff>::type factor = it->coprimeFactor();
	for (++it; it != mCoefficients.end(); ++it) {
		factor = carl::gcd(factor, it->coprimeFactor());
	}
	return factor;
}

template<typename Coeff>
template<typename C, EnableIf<is_subset_of_rationals<C>>>
UnivariatePolynomial<typename IntegralType<Coeff>::type> UnivariatePolynomial<Coeff>::coprimeCoefficients() const
{
	CARL_LOG_TRACE("carl.core", *this << " .coprimeCoefficients()");
	// Notice that even if factor is 1, we create a new polynomial
	UnivariatePolynomial<typename IntegralType<Coeff>::type> result(mMainVar);
	if (this->isZero()) {
		return result;
	}
	result.mCoefficients.reserve(mCoefficients.size());
	Coeff factor = this->coprimeFactor();
	for (const Coeff& coeff: mCoefficients) {
		assert(getDenom(coeff * factor) == 1);
		result.mCoefficients.push_back(getNum(coeff * factor));
	}
	return result;
}

template<typename Coeff>
template<typename C, DisableIf<is_subset_of_rationals<C>>>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::coprimeCoefficients() const {
	if (this->isZero()) return *this;
	UnivariatePolynomial<Coeff> result(mMainVar);
	result.mCoefficients.reserve(mCoefficients.size());
	auto factor = this->coprimeFactor();
	for (const Coeff& c: mCoefficients) {
		result.mCoefficients.push_back(factor * c);
	}
	return result;
}

template<typename Coeff>
template<typename C, EnableIf<is_integer<C>>>
DivisionResult<UnivariatePolynomial<Coeff>> UnivariatePolynomial<Coeff>::divideBy(const UnivariatePolynomial<Coeff>& divisor) const
{
	assert(!divisor.isZero());
	DivisionResult<UnivariatePolynomial<Coeff>> result(UnivariatePolynomial<Coeff>(mMainVar), *this);
	if(isZero()) return result;
	assert(*this == divisor * result.quotient + result.remainder);

	result.quotient.mCoefficients.resize(1+mCoefficients.size()-divisor.mCoefficients.size(), Coeff(0));

	uint degdiff = this->degree() - divisor.degree();
	for (std::size_t offset = 0; offset <= degdiff; offset++) {
		Coeff factor = carl::quotient(result.remainder.mCoefficients[this->degree()-offset], divisor.lcoeff());
		result.remainder -= UnivariatePolynomial<Coeff>(mMainVar, factor, degdiff - offset) * divisor;
		result.quotient.mCoefficients[degdiff-offset] += factor;
	}
	assert(*this == divisor * result.quotient + result.remainder);
	return result;
}

template<typename Coeff>
template<typename C, DisableIf<is_integer<C>>, EnableIf<is_field<C>>>
DivisionResult<UnivariatePolynomial<Coeff>> UnivariatePolynomial<Coeff>::divideBy(const UnivariatePolynomial<Coeff>& divisor) const
{
	assert(!divisor.isZero());
	assert(this->mainVar() == divisor.mainVar());
	DivisionResult<UnivariatePolynomial<Coeff>> result(UnivariatePolynomial<Coeff>(mMainVar), *this);
	if(isZero()) return result;
	assert(*this == divisor * result.quotient + result.remainder);
	if(divisor.degree() > degree())
	{
		return result;
	}
	result.quotient.mCoefficients.resize(1+mCoefficients.size()-divisor.mCoefficients.size(), Coeff(0));
	
	do
	{
		Coeff factor = result.remainder.lcoeff()/divisor.lcoeff();
		uint degdiff = result.remainder.degree() - divisor.degree();
		result.remainder -= UnivariatePolynomial<Coeff>(mMainVar, factor, degdiff) * divisor;
		result.quotient.mCoefficients[degdiff] += factor;
	}
	while(!result.remainder.isZero() && divisor.degree() <= result.remainder.degree());
	
	assert(*this == divisor * result.quotient + result.remainder);
	return result;
}

template<typename Coeff>
template<typename C, EnableIf<is_field<C>>>
DivisionResult<UnivariatePolynomial<Coeff>> UnivariatePolynomial<Coeff>::divideBy(const Coeff& divisor) const 
{
	UnivariatePolynomial<Coeff> res(*this);
	for (auto& c: res.mCoefficients) {
		c = c / divisor;
	}
	return DivisionResult<UnivariatePolynomial<Coeff>>(res, UnivariatePolynomial(this->mainVar()));
}

template<typename Coeff>
template<typename C, DisableIf<is_field<C>>, DisableIf<is_number<C>>>
bool UnivariatePolynomial<Coeff>::divideBy(const Coeff& divisor, UnivariatePolynomial<Coeff>& quotient) const 
{
	assert(this->isConsistent());
	assert(divisor.isConsistent());
	Coeff quo;
	bool res = Coeff(*this).divideBy(divisor, quo);
	CARL_LOG_TRACE("carl.core", Coeff(*this) << " / " << divisor << " = " << quo);
	assert(quo.isConsistent());
	if (res) quotient = quo.toUnivariatePolynomial(this->mainVar());
	return res;
}

template<typename Coeff>
template<typename C, DisableIf<is_field<C>>, DisableIf<is_number<C>>, EnableIf<is_field<typename UnderlyingNumberType<C>::type>>>
DivisionResult<UnivariatePolynomial<Coeff>> UnivariatePolynomial<Coeff>::divideBy(const typename UnivariatePolynomial<Coeff>::NumberType& divisor) const 
{
	UnivariatePolynomial<Coeff> res(*this);
	assert(res.isConsistent());
	for (auto& c: res.mCoefficients) {
		c = c.divideBy(divisor);
	}
	assert(res.isConsistent());
	return DivisionResult<UnivariatePolynomial<Coeff>>(res, UnivariatePolynomial(this->mainVar()));
}

template<typename Coeff>
bool UnivariatePolynomial<Coeff>::divides(const UnivariatePolynomial& divisor) const
{
	///@todo Is this correct?
	return divisor.divideBy(*this).remainder.isZero();
}

template<typename Coeff>
Coeff UnivariatePolynomial<Coeff>::modifiedCauchyBound() const
{
	// We could also use SFINAE, but this gives clearer error messages.
	// Just in case, if we want to use SFINAE, the right statement would be
	// template<typename t = Coefficient, typename std::enable_if<is_field<t>::value, int>::type = 0>
	static_assert(is_field<Coeff>::value, "Modified Cauchy bounds are only defined for field-coefficients");
	CARL_LOG_NOTIMPLEMENTED();
}

template<typename Coeff>
template<typename C, EnableIf<is_subset_of_rationals<C>>>
typename UnivariatePolynomial<Coeff>::IntNumberType UnivariatePolynomial<Coeff>::maximumNorm() const {
	typename std::vector<C>::const_iterator it = mCoefficients.begin();
	Coeff max = carl::abs(*it);
	IntNumberType num = carl::abs(getNum(*it));
	IntNumberType den = carl::abs(getDenom(*it));
	for (++it; it != mCoefficients.end(); ++it) {
		auto tmp = carl::abs(*it);
		if (tmp > max) max = tmp;
		num = carl::gcd(num, getNum(tmp));
		den = carl::lcm(den, getDenom(tmp));
	}
	assert(getDenom(Coeff(max*den/num)) == 1);
	return getNum(Coeff(max*den/num));
}

template<typename Coeff>
template<typename C, EnableIf<is_instantiation_of<GFNumber, C>>>
UnivariatePolynomial<typename IntegralType<Coeff>::type> UnivariatePolynomial<Coeff>::toIntegerDomain() const
{
	UnivariatePolynomial<typename IntegralType<Coeff>::type> res(mMainVar);
	res.mCoefficients.reserve(mCoefficients.size());
	for(const Coeff& c : mCoefficients)
	{
		assert(carl::isInteger(c));
		res.mCoefficients.push_back(c.representingInteger());
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
		assert(carl::isInteger(c));
		res.mCoefficients.push_back(getNum(c));
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
		assert(carl::isInteger(c));
		res.mCoefficients.push_back(GFNumber<typename IntegralType<Coeff>::type>(c,galoisField));
	}
	res.stripLeadingZeroes();
	return res;
	
}

template<typename Coeff>
template<typename N, EnableIf<is_subset_of_rationals<N>>>
typename UnivariatePolynomial<Coeff>::NumberType UnivariatePolynomial<Coeff>::numericContent() const
{
	if (this->isZero()) return NumberType(0);
	// Obtain main denominator for all coefficients.
	IntNumberType mainDenom = this->mainDenom();
	
	// now, some coefficient * mainDenom is always integral.
	// we convert such a product to an integral data type by getNum()
	UnivariatePolynomial<Coeff>::NumberType c = this->numericContent(0) * mainDenom;
	assert(getDenom(c) == 1);
	IntNumberType res = getNum(c);
	for (std::size_t i = 1; i < this->mCoefficients.size(); i++) {
		c = this->numericContent(i) * mainDenom;
		assert(getDenom(c) == 1);
		res = carl::gcd(getNum(c), res);
	}
	return NumberType(res) / mainDenom;
}

template<typename Coeff>
template<typename C, EnableIf<is_number<C>>>
typename UnivariatePolynomial<Coeff>::IntNumberType UnivariatePolynomial<Coeff>::mainDenom() const
{
	IntNumberType denom = 1;
	for (const auto& c: mCoefficients) {
		denom = carl::lcm(denom, getDenom(c));
	}
	CARL_LOG_TRACE("carl.core", "mainDenom of " << *this << " is " << denom);
	return denom;
}
template<typename Coeff>
template<typename C, DisableIf<is_number<C>>>
typename UnivariatePolynomial<Coeff>::IntNumberType UnivariatePolynomial<Coeff>::mainDenom() const
{
	IntNumberType denom = 1;
	for (const auto& c: mCoefficients) {
		denom = carl::lcm(denom, c.mainDenom());
	}
	return denom;
}

template<typename Coeff>
template<typename Integer>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::excludeLinearFactors(const UnivariatePolynomial<Coeff>& poly, FactorMap<Coeff>& linearFactors, const Integer& maxInt)
{
	CARL_LOG_TRACE("carl.core.upoly", "UnivELF: " << poly );
	UnivariatePolynomial<Coeff> result(poly.mainVar());
	// Exclude the factor x^i from result.
	auto cf = poly.coefficients().begin();
	if(*cf == Coeff(0)) // result is of the form a_n * x^n + ... + a_k * x^k (a>k, k>0)
	{
		uint k = 0;
		while(*cf == Coeff(0))
		{
			assert(cf != poly.coefficients().end());
			++cf;
			++k;
		}
		// Take x^k as a factor.
		auto retVal = linearFactors.emplace(UnivariatePolynomial<Coeff>(poly.mainVar(), {Coeff(0), Coeff(1)}), k);
		CARL_LOG_TRACE("carl.core.upoly", "UnivELF: add the factor (" << retVal.first->first << ")^" << k );
		if(!retVal.second)
		{
			retVal.first->second += k;
		}
		// Construct the remainder:  result := a_n * x^{n-k} + ... + a_{k-1} * x + a_k
		std::vector<Coeff> cfs;
		cfs.reserve(poly.coefficients().size()-k);
		cfs = std::vector<Coeff>(cf, poly.coefficients().end());
		result = UnivariatePolynomial<Coeff>(poly.mainVar(), std::move(cfs));
		CARL_LOG_TRACE("carl.core.upoly", "UnivELF: remainder is  " << result );
	}
	else
	{
		result = poly;
	}
	// Check whether the polynomial is already a linear factor.
	if(!result.isLinearInMainVar())
	{
		// Exclude the factor (x-r)^i, with r rational and r!=0, from result.
		assert(result.coefficients().size() > 1);
		typename IntegralType<Coeff>::type lc = carl::abs(getNum(result.lcoeff()));
		typename IntegralType<Coeff>::type tc = carl::abs(getNum(result.coefficients().front()));
		typename IntegralType<Coeff>::type mi = carl::fromInt<typename IntegralType<Coeff>::type>(maxInt);
		if( maxInt != 0 && (tc > mi || lc > mi) )
		{
			return result;
		}
		Integer lcAsInt = toInt<Integer>(lc);
		Integer tcAsInt = toInt<Integer>(tc);
		Integer halfOfLcAsInt = lcAsInt == 1 ? 1 : lcAsInt/2;
		Integer halfOfTcAsInt = tcAsInt == 1 ? 1 : tcAsInt/2;
		std::vector<std::pair<Integer, Integer>> shiftedTcs;
		bool positive = true;
		bool tcFactorsFound = false;
		std::vector<Integer> tcFactors(1, 1); // TODO: store the divisors of some numbers during compilation
		auto tcFactor = tcFactors.begin();
		bool lcFactorsFound = false;
		std::vector<Integer> lcFactors(1, 1); // TODO: store the divisors of some numbers during compilation
		auto lcFactor = lcFactors.begin();
		while(true)
		{
			CARL_LOG_TRACE("carl.core.upoly", "UnivELF: try rational  " << (positive ? "-" : "") << *tcFactor << "/" << *lcFactor);
			// Check whether the numerator of the rational to consider divides the trailing coefficient of all
			// zero-preserving shifts {result(x+x_0) | for some found x_0 with result(x_0)!=0 and x_0 integer}
			auto shiftedTc = shiftedTcs.begin();
			for(; shiftedTc != shiftedTcs.end(); ++shiftedTc)
			{
				// we need to be careful with overflows in the following lines
				if(maxInt/(*lcFactor) >= shiftedTc->first)
				{
					Integer divisor = (*lcFactor) * shiftedTc->first;
					if( divisor != *tcFactor )
					{
						if( !(divisor < 0 && *tcFactor < 0 && maxInt + divisor >= -(*tcFactor)) && !(divisor > 0 && *tcFactor > 0 && maxInt - divisor >= *tcFactor ) )
						{
							if( divisor > *tcFactor )
							{
								divisor = divisor - *tcFactor;
							}
							else
							{
								divisor = *tcFactor - divisor;
							}
							if(carl::mod(shiftedTc->second, divisor) != 0)
							{
								break;
							}
						}
					}	
				}
			}
			if(shiftedTc == shiftedTcs.end())
			{
				Coeff posRatZero = carl::fromInt<typename IntegralType<Coeff>::type>(*tcFactor) / carl::fromInt<typename IntegralType<Coeff>::type>(*lcFactor);
				if (!positive) posRatZero = -posRatZero;
				CARL_LOG_TRACE("carl.core.upoly", "UnivELF: consider possible non zero rational factor  " << posRatZero);
				Coeff image = result.syntheticDivision(posRatZero);
				if (carl::isZero(image)) {
					// Remove all linear factor with the found zero from result.
					UnivariatePolynomial<Coeff> linearFactor(result.mainVar(), {-posRatZero, Coeff(1)});
					while (carl::isZero(image)) {
						auto retVal = linearFactors.emplace(linearFactor, 1);
						CARL_LOG_TRACE("carl.core.upoly", "UnivELF: add the factor (" << linearFactor << ")^" << 1 );
						if(!retVal.second)
						{
							++retVal.first->second;
						}
						// Check whether result is a linear factor now.
						if(result.isLinearInMainVar() <= 1)
						{
							goto LinearFactorRemains;
						}
						image = result.syntheticDivision(posRatZero);
					}
				}
				else if(isInteger(posRatZero))
				{
					// Add a zero-preserving shift.
					assert(isInteger(image));
					typename IntegralType<Coeff>::type imageInt = carl::abs(getNum(image));
					if( imageInt <= carl::fromInt<IntNumberType>(maxInt) )
					{
						CARL_LOG_TRACE("carl.core.upoly", "UnivELF: new shift with " << getNum(posRatZero) << " to " << carl::abs(getNum(image)));
						shiftedTcs.push_back(std::pair<Integer, Integer>(toInt<Integer>(getNum(posRatZero)), toInt<Integer>(carl::abs(getNum(image)))));
					}
				}
			}
			// Find the next numerator-denominator combination.
			if(shiftedTc == shiftedTcs.end() && positive)
			{
				positive = false;
			}
			else
			{
				positive = true;
				if(lcFactorsFound)
				{
					++lcFactor;
				}
				else
				{
					lcFactors.push_back(lcFactors.back());
					while(lcFactors.back() <= halfOfLcAsInt)
					{
						++lcFactors.back();
						if(carl::mod(lcAsInt, lcFactors.back()) == 0)
						{
							break;
						}
					}
					if(lcFactors.back() > halfOfLcAsInt)
					{
						lcFactors.pop_back();
						lcFactorsFound = true;
						lcFactor = lcFactors.end();
					}
					else
					{
						lcFactor = --(lcFactors.end());
					}
				}
				if(lcFactor == lcFactors.end())
				{
					if(tcFactorsFound)
					{
						++tcFactor;
					}
					else
					{
						tcFactors.push_back(tcFactors.back());
						while(tcFactors.back() <= halfOfTcAsInt)
						{
							++(tcFactors.back());
							if(carl::mod(tcAsInt, tcFactors.back()) == 0)
							{
								break;
							}
						}
						if(tcFactors.back() > halfOfTcAsInt)
						{
							tcFactors.pop_back();
							tcFactor = tcFactors.end();
						}
						else
						{
							tcFactor = --(tcFactors.end());
						}
					}
					if(tcFactor == tcFactors.end())
					{
						Coeff factor = result.coprimeFactor();
						if (!carl::isOne(factor)) {
							result *= factor;
							CARL_LOG_TRACE("carl.core.upoly", "UnivFactor: add the factor (" << UnivariatePolynomial<Coeff>(result.mainVar(), std::initializer_list<Coeff>({(Coeff)1/factor})) << ")^" << 1 );
							// Add the constant factor to the factors.
							if( linearFactors.begin()->first.isConstant() )
							{
								factor = Coeff(1) / factor;
								factor *= linearFactors.begin()->first.lcoeff();
								linearFactors.erase(linearFactors.begin());
							}
							linearFactors.insert(linearFactors.begin(), std::pair<UnivariatePolynomial<Coeff>, uint>(UnivariatePolynomial<Coeff>(result.mainVar(), std::initializer_list<Coeff>({factor})), 1));
						}
						return result;
					}
					lcFactor = lcFactors.begin();
				}
			}
		}
		assert(false);
	}
LinearFactorRemains:
	Coeff factor = result.lcoeff();
	if (!carl::isOne(factor)) {
		result /= factor;
		CARL_LOG_TRACE("carl.core.upoly", "UnivFactor: add the factor (" << UnivariatePolynomial<Coeff>(result.mainVar(), factor) << ")^" << 1 );
		// Add the constant factor to the factors.
		if( !linearFactors.empty() && linearFactors.begin()->first.isConstant() )
		{
			if( linearFactors.begin()->first.isZero() )
				factor = Coeff( 0 );
			else
				factor *= linearFactors.begin()->first.lcoeff();
			linearFactors.erase(linearFactors.begin());
		}
		linearFactors.insert(linearFactors.begin(), std::pair<UnivariatePolynomial<Coeff>, uint>(UnivariatePolynomial<Coeff>(result.mainVar(), factor), 1));
	}
	auto retVal = linearFactors.emplace(result, 1);
	CARL_LOG_TRACE("carl.core.upoly", "UnivELF: add the factor (" << result << ")^" << 1 );
	if(!retVal.second)
	{
		++retVal.first->second;
	}
	return UnivariatePolynomial<Coeff>(result.mainVar(), Coeff(1));
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
		CARL_LOG_TRACE("carl.core.upoly", "UnivSynDiv: reduced by ((" << carl::abs(getDenom(thirdRow.front())) << ")*" << mainVar() << " + (" << (thirdRow.front()<0 ? "-" : "") << carl::abs(getNum(thirdRow.front())) << "))  ->  " << *this);
		return Coeff(0);
	}
	return thirdRow.front();
}

template<typename Coeff>
void UnivariatePolynomial<Coeff>::eliminateZeroRoots() {
	std::size_t i = 0;
	while ((i < this->mCoefficients.size()) && (this->mCoefficients[i] == Coeff(0))) i++;
	if (i == 0) return;
	// Now shift by i elements, drop lower i coefficients (they are zero anyway)
	for (std::size_t j = 0; j < this->mCoefficients.size()-i; j++) {
		this->mCoefficients[j] = this->mCoefficients[j+i];
	}
	this->mCoefficients.resize(this->mCoefficients.size()-i);
	assert(this->isConsistent());
}

template<typename Coeff>
void UnivariatePolynomial<Coeff>::eliminateRoot(const Coeff& root) {
	assert(this->isRoot(root));
	if (this->isZero()) return;
	if (root == Coeff(0)) {
		this->eliminateZeroRoots();
		return;
	}
	do {
		std::vector<Coeff> tmp(this->mCoefficients.size()-1);
		for (std::size_t i = this->mCoefficients.size()-1; i > 0; i--) {
			tmp[i-1] = this->mCoefficients[i];
			this->mCoefficients[i-1] += this->mCoefficients[i] * root;
		}
		this->mCoefficients = tmp;
	} while ((this->evaluate(root) == Coeff(0)) && (this->mCoefficients.size() > 0));
}

template<typename Coeff>
std::list<UnivariatePolynomial<Coeff>> UnivariatePolynomial<Coeff>::standardSturmSequence() const {
	return this->standardSturmSequence(this->derivative());
}

template<typename Coeff>
std::list<UnivariatePolynomial<Coeff>> UnivariatePolynomial<Coeff>::standardSturmSequence(const UnivariatePolynomial<Coeff>& polynomial) const {
	assert(this->mainVar() == polynomial.mainVar());
	std::list<UnivariatePolynomial<Coeff>> seq;

	UnivariatePolynomial<Coeff> p = *this;
	UnivariatePolynomial<Coeff> q = polynomial;

	seq.push_back(p);
	while (! q.isZero()) {
		seq.push_back(q);
		q = - p.remainder(q);
		p = seq.back();
	}
	return seq;
}

template<typename Coeff>
uint UnivariatePolynomial<Coeff>::signVariations(const Interval<Coeff>& interval) const {
	if (interval.isEmpty()) return 0;
	if (interval.isPointInterval()) {
		std::vector<Coeff> vals;
		Coeff factor = carl::constant_one<Coeff>::get();
		for (const auto& c: mCoefficients) {
			vals.push_back(c * factor);
			factor *= interval.lower();
		}
		auto res = carl::signVariations(vals.begin(), vals.end(), [](const Coeff& c){ return carl::sgn(c); });
		CARL_LOG_TRACE("carl.core", *this << " has " << res << " sign variations at " << interval.lower());
		return res;
	}
	UnivariatePolynomial<Coeff> p(*this);
	p.shift(interval.lower());
	p.scale(interval.diameter());
	p.reverse();
	p.shift(1);
	p.stripLeadingZeroes();
	assert(p.isConsistent());
	auto res = carl::signVariations(p.mCoefficients.begin(), p.mCoefficients.end(), [](const Coeff& c){ return carl::sgn(c); });
	CARL_LOG_TRACE("carl.core", *this << " has " << res << " sign variations within " << interval);
	return res;
}

template<typename Coeff>
int UnivariatePolynomial<Coeff>::countRealRoots(const Interval<Coeff>& interval) const {
	assert(!this->isZero());
	assert(!this->isRoot(interval.lower()));
	assert(!this->isRoot(interval.upper()));
	return UnivariatePolynomial<Coeff>::countRealRoots(this->standardSturmSequence(), interval);
}

template<typename Coeff>
template<typename C, typename Number>
int UnivariatePolynomial<Coeff>::countRealRoots(const std::list<UnivariatePolynomial<Coeff>>& seq, const Interval<Number>& interval) {
	int l = int(carl::signVariations(seq.begin(), seq.end(), [&interval](const UnivariatePolynomial<Coeff>& p){ return p.sgn(interval.lower()); }));
	int r = int(carl::signVariations(seq.begin(), seq.end(), [&interval](const UnivariatePolynomial<Coeff>& p){ return p.sgn(interval.upper()); }));
	return l - r;
}


template<typename Coeff>
void UnivariatePolynomial<Coeff>::reverse() {
	std::reverse(this->mCoefficients.begin(), this->mCoefficients.end());
}

template<typename Coeff>
void UnivariatePolynomial<Coeff>::scale(const Coeff& factor) {
	Coeff f = factor;
	for (auto& c: mCoefficients) {
		c *= f;
		f *= factor;
	}
}

template<typename Coeff>
void UnivariatePolynomial<Coeff>::shift(const Coeff& a) {
	std::vector<Coeff> next;
	next.reserve(this->mCoefficients.size());
	next.push_back(this->mCoefficients.back());

	for (std::size_t i = 0; i < this->mCoefficients.size()-1; i++) {
		next.push_back(next.back());
		for (std::size_t j = i; j > 0; j--) {
			next[j] = a * next[j] + next[j-1];
		}
		next[0] = a * next[0] + this->mCoefficients[this->mCoefficients.size()-2-i];
	}
	this->mCoefficients = next;
}

template<typename Coeff>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::operator -() const
{
	UnivariatePolynomial result(mMainVar);
	result.mCoefficients.reserve(mCoefficients.size());
	for(auto c : mCoefficients)
	{
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
	
	if(rhs.isZero())
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
	CARL_LOG_INEFFICIENT();
	return *this += -rhs;
}

template<typename Coeff>
UnivariatePolynomial<Coeff>& UnivariatePolynomial<Coeff>::operator-=(const UnivariatePolynomial& rhs)
{
	CARL_LOG_INEFFICIENT();
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
template<typename C, EnableIf<is_number<C>>>
UnivariatePolynomial<Coefficient>& UnivariatePolynomial<Coefficient>::operator*=(Variable rhs) {
	if (rhs == this->mMainVar) {
		this->mCoefficients.insert(this->mCoefficients.begin(), Coefficient(0));
		return *this;
	}
	assert(!carl::is_number<Coefficient>::value);
	return *this;
}
template<typename Coefficient>
template<typename C, DisableIf<is_number<C>>>
UnivariatePolynomial<Coefficient>& UnivariatePolynomial<Coefficient>::operator*=(Variable rhs) {
	if (rhs == this->mMainVar) {
		this->mCoefficients.insert(this->mCoefficients.begin(), Coefficient(0));
		return *this;
	}
	assert(!carl::is_number<Coefficient>::value);
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
	
	if(is_finite<Coefficient>::value)
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
	if(rhs.isZero())
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
template<typename C, EnableIf<is_field<C>>>
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
template<typename C, DisableIf<is_field<C>>>
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
	if(lhs.isZero()) return lhs;
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
		if(lhs.isZero() && rhs.isZero()) return true;
		if(lhs.isConstant() && rhs.isConstant() && lhs.lcoeff() == rhs.lcoeff()) return true;
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
	if(lhs.isZero())
	{
		return rhs == C(0);
	}
	return lhs.isConstant() && lhs.lcoeff() == rhs;
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
			if (rhs.isZero()) return false;
			if (isZero() || this->degree() < rhs.degree()) return true;
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
	if(rhs.isZero()) return os << "0";
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
template<typename C, EnableIf<is_number<C>>>
bool UnivariatePolynomial<Coefficient>::isConsistent() const {
	if (this->mCoefficients.size() > 0) {
		assert(this->lcoeff() != Coefficient(0));
	}
	return true;
}

template<typename Coefficient>
template<typename C, DisableIf<is_number<C>>>
bool UnivariatePolynomial<Coefficient>::isConsistent() const {
	if (this->mCoefficients.size() > 0) {
		assert(!this->lcoeff().isZero());
	}
	for (auto c: this->mCoefficients) {
		assert(!c.has(this->mainVar()));
		assert(c.isConsistent());
	}
	return true;
}

}
