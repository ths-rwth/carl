#pragma once

#include "GCD.h"

#include "../../converter/CoCoAAdaptor.h"
#include "../../numbers/FunctionSelector.h"
#include "../logging.h"
#include "../MultivariatePolynomial.h"
#include "../UnivariatePolynomial.h"

namespace carl {

template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> squareFreePart(const MultivariatePolynomial<C,O,P>& p) {
	CARL_LOG_DEBUG("carl.core.sqfree", "SquareFreePart of " << p);
	if (p.isConstant() || p.isLinear()) return p;

	using TypeSelector = carl::function_selector::NaryTypeSelector;

	using types = carl::function_selector::wrap_types<
		mpz_class,mpq_class
#if defined USE_GINAC
		,cln::cl_I,cln::cl_RA
#endif
	>;

	auto s = carl::createFunctionSelector<TypeSelector, types>(
	#if defined USE_COCOA
		[](const auto& p){ CoCoAAdaptor<MultivariatePolynomial<C,O,P>> c({p}); return c.squareFreePart(p); },
		[](const auto& p){ CoCoAAdaptor<MultivariatePolynomial<C,O,P>> c({p}); return c.squareFreePart(p); }
	#else
		[](const auto& p){ return p; },
		[](const auto& p){ return p; }
	#endif
	#if defined USE_GINAC
		,
		[](const auto& p){ return p; },
		[](const auto& p){ return p; }
	#endif
	);
	return s(p);
}

template<typename Coeff, EnableIf<is_subset_of_rationals<Coeff>> = dummy>
UnivariatePolynomial<Coeff> squareFreePart(const UnivariatePolynomial<Coeff>& p) {
	CARL_LOG_DEBUG("carl.core.sqfree", "SquareFreePart of " << p);
	if (p.isZero()) return p;
	if (p.isLinearInMainVar()) return p;
	UnivariatePolynomial<Coeff> normalized = p.coprimeCoefficients().template convert<Coeff>();
	return normalized.divideBy(carl::gcd(normalized, normalized.derivative())).quotient;
}

template<typename Coeff, DisableIf<is_subset_of_rationals<Coeff>> = dummy>
UnivariatePolynomial<Coeff> squareFreePart(const UnivariatePolynomial<Coeff>& p) {
	CARL_LOG_DEBUG("carl.core.sqfree", "SquareFreePart of " << p);
	return carl::squareFreePart(MultivariatePolynomial<typename carl::UnderlyingNumberType<Coeff>::type>(p)).toUnivariatePolynomial(p.mainVar());
}

}
