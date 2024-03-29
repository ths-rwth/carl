#pragma once

#include "Division.h"
#include "Remainder.h"

#include "../UnivariatePolynomial.h"
#include <carl-arith/core/Variable.h>

namespace carl {

template<typename Coeff>
UnivariatePolynomial<Coeff> gcd_recursive(const UnivariatePolynomial<Coeff>& a, const UnivariatePolynomial<Coeff>& b) {
	assert(!carl::is_zero(a));
	assert(carl::is_zero(b) || b.degree() <= a.degree());
	
	if(carl::is_zero(b)) {
		return a;
	}
//	if(is_field_type<Coeff>::value)
//	{
//		if(b.is_constant()) return b;
//	}
	else {
		return gcd_recursive(b, remainder(a, b));
	}
}

/**
 * Calculates the extended greatest common divisor `g` of two polynomials.
 * The output polynomials `s` and `t` are computed such that \f$g = s \cdot a + t \cdot b\f$.
 * @param a First polynomial.
 * @param b Second polynomial.
 * @param s First output polynomial.
 * @param t Second output polynomial.
 * @see @cite GCL92, Algorithm 2.2
 * @return `gcd(a,b)`
 */
template<typename Coeff>
UnivariatePolynomial<Coeff> extended_gcd(const UnivariatePolynomial<Coeff>& a, const UnivariatePolynomial<Coeff>& b,
											UnivariatePolynomial<Coeff>& s, UnivariatePolynomial<Coeff>& t) {
	assert(a.main_var() == b.main_var());
	assert(a.main_var() == s.main_var());
	assert(a.main_var() == t.main_var());
	assert(!is_zero(a));
	assert(!is_zero(b));
	
	CARL_LOG_DEBUG("carl.core", "UnivEEA: a=" << a << ", b=" << b );
	Variable x = a.main_var();
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
	
	while(!is_zero(d))
	{
		DivisionResult<UnivariatePolynomial<Coeff>> divres = carl::divide(c, d);
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
	assert(!is_zero(c));
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

/**
 * Calculates the greatest common divisor of two polynomials.
 * @param a First polynomial.
 * @param b Second polynomial.
 * @return `gcd(a,b)`
 */
template<typename Coeff>
UnivariatePolynomial<Coeff> gcd(const UnivariatePolynomial<Coeff>& a, const UnivariatePolynomial<Coeff>& b) {
	// We want degree(b) <= degree(a).
	assert(!carl::is_zero(a));
	assert(!carl::is_zero(b));
	assert(a.main_var() == b.main_var());
	if(a.degree() < b.degree()) {
		return gcd_recursive(b.normalized(),a.normalized()).normalized();
	} else {
		return gcd_recursive(a.normalized(),b.normalized()).normalized();
	}
}

}
