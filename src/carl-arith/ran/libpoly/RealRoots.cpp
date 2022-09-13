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

    poly::Interval inter_poly = to_libpoly_interval(interval);

    // Multivariate Polynomial
    // build the assignment
    poly::Assignment assignment;
    Variable mainVar = polynomial.main_var();
    for (Variable& var : carl::variables(polynomial)) {
        if (var == mainVar) continue;
        // We convert numbers to libpoly values and add to assignment so we can substitute them later using libpoly
        lp_value_t val;
        // Turn into value
        lp_value_construct(&val, lp_value_type_t::LP_VALUE_ALGEBRAIC, m.at(var).get_internal());
        // That copies the value into the assignment
        assignment.set(VariableMapper::getInstance().getLibpolyVariable(var), poly::Value(&val));
        // Destroy the value, but dont free the algebraic number!
        lp_value_destruct(&val);
    }

    std::vector<poly::Value> roots = poly::isolate_real_roots(polynomial.get_polynomial(), assignment);

    if (roots.empty()) {
        CARL_LOG_DEBUG("carl.ran.libpoly", " Checking for nullification -> Evaluation at " << mainVar << "= 1");
        assignment.set(VariableMapper::getInstance().getLibpolyVariable(mainVar), poly::Value(long(1)));
        poly::Value eval_val = poly::evaluate(polynomial.get_polynomial(), assignment);
        CARL_LOG_DEBUG("carl.ran.libpoly", " Got eval_val " << eval_val);

        if (eval_val == poly::Value(long(0))) {
            CARL_LOG_DEBUG("carl.ran.libpoly", "poly is 0 after substituting rational assignments -> nullified");
            return RealRootsResult<LPRealAlgebraicNumber>::nullified_response();
        } else {
            CARL_LOG_DEBUG("carl.ran.libpoly", "Poly has no roots");
            return RealRootsResult<LPRealAlgebraicNumber>::no_roots_response();
        }
    }

    std::sort(roots.begin(), roots.end(), std::less<poly::Value>());

    // turn result into RealRootsResult
    std::vector<LPRealAlgebraicNumber> res;
    for (const poly::Value& val : roots) {
        auto tmp = LPRealAlgebraicNumber::create_from_value(val.get_internal());
        // filter out roots not in interval
        if (poly::contains(inter_poly, val)) {
            CARL_LOG_DEBUG("carl.ran.libpoly", " Found root " << val);
            res.emplace_back(tmp);
        }
    }

    return RealRootsResult<LPRealAlgebraicNumber>::roots_response(std::move(res));
}
}

#endif