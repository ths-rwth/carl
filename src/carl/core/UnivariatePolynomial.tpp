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
#include "logging.h"
#include "MultivariateGCD.h"
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

template<typename Coeff>
Coeff UnivariatePolynomial<Coeff>::content() const
{
	if(isZero())
	{
		// By definition.
		return Coeff(0);
	}
	assert(isNormal());
	// As we consider normal polynomials, for fields, we know that the content must be 1.
	if(is_field<Coeff>::value)
	{
		return Coeff(1);
	}
	typename std::vector<Coeff>::const_reverse_iterator it = mCoefficients.rbegin();
	Coeff gcd = *it;
	for(++it; it != mCoefficients.rend(); ++it)
	{
		if(gcd == Coeff(1)) break;
		if(*it == Coeff(0)) continue;
		gcd = carl::gcd(gcd, *it);
	}
	return gcd;
}

template<typename Coeff>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::primitivePart() const
{
	if(this->isZero()) {
		return *this;
	}
	
	if (this->isNormal())
	{
		return *this / this->content();
	}
	else
	{
		auto tmp = *this * Coeff(-1);
		return tmp / tmp.content();
	}
}

/**
 * See Algorithm 2.2 in @cite GCL92.
 * @param a
 * @param b
 * @param s
 * @param t
 * @return 
 */
template<typename Coeff>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::extended_gcd(const UnivariatePolynomial& a, const UnivariatePolynomial& b, UnivariatePolynomial& s, UnivariatePolynomial& t)
{
	assert(a.mMainVar == b.mMainVar);
	assert(a.mMainVar == s.mMainVar);
	assert(a.mMainVar == t.mMainVar);
	assert(!a.isZero());
	assert(!b.isZero());
	
	CARL_LOG_DEBUG("carl.core", "UnivEEA: a=" << a << ", b=" << b );
	Variable x = a.mMainVar;
	UnivariatePolynomial<Coeff> c(a);
	UnivariatePolynomial<Coeff> d(b);
	c.normalizeCoefficients();
	d.normalizeCoefficients();
	c = c.normalized();
	d = d.normalized();
	
	UnivariatePolynomial<Coeff> c1 = a.one();
	UnivariatePolynomial<Coeff> c2(x);
	
	UnivariatePolynomial<Coeff> d1(x);
	UnivariatePolynomial<Coeff> d2 = a.one();
	
	while(!d.isZero())
	{
		DivisionResult<UnivariatePolynomial<Coeff>> divres = c.divideBy(d);
		assert(divres.remainder == c - divres.quotient * d);
		UnivariatePolynomial r1 = c1 - divres.quotient*d1;
		UnivariatePolynomial r2 = c2 - divres.quotient*d2;
		CARL_LOG_TRACE("carl.core", "UnivEEA: q=" << divres.quotient << ", r=" << divres.remainder);
		CARL_LOG_TRACE("carl.core", "UnivEEA: r1=" << c1 << "-" << divres.quotient << "*" << d1 << "==" << c1 - divres.quotient * d1 );
		CARL_LOG_TRACE("carl.core", "UnivEEA: r2=" << c2 << "-" << divres.quotient << "*" << d2 << "==" << c2 - divres.quotient * d2 );
		c = d;
		c1 = d1;
		c2 = d2;
		d = divres.remainder;
		d1 = r1;
		d2 = r2;
		c.normalizeCoefficients();
		d.normalizeCoefficients();
		
		CARL_LOG_TRACE("carl.core", "UnivEEA: c=" << c << ", d=" << d );
		CARL_LOG_TRACE("carl.core", "UnivEEA: c1=" << c1 << ", c2=" << c2 );
		CARL_LOG_TRACE("carl.core", "UnivEEA: d1=" << d1 << ", d2=" << d2 );
	}
	assert(!c.isZero());
	s = c1 / (a.lcoeff() * c.lcoeff());
	t = c2 / (b.lcoeff() * c.lcoeff());
	c = c.normalized();
	c.normalizeCoefficients();
	s.normalizeCoefficients();
	t.normalizeCoefficients();
	CARL_LOG_DEBUG("carl.core", "UnivEEA: g=" << c << ", s=" << s << ", t=" << t );
	CARL_LOG_TRACE("carl.core", "UnivEEA: " << c << "==" << s*a + t*b << "==" << s*a << " + " << t*b );
	assert(c == s*a + t*b);
	return c;
}

template<typename Coeff>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::gcd(const UnivariatePolynomial& a, const UnivariatePolynomial& b)
{
	// We want degree(b) <= degree(a).
	assert(!a.isZero());
	assert(!b.isZero());
	assert(a.mainVar() == b.mainVar());
	if(a.degree() < b.degree()) return gcd_recursive(b.normalized(),a.normalized()).normalized();
	else return gcd_recursive(a.normalized(),b.normalized()).normalized();
}


template<typename Coeff>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::gcd_recursive(const UnivariatePolynomial& a, const UnivariatePolynomial& b)
{
	assert(!a.isZero());
	assert(b.isZero() || b.degree() <= a.degree());
	
	if(b.isZero()) return a;
//	if(is_field<Coeff>::value)
//	{
//		if(b.isConstant()) return b;
//	}
	else return gcd_recursive(b, a.remainder(b));
}

template<typename Coeff>
template<typename C, EnableIf<is_subset_of_rationals<C>>>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::squareFreePart() const {
	if (this->isZero()) return *this;
	if (this->isLinearInMainVar()) return *this;
	UnivariatePolynomial normalized = this->coprimeCoefficients().template convert<Coeff>();
	return normalized.divideBy(UnivariatePolynomial::gcd(normalized, normalized.derivative())).quotient;
}

template<typename Coeff>
template<typename C, DisableIf<is_subset_of_rationals<C>>>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::squareFreePart() const {
//
	CARL_LOG_NOTIMPLEMENTED();
	return *this;
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
FactorMap<Coeff> UnivariatePolynomial<Coeff>::factorization() const
{
	CARL_LOG_TRACE("carl.core", "UnivFactor: " << *this );
	FactorMap<Coeff> result;
	if(isConstant()) // Constant.
	{
		CARL_LOG_TRACE("carl.core", "UnivFactor: add the factor (" << *this << ")^" << 1 );
		result.emplace(*this, 1);
		return result;
	}
	// Make the polynomial's coefficients coprime (integral and with gcd 1).
	UnivariatePolynomial<Coeff> remainingPoly(mainVar());
	Coeff factor = coprimeFactor();
	if(factor == 1)
	{
		remainingPoly = *this;
	}
	else
	{
		// Store the rational factor and make the polynomial's coefficients coprime.
		CARL_LOG_TRACE("carl.core", "UnivFactor: add the factor (" << UnivariatePolynomial<Coeff>(mainVar(), constant_one<Coeff>::get() / factor) << ")^" << 1 );
		result.emplace(UnivariatePolynomial<Coeff>(mainVar(), constant_one<Coeff>::get() / factor), 1);
		remainingPoly.mCoefficients.reserve(mCoefficients.size());
		for(const Coeff& coeff : mCoefficients)
		{
			remainingPoly.mCoefficients.push_back(coeff * factor);
		}
	}
	assert(mCoefficients.size() > 1);
	// Exclude the factors  (x-r)^i  with  r rational.
	remainingPoly = excludeLinearFactors<sint>(remainingPoly, result, carl::fromInt<typename IntegralType<Coeff>::type>(INT_MAX));
	assert(!remainingPoly.isConstant() || remainingPoly.lcoeff() == (Coeff)1);
	if(!remainingPoly.isConstant())
	{
		// Calculate the square free factorization.
		auto sff = remainingPoly.squareFreeFactorization();
//		factor = (Coeff) 1;
		for(auto expFactorPair = sff.begin(); expFactorPair != sff.end(); ++expFactorPair)
		{
//			Coeff cpf = expFactorPair->second.coprimeFactor();
//			if(cpf != (Coeff) 1)
//			{
//				factor *= pow(expFactorPair->second.coprimeFactor(), expFactorPair->first);
//				expFactorPair->second /= cpf;
//			}
			if(!expFactorPair->second.isConstant() || !carl::isOne(expFactorPair->second.lcoeff()))
			{
				auto retVal = result.emplace(expFactorPair->second, expFactorPair->first);
				CARL_LOG_TRACE("carl.core", "UnivFactor: add the factor (" << expFactorPair->second << ")^" << expFactorPair->first );
				if(!retVal.second)
				{
					retVal.first->second += expFactorPair->first;
				}
			}
		}
//		if(factor != (Coeff) 1)
//		{
//			CARL_LOG_TRACE("carl.core", "UnivFactor: add the factor (" << UnivariatePolynomial<Coeff>(mainVar(), {factor}) << ")^" << 1 );
//			// Add the constant factor to the factors.
//			if( result.begin()->first.isConstant() )
//			{
//				factor *= result.begin()->first.lcoeff();
//				result.erase( result.begin() );
//			}
//			result.insert(result.begin(), std::pair<UnivariatePolynomial<Coeff>, unsigned>(UnivariatePolynomial<Coeff>(mainVar(), {factor}), 1));
//		}
	}
	return result;
}

template<typename Coeff>
template<typename Integer>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::excludeLinearFactors(const UnivariatePolynomial<Coeff>& poly, FactorMap<Coeff>& linearFactors, const Integer& maxInt)
{
	CARL_LOG_TRACE("carl.core", "UnivELF: " << poly );
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
		CARL_LOG_TRACE("carl.core", "UnivELF: add the factor (" << retVal.first->first << ")^" << k );
		if(!retVal.second)
		{
			retVal.first->second += k;
		}
		// Construct the remainder:  result := a_n * x^{n-k} + ... + a_{k-1} * x + a_k
		std::vector<Coeff> cfs;
		cfs.reserve(poly.coefficients().size()-k);
		cfs = std::vector<Coeff>(cf, poly.coefficients().end());
		result = UnivariatePolynomial<Coeff>(poly.mainVar(), std::move(cfs));
		CARL_LOG_TRACE("carl.core", "UnivELF: remainder is  " << result );
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
		if( maxInt != 0 && (tc > maxInt || lc > maxInt) )
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
			CARL_LOG_TRACE("carl.core", "UnivELF: try rational  " << (positive ? "-" : "") << *tcFactor << "/" << *lcFactor);
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
				Coeff posRatZero = positive ? (Coeff(*tcFactor) / Coeff(*lcFactor)) : Coeff(-(Coeff(*tcFactor) / Coeff(*lcFactor)));
				CARL_LOG_TRACE("carl.core", "UnivELF: consider possible non zero rational factor  " << posRatZero);
				Coeff image = result.syntheticDivision(posRatZero);
				if (carl::isZero(image)) {
					// Remove all linear factor with the found zero from result.
					UnivariatePolynomial<Coeff> linearFactor(result.mainVar(), {-posRatZero, Coeff(1)});
					while (carl::isZero(image)) {
						auto retVal = linearFactors.emplace(linearFactor, 1);
						CARL_LOG_TRACE("carl.core", "UnivELF: add the factor (" << linearFactor << ")^" << 1 );
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
					if( imageInt <= maxInt )
					{
						CARL_LOG_TRACE("carl.core", "UnivELF: new shift with " << getNum(posRatZero) << " to " << carl::abs(getNum(image)));
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
							CARL_LOG_TRACE("carl.core", "UnivFactor: add the factor (" << UnivariatePolynomial<Coeff>(result.mainVar(), std::initializer_list<Coeff>({(Coeff)1/factor})) << ")^" << 1 );
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
		CARL_LOG_TRACE("carl.core", "UnivFactor: add the factor (" << UnivariatePolynomial<Coeff>(result.mainVar(), factor) << ")^" << 1 );
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
	CARL_LOG_TRACE("carl.core", "UnivELF: add the factor (" << result << ")^" << 1 );
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
	CARL_LOG_TRACE("carl.core", "UnivSynDiv: (" << *this << ")[x -> " << zeroOfDivisor << "]  =  " << thirdRow.front());
	if(thirdRow.front() == 0)
	{
		thirdRow.erase(thirdRow.begin());
		this->mCoefficients.swap(thirdRow);
		CARL_LOG_TRACE("carl.core", "UnivSynDiv: reduced by ((" << carl::abs(getDenom(thirdRow.front())) << ")*" << mainVar() << " + (" << (thirdRow.front()<0 ? "-" : "") << carl::abs(getNum(thirdRow.front())) << "))  ->  " << *this);
		return Coeff(0);
	}
	return thirdRow.front();
}

template<typename Coeff>
std::map<uint, UnivariatePolynomial<Coeff>> UnivariatePolynomial<Coeff>::squareFreeFactorization() const
{
	CARL_LOG_TRACE("carl.core", "UnivSSF: " << *this);
	std::map<uint,UnivariatePolynomial<Coeff>> result;
CLANG_WARNING_DISABLE("-Wtautological-compare")
	assert(!isZero()); // TODO what if zero?
	// degree() >= characteristic<Coeff>::value throws a warning in clang...
	if(characteristic<Coeff>::value != 0 && degree() >= characteristic<Coeff>::value)
CLANG_WARNING_RESET
	{
		CARL_LOG_TRACE("carl.core", "UnivSSF: degree greater than characteristic!");
		result.emplace(1, *this);
		CARL_LOG_TRACE("carl.core", "UnivSSF: add the factor (" << *this << ")^1");
	}
	else
	{
		assert(!isConstant()); // Othewise, the derivative is zero and the next assertion is thrown.
		UnivariatePolynomial<Coeff> b = this->derivative();
		CARL_LOG_TRACE("carl.core", "UnivSSF: b = " << b);
		UnivariatePolynomial<Coeff> s(mainVar());
		UnivariatePolynomial<Coeff> t(mainVar());
		assert(!b.isZero());
		UnivariatePolynomial<Coeff> c = extended_gcd((*this), b, s, t); // TODO: use gcd instead
		typename IntegralType<Coeff>::type numOfCpf = getNum(c.coprimeFactor());
		if(numOfCpf != 1) // TODO: is this maybe only necessary because the extended_gcd returns a polynomial with non-integer coefficients but it shouldn't?
		{
			c *= Coeff(numOfCpf);
		}
		CARL_LOG_TRACE("carl.core", "UnivSSF: c = " << c);
		if(c.isZero())
		{
			result.emplace(1, *this);
			CARL_LOG_TRACE("carl.core", "UnivSSF: add the factor (" << *this << ")^1");
		}
		else
		{
			UnivariatePolynomial<Coeff> w = (*this).divideBy(c).quotient;
			CARL_LOG_TRACE("carl.core", "UnivSSF: w = " << w);
			UnivariatePolynomial<Coeff> y = b.divideBy(c).quotient;
			CARL_LOG_TRACE("carl.core", "UnivSSF: y = " << y);
			UnivariatePolynomial<Coeff> z = y-w.derivative();
			CARL_LOG_TRACE("carl.core", "UnivSSF: z = " << z);
			uint i = 1;
			while(!z.isZero())
			{
				CARL_LOG_TRACE("carl.core", "UnivSSF: next iteration");
				UnivariatePolynomial<Coeff> g = extended_gcd(w, z, s, t); // TODO: use gcd instead
				numOfCpf = getNum(g.coprimeFactor());
				if(numOfCpf != 1) // TODO: is this maybe only necessary because the extended_gcd returns a polynomial with non-integer coefficients but it shouldn't?
				{
					g *= Coeff(numOfCpf);
				}
				CARL_LOG_TRACE("carl.core", "UnivSSF: g = " << g);
				assert(result.find(i) == result.end());
				result.emplace(i, g);
				CARL_LOG_TRACE("carl.core", "UnivSSF: add the factor (" << g << ")^" << i);
				++i;
				w = w.divideBy(g).quotient;
				CARL_LOG_TRACE("carl.core", "UnivSSF: w = " << w);
				y = z.divideBy(g).quotient;
				CARL_LOG_TRACE("carl.core", "UnivSSF: y = " << y);
				z = y - w.derivative();
				CARL_LOG_TRACE("carl.core", "UnivSSF: z = " << z);
			}
			result.emplace(i, w);
			CARL_LOG_TRACE("carl.core", "UnivSSF: add the factor (" << w << ")^" << i);
		}
	}
	return result;
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
const std::list<UnivariatePolynomial<Coeff>> UnivariatePolynomial<Coeff>::subresultants(
		const UnivariatePolynomial<Coeff>& pol1,
		const UnivariatePolynomial<Coeff>& pol2,
		const SubresultantStrategy strategy
) {
	/* The algorithm consists of three parts:
	 * Part 1: Initialization, i.e. preparation of the input so that the requirements of the core algorithm in parts 2 and 3 are met.
	 * Part 2: First part of the main loop. If the two subresultants which were added before (initially the two inputs) differ by more
	 *		 than 1 in their degree, an intermediate subresultant is computed by reducing the last one added with the leading coefficient
	 *		 of the one before this one.
	 * Part 3: Second part of the main loop. The pseudo remainder of the last two subresultants (the one possibly added in Part 2 disregarded)
	 *		 is computed and added to the subresultant sequence.
	 */

	/* Part 1
	 * Check and normalize input, initialize local variables.
	 */
	assert(pol1.mainVar() == pol2.mainVar());
	CARL_LOG_TRACE("carl.core.resultant", "subresultants(" << pol1 << ", " << pol2 << ")");
	std::list<UnivariatePolynomial<Coeff>> subresultants;
	Variable variable = pol1.mainVar();
	
	assert(!pol1.isZero());
	assert(!pol2.isZero());
	// We initialize p and q with pol1 and pol2.
	UnivariatePolynomial<Coeff> p(pol1), q(pol2);
	
	// pDeg >= qDeg shall hold, so switch if it does not hold
	if (p.degree() < q.degree()) {
		std::swap(p, q);
	}
	CARL_LOG_TRACE("carl.core.resultant", "p = " << p);
	CARL_LOG_TRACE("carl.core.resultant", "q = " << q);
	
	subresultants.push_front(p);
	if (q.isZero()) {
		CARL_LOG_TRACE("carl.core.resultant", "q is Zero.");
		return subresultants;
	}
	subresultants.push_front(q);
	
	// SPECIAL CASE: both, p and q, are constant
	if (q.isConstant()) {
		CARL_LOG_TRACE("carl.core.resultant", "q is constant.");
		return subresultants;
	}
	
	// Explicitly check preconditions
	assert(p.degree() >= q.degree());
	assert(q.degree() >= 1);
	
	// BUG in Duco's article(?):
	//ex subresLcoeff = GiNaC::pow( a.lcoeff(), a.degree() - b.degree() );	// initialized on the basis of the smaller-degree polynomial
	//Coeff subresLcoeff(a.lcoeff()); // initialized on the basis of the smaller-degree polynomial
	Coeff subresLcoeff = q.lcoeff().pow(p.degree() - q.degree());
	CARL_LOG_TRACE("carl.core.resultant", "subresLcoeff = " << subresLcoeff);
	
	UnivariatePolynomial<Coeff> tmp = q;
	q = p.prem(-q);
	p = tmp;
	CARL_LOG_TRACE("carl.core.resultant", "q = p.prem(-q) = " << q);
	CARL_LOG_TRACE("carl.core.resultant", "p = " << p);
	//CARL_LOG_TRACE("carl.core.resultant", "p = q");
	//CARL_LOG_TRACE("carl.core.resultant", "q = p.prem(-q)");
	
	/* Parts 2 and 3
	 * Main loop filling the subresultants chain step by step.
	 */
	// MAIN: start main loop containing different computation strategies
	while (true) {
		CARL_LOG_TRACE("carl.core.resultant", "Looping...");
		CARL_LOG_TRACE("carl.core.resultant", "p = " << p);
		CARL_LOG_TRACE("carl.core.resultant", "q = " << q);
		if (q.isZero()) return subresultants;
		uint pDeg = p.degree();
		uint qDeg = q.degree();
		subresultants.push_front(q);
		
		// Part 2
		assert(pDeg >= qDeg);
		uint delta = pDeg - qDeg;
		CARL_LOG_TRACE("carl.core.resultant", "delta = " << delta);
		
		/** Case distinction on delta: either we choose b as next subresultant or we could reduce b (delta > 1)
		 * and add the reduced version c as next subresultant. The reduction is done by division, which
		 * depends on the internal variable order of GiNaC and might fail although for some order it would succeed.
		 * In this case, we just do not reduce b. (A relaxed reduction could also be applied.)
		 *
		 * After the if-else block, bDeg is the degree of the front-most element of subresultants, be it c or b.
		 */
		UnivariatePolynomial<Coeff> c(variable);
		if (delta > 1) {
			// compute c
			// Notation hints: Compared to [Duc98], here S_{d-1} is b and S_d is a, and S_e is c.
			switch (strategy) {
				case SubresultantStrategy::Generic: {
					CARL_LOG_TRACE("carl.core.resultant", "Part 2: Generic strategy");
					UnivariatePolynomial<Coeff> reductionCoeff = q.lcoeff().pow(delta - 1) * q;
					Coeff dividant = subresLcoeff.pow(delta-1);
					bool res = reductionCoeff.divideBy(dividant, c);
					if (res) {
						subresultants.push_front(c);
						assert(!c.isZero());
						qDeg = c.degree();
					} else {
						c = q;
					}
					break;
				}
				case SubresultantStrategy::Ducos:
				case SubresultantStrategy::Lazard: {
					CARL_LOG_TRACE("carl.core.resultant", "Part 2: Ducos/Lazard strategy");
					// "dichotomous Lazard": efficient exponentiation
					uint deltaReduced = delta-1;
					CARL_LOG_TRACE("carl.core.resultant", "deltaReduced = " << deltaReduced);
					// should be true by the loop condition
					assert(deltaReduced > 0);
					
					Coeff lcoeffQ = q.lcoeff();
					UnivariatePolynomial<Coeff> reductionCoeff(variable, lcoeffQ);
					
					CARL_LOG_TRACE("carl.core.resultant", "lcoeffQ = " << lcoeffQ);
					CARL_LOG_TRACE("carl.core.resultant", "reductionCoeff = " << reductionCoeff);
					
					uint exponent = highestPower(deltaReduced);
					deltaReduced -= exponent;
					CARL_LOG_TRACE("carl.core.resultant", "exponent = " << exponent);
					CARL_LOG_TRACE("carl.core.resultant", "deltaReduced = " << deltaReduced);
					
					while (exponent != 1) {
						exponent /= 2;
						CARL_LOG_TRACE("carl.core.resultant", "exponent = " << exponent);
						bool res = (reductionCoeff*reductionCoeff).divideBy(subresLcoeff, reductionCoeff);
						if (res && deltaReduced >= exponent) {
							(reductionCoeff*lcoeffQ).divideBy(subresLcoeff, reductionCoeff);
							deltaReduced -= exponent;
						}
					}
					CARL_LOG_TRACE("carl.core.resultant", "reductionCoeff = " << reductionCoeff);
					reductionCoeff *= q;
					CARL_LOG_TRACE("carl.core.resultant", "reductionCoeff = " << reductionCoeff);
					bool res = reductionCoeff.divideBy(subresLcoeff, c);
					if (res) {
						subresultants.push_front(c);
						assert(!c.isZero());
						qDeg = c.degree();
						CARL_LOG_TRACE("carl.core.resultant", "qDeg = " << qDeg);
					} else {
						c = q;
					}
					CARL_LOG_TRACE("carl.core.resultant", "c = " << c);
					break;
				}
			}
		} else {
			c = q;
		}
		if (qDeg == 0) return subresultants;
		
		CARL_LOG_TRACE("carl.core.resultant", "Mid");
		//CARL_LOG_TRACE("carl.core.resultant", "p = " << p);
		//CARL_LOG_TRACE("carl.core.resultant", "q = " << q);
		
		// Part 3
		switch (strategy) {
			// Compared to [Duc98], here S_{d-1} is b and S_d is a, S_e is c, and s_d is subresLcoeff.
			case SubresultantStrategy::Generic:
			case SubresultantStrategy::Lazard: {
				CARL_LOG_TRACE("carl.core.resultant", "Part 3: Generic/Lazard strategy");
				if (p.isZero()) return subresultants;
				
				/* If b was constant, the degree properties for subresultants are still met, enforcing us to disregard whether
				 * the above division was successful (in this case, reducedNewB remains unchanged).
				 * If it was successful, the resulting term is safely added to the list, yielding an optimized resultant.
				 */
				UnivariatePolynomial<Coeff> reducedNewB = p.prem(-q);
				bool r = reducedNewB.divideBy(subresLcoeff.pow(delta)*p.lcoeff(), q);
				assert(r);
				break;
			}
			case SubresultantStrategy::Ducos: {
				CARL_LOG_TRACE("carl.core.resultant", "Part 3: Ducos strategy");
				// Ducos' optimization
				Coeff lcoeffQ = q.lcoeff();
				Coeff lcoeffC = c.lcoeff();
				std::vector<Coeff> h(pDeg);
				
				for (uint d = 0; d < qDeg; d++) {
					h[d] = lcoeffC * Coeff(variable).pow(d);
				}
				if (pDeg != qDeg) { // => aDeg > bDeg
					h[qDeg] = Coeff(lcoeffC * Coeff(variable).pow(qDeg) - c); // H_e
				}
				for (uint d = qDeg + 1; d < pDeg; d++) {
					Coeff t = h[d-1] * variable;
					UnivariatePolynomial<Coeff> reducedNewB = t.toUnivariatePolynomial(variable).coefficients()[qDeg] * q;
					bool res = reducedNewB.divideBy(lcoeffQ, reducedNewB);
					assert(res || reducedNewB.isConstant());
					h[d] = Coeff(t - reducedNewB);
				}
				
				UnivariatePolynomial<Coeff> sum(p.mainVar(), h.front() * p.coefficients()[0]);
				for (uint d = 1; d < pDeg; d++) {
					sum += h[d] * p.coefficients()[d];
				}
				UnivariatePolynomial<Coeff> normalizedSum(p.mainVar());
				bool res = sum.divideBy(p.lcoeff(), normalizedSum);
				assert(res || sum.isConstant());
				
				UnivariatePolynomial<Coeff> t(variable, {0, h.back()});
				UnivariatePolynomial<Coeff> reducedNewB = ((t + normalizedSum) * lcoeffQ - t.coefficients()[qDeg].toUnivariatePolynomial(variable));
				reducedNewB.divideBy(p.lcoeff(), reducedNewB);
				if (delta % 2 == 0) {
					q = -reducedNewB;
				} else {
					q = reducedNewB;
				}
				break;
			}
		}
		p = c;
		subresLcoeff = p.lcoeff();
	}
}

template<typename Coeff>
const std::vector<UnivariatePolynomial<Coeff>> UnivariatePolynomial<Coeff>::principalSubresultantsCoefficients(
		const UnivariatePolynomial<Coeff>& p,
		const UnivariatePolynomial<Coeff>& q,
		const SubresultantStrategy strategy
) {
	// Attention: Mathematica / Wolframalpha has one entry less (the last one) which is identical to p!
	std::list<UnivariatePolynomial<Coeff>> subres = UnivariatePolynomial<Coeff>::subresultants(p, q, strategy);
	std::vector<UnivariatePolynomial<Coeff>> subresCoeffs;
	uint i = 0;
	for (const auto& s: subres) {
		assert(!s.isZero());
		if (s.degree() < i) {
			// this and all further subresultants won't have a non-zero i-th coefficient
			break;
		}
		assert(s.degree() == i);
		subresCoeffs.emplace_back(s.mainVar(), s.lcoeff());
		i++;
	}
	return subresCoeffs;
}

template<typename Coeff>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::resultant(
		const UnivariatePolynomial& p,
		const SubresultantStrategy strategy
) const {
	assert(p.mainVar() == this->mainVar());
	if (this->isZero()) return UnivariatePolynomial(this->mainVar());
	if (p.isZero()) return UnivariatePolynomial(this->mainVar());
	UnivariatePolynomial<Coeff> resultant = UnivariatePolynomial<Coeff>::subresultants(this->normalized(), p.normalized(), strategy).front();
	//UnivariatePolynomial<Coeff> resultant = UnivariatePolynomial<Coeff>::subresultants(*this, p, strategy).front();
	CARL_LOG_TRACE("carl.core.resultant", "resultant(" << *this << ", " << p << ") = " << resultant);
	if (resultant.isConstant()) {
		return resultant;
	} else {
		return UnivariatePolynomial(this->mainVar());
	}
}

template<typename Coeff>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::discriminant(const SubresultantStrategy strategy) const {
	UnivariatePolynomial<Coeff> resultant = this->resultant(this->derivative(), strategy);
	if(isLinearInMainVar()) return resultant;
	uint d = this->degree();
	Coeff sign = ((d*(d-1) / 2) % 2 == 0) ? Coeff(1) : Coeff(-1);
	Coeff redCoeff = sign * this->lcoeff();
	bool res = resultant.divideBy(redCoeff, resultant);
	assert(res);
	CARL_LOG_TRACE("carl.cad", "discriminant(" << *this << ") = " << resultant);
	return resultant;
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
