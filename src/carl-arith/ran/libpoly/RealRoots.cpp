#include "RealRoots.h"

#ifdef USE_LIBPOLY


namespace carl {

RealRootsResult<LPRealAlgebraicNumber> real_roots(
    const LPPolynomial& polynomial,
    const Interval<LPRealAlgebraicNumber::NumberType>& interval) {
    CARL_LOG_DEBUG("carl.ran.libpoly", " Real roots of " << polynomial << " within " << interval);

    assert(poly::is_univariate(polynomial.get_polynomial()));

    // Easy checks
    if (carl::is_zero(polynomial)) {
        CARL_LOG_TRACE("carl.ran.libpoly", "poly is 0 -> nullified");
        return RealRootsResult<LPRealAlgebraicNumber>::nullified_response();
    } else if (carl::is_constant(polynomial)) {
        CARL_LOG_TRACE("carl.ran.libpoly", "poly is constant but not zero -> no root");
        return RealRootsResult<LPRealAlgebraicNumber>::no_roots_response();
    }

    poly::Interval inter_poly = to_libpoly_interval(interval);

    // actual calculations
    std::vector<poly::AlgebraicNumber> roots = poly::isolate_real_roots(poly::to_univariate(polynomial.get_polynomial()));

    if (roots.empty()) {
        CARL_LOG_DEBUG("carl.ran.libpoly", "Poly has no roots");
        return RealRootsResult<LPRealAlgebraicNumber>::no_roots_response();
    }

    // sort roots in ascending order
    std::sort(roots.begin(), roots.end(), std::less<poly::AlgebraicNumber>());

    // turn into RealRootsResult
    std::vector<LPRealAlgebraicNumber> res;
    for (const auto& val : roots) {
        auto tmp = LPRealAlgebraicNumber(val);
        // filter out roots not in interval
        if (poly::contains(inter_poly, poly::Value(val))) {
            CARL_LOG_DEBUG("carl.ran.libpoly", " Found Root " << val);
            res.emplace_back(tmp);
        }
    }

    return RealRootsResult<LPRealAlgebraicNumber>::roots_response(std::move(res));
}

RealRootsResult<LPRealAlgebraicNumber> real_roots(
    const LPPolynomial& polynomial,
    const std::map<Variable, LPRealAlgebraicNumber>& m,
    const Interval<LPRealAlgebraicNumber::NumberType>& interval) {
    CARL_LOG_DEBUG("carl.ran.libpoly", polynomial << " " << m << " " << interval);

    if (poly::is_univariate(polynomial.get_polynomial())) {
        return real_roots(polynomial, interval);
    }

    // easy checks
    if (carl::is_zero(polynomial)) {
        CARL_LOG_TRACE("carl.ran.libpoly", "poly is 0 -> nullified");
        return RealRootsResult<LPRealAlgebraicNumber>::nullified_response();
    } else if (carl::is_constant(polynomial)) {
        CARL_LOG_TRACE("carl.ran.libpoly", "poly is constant but not zero -> no root");
        return RealRootsResult<LPRealAlgebraicNumber>::no_roots_response();
    }

    for (const auto& v : carl::variables(polynomial)) {
		if (v != polynomial.main_var() && m.find(v) == m.end()) return RealRootsResult<LPRealAlgebraicNumber>::non_univariate_response();
	}

    poly::Interval inter_poly = to_libpoly_interval(interval);

    // Multivariate Polynomial
    // build the assignment
    lp_assignment_t& assignment = LPVariables::getInstance().get_assignment();
	
    Variable mainVar = polynomial.main_var();
    for (Variable& var : carl::variables(polynomial)) {
        if (var == mainVar) continue;
        // We convert numbers to libpoly values and add to assignment so we can substitute them later using libpoly
        lp_value_t val;
        // Turn into value
        lp_value_construct(&val, lp_value_type_t::LP_VALUE_ALGEBRAIC, m.at(var).get_internal());
        // That copies the value into the assignment
        lp_assignment_set_value(&assignment, LPVariables::getInstance().lp_variable(var), &val);
        // Destroy the value, but dont free the algebraic number!
        lp_value_destruct(&val);
    }

    CARL_LOG_TRACE("carl.ran.libpoly", "Call libpoly");
    lp_value_t* roots = new lp_value_t[poly::degree(polynomial.get_polynomial())];
    std::size_t roots_size;
    lp_polynomial_roots_isolate(polynomial.get_internal(), &assignment, roots, &roots_size);

    CARL_LOG_TRACE("carl.ran.libpoly", "Libpoly returned");
    if (roots_size == 0) {
        delete[] roots;
        CARL_LOG_DEBUG("carl.ran.libpoly", " Checking for nullification -> Evaluation at " << mainVar << "= 1");
        lp_value_t val;
        lp_value_construct_int(&val, long(1));
        lp_assignment_set_value(&assignment, LPVariables::getInstance().lp_variable(mainVar), &val);
        lp_value_destruct(&val);
        auto eval_val = lp_polynomial_evaluate(polynomial.get_internal(), &assignment);
        //CARL_LOG_DEBUG("carl.ran.libpoly", " Got eval_val " << eval_val);


        if (lp_value_cmp(eval_val, poly::Value(long(0)).get_internal()) == 0) {
            CARL_LOG_DEBUG("carl.ran.libpoly", "poly is 0 after substituting rational assignments -> nullified");
            lp_value_delete(eval_val);
            return RealRootsResult<LPRealAlgebraicNumber>::nullified_response();
        } else {
            CARL_LOG_DEBUG("carl.ran.libpoly", "Poly has no roots");
            lp_value_delete(eval_val);
            return RealRootsResult<LPRealAlgebraicNumber>::no_roots_response();
        }
    }

    std::vector<LPRealAlgebraicNumber> res;
    for (std::size_t i = 0; i < roots_size; ++i) {
        auto tmp = LPRealAlgebraicNumber::create_from_value(&roots[i]);
        // filter out roots not in interval
        if (lp_interval_contains(inter_poly.get_internal(), &roots[i])) {
            CARL_LOG_DEBUG("carl.ran.libpoly", " Found root " << tmp);
            res.emplace_back(tmp);
        }
        lp_value_destruct(&roots[i]);
    }

    delete[] roots;

    std::sort(res.begin(), res.end());

    return RealRootsResult<LPRealAlgebraicNumber>::roots_response(std::move(res));
}
}

#endif