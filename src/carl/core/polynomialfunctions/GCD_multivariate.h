#pragma once

#include "PrimitiveEuclidean.h"
#include "../MultivariatePolynomial.h"
#include "../../numbers/typetraits.h"

namespace carl {

namespace gcd_detail {
	template<typename Polynomial>
	Variable select_variable(const Polynomial& p1, const Polynomial& p2) {
		std::set<Variable> v1 = p1.gatherVariables();
		std::set<Variable> v2 = p2.gatherVariables();
		std::set<Variable> common;
		
		std::set_intersection(v1.begin(),v1.end(),v2.begin(),v2.end(), std::inserter(common,common.begin()));
		if (common.empty()) {
			return Variable::NO_VARIABLE;
		} else {
			return *common.begin();
		}
	}

	template<typename Polynomial>
	Polynomial gcd_calculate(const Polynomial& a, const Polynomial& b) {
		Variable x = select_variable(a, b);
		if (x == Variable::NO_VARIABLE) {
			return Polynomial(1);
		}
		CARL_LOG_INEFFICIENT();
		auto A = a.toUnivariatePolynomial(x);
		auto B = b.toUnivariatePolynomial(x);
		Polynomial result(primitive_euclidean(A.normalized(), B.normalized()));
		if (carl::isNegative(result.lcoeff()) && !(carl::isNegative(a.lcoeff()) && carl::isNegative(b.lcoeff()))) {
			return -result;
		}
		return result;
	}
}

template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> gcd(const MultivariatePolynomial<C,O,P>& a, const MultivariatePolynomial<C,O,P>& b) {
	CARL_LOG_DEBUG("carl.core.gcd", "gcd(" << a << ", " << b << ")");
	assert(!a.isZero());
	assert(!b.isZero());

	if (a.isOne() || b.isOne()) {
		return MultivariatePolynomial<C,O,P>(1);
	}
	if (a.isConstant() && b.isConstant()) {
		CARL_LOG_DEBUG("carl.core.gcd", "gcd(" << a << ", " << b << ") = " << carl::gcd(a.constantPart(), b.constantPart()));
		return MultivariatePolynomial<C,O,P>(carl::gcd(a.constantPart(), b.constantPart()));
	}
	if (is_field<C>::value && a.isConstant()) {
		CARL_LOG_DEBUG("carl.core.gcd", "gcd(" << a << ", " << b << ") = " << carl::gcd(a.constantPart(), b.coprimeFactor()));
		return MultivariatePolynomial<C,O,P>(carl::gcd(a.constantPart(), b.coprimeFactor()));
	}
	if (is_field<C>::value && b.isConstant()) {
		CARL_LOG_DEBUG("carl.core.gcd", "gcd(" << a << ", " << b << ") = " << carl::gcd(b.constantPart(), a.coprimeFactor()));
		return MultivariatePolynomial<C,O,P>(carl::gcd(b.constantPart(), a.coprimeFactor()));
	}

	auto s = overloaded {
	#if defined USE_COCOA
		[](const MultivariatePolynomial<mpq_class,O,P>& n1, const MultivariatePolynomial<mpq_class,O,P>& n2){ CoCoAAdaptor<MultivariatePolynomial<mpq_class,O,P>> c({n1, n2}); return c.gcd(n1,n2); },
		[](const MultivariatePolynomial<mpz_class,O,P>& n1, const MultivariatePolynomial<mpz_class,O,P>& n2){ CoCoAAdaptor<MultivariatePolynomial<mpz_class,O,P>> c({n1, n2}); return c.gcd(n1,n2); }
	#else
		[](const Polynomial& n1, const Polynomial& n2){ return gcd_detail::gcd_calculate(n1,n2); }
	#endif
	#if defined USE_GINAC
		,
		[](const MultivariatePolynomial<cln::cl_RA,O,P>& n1, const MultivariatePolynomial<cln::cl_RA,O,P>& n2){ return ginacGcd<MultivariatePolynomial<cln::cl_RA,O,P>>( n1, n2 ); },
		[](const MultivariatePolynomial<cln::cl_I,O,P>& n1, const MultivariatePolynomial<cln::cl_I,O,P>& n2){ return ginacGcd<MultivariatePolynomial<cln::cl_I,O,P>>( n1, n2 ); }
	#endif
	};
	CARL_LOG_DEBUG("carl.core.gcd", "gcd(" << a.coprimeCoefficients() << ", " << b.coprimeCoefficients() << ")");
	auto res = s(a, b);
	CARL_LOG_DEBUG("carl.core.gcd", "gcd(" << a.coprimeCoefficients() << ", " << b.coprimeCoefficients() << ") = " << res);
	return res;
}
}