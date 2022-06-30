#pragma once

#include <carl-common/config.h>

#include "ran.h"

#include "interval/ran_interval_real_roots.h"

//#include "thom/ran_thom_real_roots.h"

#ifdef USE_LIBPOLY
#include "libpoly/RealRoots.h"
#endif

namespace carl::ran {

namespace internal {
template<typename Poly>
struct real_roots_internal {
    static real_roots_result<typename Poly::RootType> real_roots(const Poly& polynomial, const Interval<typename Poly::NumberType>& interval) {
        CARL_LOG_FATAL("carl.ran", "real_roots not implemented for type: " << typeid(Poly).name() << " " << polynomial << " " << interval);
        assert(false);
        return real_roots_result<typename Poly::RootType>::no_roots_response();
    }

    static real_roots_result<typename Poly::RootType> real_roots(const Poly& polynomial, const std::map<Variable, typename Poly::RootType>& assignment, const Interval<typename Poly::Number>& interval) {
        CARL_LOG_FATAL("carl.ran", "real_roots not implemented for type: " << typeid(Poly).name() << " " << polynomial << " " << assignment << " " << interval);
        assert(false);
        return real_roots_result<typename Poly::RootType>::no_roots_response();
    }
};

#ifdef USE_LIBPOLY
template<>
struct real_roots_internal<LPPolynomial> {
    static real_roots_result<LPPolynomial::RootType> real_roots(const LPPolynomial& polynomial, const Interval<LPPolynomial::NumberType>& interval) {
        CARL_LOG_DEBUG("carl.ran", "Called real_roots for LPPolynomial: " << polynomial);
        return carl::ran::libpoly::real_roots_libpoly(polynomial, interval);
    }

    static real_roots_result<LPPolynomial::RootType> real_roots(const LPPolynomial& polynomial, const std::map<Variable, LPPolynomial::RootType>& assignment, const Interval<LPPolynomial::NumberType>& interval) {
        CARL_LOG_DEBUG("carl.ran", "Called real_roots for LPPolynomial: " << polynomial << " " << assignment);
        return carl::ran::libpoly::real_roots_libpoly(polynomial, assignment, interval);
    }
};
#endif

template<typename Coeff>
struct real_roots_internal<UnivariatePolynomial<Coeff>> {
    static real_roots_result<typename UnivariatePolynomial<Coeff>::RootType> real_roots(const UnivariatePolynomial<Coeff>& polynomial, const Interval<typename UnivariatePolynomial<Coeff>::NumberType>& interval) {
        CARL_LOG_DEBUG("carl.ran", "Called real_roots for UnivariatePolynomial: " << polynomial);
        return carl::ran::interval::real_roots_interval(polynomial, interval);
    }

    static real_roots_result<typename UnivariatePolynomial<Coeff>::RootType> real_roots(const UnivariatePolynomial<Coeff>& polynomial, const std::map<Variable, typename UnivariatePolynomial<Coeff>::RootType>& assignment, const Interval<typename UnivariatePolynomial<Coeff>::NumberType>& interval) {
        CARL_LOG_DEBUG("carl.ran", "Called real_roots for UnivariatePolynomial: " << polynomial << " " << assignment);
        return carl::ran::interval::real_roots_interval(polynomial, assignment, interval);
    }
};
} // namespace internal

template<typename Poly, typename... Coeff>
real_roots_result<typename Poly::RootType> real_roots(const Poly& polynomial, const std::map<Variable, typename Poly::RootType>& assignment, const Interval<typename Poly::NumberType>& interval = Interval<typename Poly::NumberType>::unbounded_interval()) {
    return internal::real_roots_internal<Poly>::real_roots(polynomial, assignment, interval);
}

template<typename Poly, typename... Coeff>
real_roots_result<typename Poly::RootType> real_roots(const Poly& polynomial, const Interval<typename Poly::NumberType>& interval = Interval<typename Poly::NumberType>::unbounded_interval()) {
    return internal::real_roots_internal<Poly>::real_roots(polynomial, interval);
}
} // namespace carl::ran

namespace carl {
using carl::ran::real_roots;
}
