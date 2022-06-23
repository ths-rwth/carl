#pragma once

#include <carl-common/config.h>

#include "ran.h"

#include "interval/ran_interval_real_roots.h"

//#include "thom/ran_thom_real_roots.h"

#ifdef USE_LIBPOLY
#include "libpoly/ran_libpoly_real_roots.h"
#endif

namespace carl::ran {
// template<template<typename CoeffType> typename Poly, typename CoeffType, typename Number>
// struct real_roots_internal_interval {
//	static real_roots_result<typename Poly<CoeffType>::RootType<Number>> real_roots(const Poly<CoeffType>& polynomial, const Interval<Number>& interval) {
//		CARL_LOG_ERROR("carl.ran", "real_roots not implemented for type: " << typeid(Poly<CoeffType>).name() << " " << polynomial << " " << interval);
//		assert(false);
//		return real_roots_result<typename Poly<CoeffType>::RootType<Number>>::non_univariate_response();
//	}
//	static real_roots_result<typename Poly<CoeffType>::RootType<Number>> real_roots(const Poly<CoeffType>& polynomial, const std::map<Variable, typename Poly<CoeffType>::RootType<Number>>& assignment, const Interval<Number>& interval) {
//		CARL_LOG_ERROR("carl.ran", "real_roots not implemented for type: " << typeid(Poly<CoeffType>).name() << " " << polynomial << " " << interval);
//		assert(false);
//		return real_roots_result<typename Poly<CoeffType>::RootType<Number>>::non_univariate_response();
//	}
// };

template<typename Poly, typename Number>
struct real_roots_internal {
	static real_roots_result<typename Poly::RootType<Number>> real_roots(const Poly& polynomial, const Interval<Number>& interval) {
		CARL_LOG_ERROR("carl.ran", "real_roots not implemented for type: " << typeid(Poly).name() << " " << polynomial << " " << interval);
		return real_roots_result<typename Poly::RootType<Number>>::no_roots_response();
	}

	static real_roots_result<typename Poly::RootType<Number>> real_roots(const Poly& polynomial, const std::map<Variable, typename Poly::RootType<Number>>& assignment, const Interval<Number>& interval) {
		CARL_LOG_ERROR("carl.ran", "real_roots not implemented for type: " << typeid(Poly).name() << " " << polynomial << " " << assignment << " " << interval);
		return real_roots_result<typename Poly::RootType<Number>>::no_roots_response();
	}
};

template<typename Number>
struct real_roots_internal<LPPolynomial, Number> {
	static real_roots_result<LPPolynomial::RootType<Number>> real_roots(const LPPolynomial& polynomial, const Interval<Number>& interval) {
		CARL_LOG_DEBUG("carl.ran", "Called real_roots for LPPolynomial: " << polynomial);
		return real_roots_result<LPPolynomial::RootType<Number>>::no_roots_response();
	}

	static real_roots_result<LPPolynomial::RootType<Number>> real_roots(const LPPolynomial& polynomial, const std::map<Variable, LPPolynomial::RootType<Number>>& assignment, const Interval<Number>& interval) {
		CARL_LOG_DEBUG("carl.ran", "Called real_roots for LPPolynomial: " << polynomial);
		return real_roots_result<LPPolynomial::RootType<Number>>::no_roots_response();
	}
};

template<typename Coeff, typename Number>
struct real_roots_internal<UnivariatePolynomial<Coeff>, Number> {
	static real_roots_result<typename UnivariatePolynomial<Coeff>::RootType<Number>> real_roots(const UnivariatePolynomial<Coeff>& polynomial, const Interval<Number>& interval) {
		CARL_LOG_DEBUG("carl.ran", "Called real_roots for UnivariatePolynomial: " << polynomial);
		return real_roots_result<typename UnivariatePolynomial<Coeff>::RootType<Number>>::no_roots_response();
	}

	static real_roots_result<typename UnivariatePolynomial<Coeff>::RootType<Number>> real_roots(const UnivariatePolynomial<Coeff>& polynomial, const std::map<Variable, typename UnivariatePolynomial<Coeff>::RootType<Number>>& assignment, const Interval<Number>& interval) {
		CARL_LOG_DEBUG("carl.ran", "Called real_roots for UnivariatePolynomial: " << polynomial);
		return real_roots_result<typename UnivariatePolynomial<Coeff>::RootType<Number>>::no_roots_response();
	}
};

// template<template<typename CoeffType> typename Poly, typename CoeffType, typename Number = typename UnderlyingNumberType<CoeffType>::type>
// real_roots_result<typename Poly<CoeffType>::RootType<Number>> real_roots(const Poly<CoeffType>& polynomial, const Interval<Number>& interval = Interval<Number>::unbounded_interval()) {
//	return real_roots_internal_interval<Poly, CoeffType, Number>::real_roots(polynomial, interval);
// }
//
// template<template<typename CoeffType> typename Poly, typename CoeffType, typename Number = typename UnderlyingNumberType<CoeffType>::type>
// real_roots_result<typename Poly<CoeffType>::RootType<Number>> real_roots(const Poly<CoeffType>& polynomial, const std::map<Variable, typename Poly<CoeffType>::RootType<Number>>& assignment, const Interval<Number>& interval = Interval<Number>::unbounded_interval()) {
//	return real_roots_internal_interval<Poly, CoeffType, Number>::real_roots(polynomial, interval);
// }

template<typename Poly, typename Number = typename UnderlyingNumberType<typename Poly::CoeffType>::type, typename... Coeff>
real_roots_result<typename Poly::RootType<Number>> real_roots(const Poly& polynomial, const std::map<Variable, typename Poly::RootType<Number>>& assignment, const Interval<Number>& interval = Interval<Number>::unbounded_interval()) {
	return real_roots_internal<Poly, Number>::real_roots(polynomial, interval);
}

template<typename Poly, typename Number = typename UnderlyingNumberType<typename Poly::CoeffType>::type, typename... Coeff>
real_roots_result<typename Poly::RootType<Number>> real_roots(const Poly& polynomial, const Interval<Number>& interval = Interval<Number>::unbounded_interval()) {
	return real_roots_internal<Poly, Number>::real_roots(polynomial, interval);
}
} // namespace carl::ran

namespace carl {
using carl::ran::real_roots;
}
