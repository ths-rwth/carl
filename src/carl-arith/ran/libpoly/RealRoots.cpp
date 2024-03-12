#include "RealRoots.h"

#ifdef USE_LIBPOLY

#include "LPAssignment.h"


namespace carl {

RealRootsResult<LPRealAlgebraicNumber> real_roots(
    const LPPolynomial& polynomial,
    const Interval<LPRealAlgebraicNumber::NumberType>& interval) {
    CARL_LOG_DEBUG("carl.ran.libpoly", " Real roots of " << polynomial << " within " << interval);

    assert(carl::is_univariate(polynomial));

    if (carl::is_zero(polynomial)) {
        CARL_LOG_TRACE("carl.ran.libpoly", "poly is 0 -> nullified");
        return RealRootsResult<LPRealAlgebraicNumber>::nullified_response();
    } else if (carl::is_constant(polynomial)) {
        CARL_LOG_TRACE("carl.ran.libpoly", "poly is constant but not zero -> no root");
        return RealRootsResult<LPRealAlgebraicNumber>::no_roots_response();
    }

    lp_upolynomial_t* upoly = lp_polynomial_to_univariate(polynomial.get_internal());

    lp_algebraic_number_t* roots = new lp_algebraic_number_t[lp_upolynomial_degree(upoly)];
    std::size_t roots_size;
    lp_upolynomial_roots_isolate(upoly, roots, &roots_size);

    if (roots_size == 0) {
        delete[] roots;
        lp_upolynomial_delete(upoly);
        CARL_LOG_DEBUG("carl.ran.libpoly", "Poly has no roots");
        return RealRootsResult<LPRealAlgebraicNumber>::no_roots_response();
    }

    lp_interval_t* inter_poly = to_libpoly_interval(interval);
    std::vector<LPRealAlgebraicNumber> res;
    for (std::size_t i = 0; i < roots_size; ++i) {
        lp_value_t tmp;
        lp_value_construct(&tmp, LP_VALUE_ALGEBRAIC, &roots[i]);
        if (lp_interval_contains(inter_poly, &tmp)) {
            res.emplace_back(std::move(tmp));
        } else {
            lp_value_destruct(&tmp);
        }
        lp_algebraic_number_destruct(&roots[i]);
    }

    lp_interval_destruct(inter_poly);
    delete inter_poly;

    delete[] roots;
    lp_upolynomial_delete(upoly);

    std::sort(res.begin(), res.end());
    return RealRootsResult<LPRealAlgebraicNumber>::roots_response(std::move(res));
}

RealRootsResult<LPRealAlgebraicNumber> real_roots(
    const LPPolynomial& polynomial,
    const std::map<Variable, LPRealAlgebraicNumber>& m,
    const Interval<LPRealAlgebraicNumber::NumberType>& interval) {
    CARL_LOG_DEBUG("carl.ran.libpoly", polynomial << " " << m << " " << interval);

    if (carl::is_univariate(polynomial)) {
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

    // Multivariate Polynomial
    // build the assignment
    Variable mainVar = polynomial.main_var();
    auto evalMap = m;
    evalMap.erase(mainVar);
    lp_assignment_t& assignment = LPAssignment::getInstance().get(evalMap);

    CARL_LOG_TRACE("carl.ran.libpoly", "Call libpoly");
    lp_value_t* roots = new lp_value_t[lp_polynomial_degree(polynomial.get_internal())];
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

        lp_value_t zero_value;
        lp_value_construct_zero(&zero_value);
        if (lp_value_cmp(eval_val, &zero_value) == 0) {
            lp_value_destruct(&zero_value);
            CARL_LOG_DEBUG("carl.ran.libpoly", "poly is 0 after substituting rational assignments -> nullified");
            lp_value_delete(eval_val);
            return RealRootsResult<LPRealAlgebraicNumber>::nullified_response();
        } else {
            lp_value_destruct(&zero_value);
            CARL_LOG_DEBUG("carl.ran.libpoly", "Poly has no roots");
            lp_value_delete(eval_val);
            return RealRootsResult<LPRealAlgebraicNumber>::no_roots_response();
        }
    }

    lp_interval_t* inter_poly = to_libpoly_interval(interval);
    std::vector<LPRealAlgebraicNumber> res;
    for (std::size_t i = 0; i < roots_size; ++i) {
        if (lp_interval_contains(inter_poly, &roots[i])) {
            res.emplace_back(std::move(roots[i]));
            CARL_LOG_DEBUG("carl.ran.libpoly", " Found root " << res.back());
        } else {
            lp_value_destruct(&roots[i]);
        }
    }
    lp_interval_destruct(inter_poly);
    delete inter_poly;
    delete[] roots;

    std::sort(res.begin(), res.end());

    return RealRootsResult<LPRealAlgebraicNumber>::roots_response(std::move(res));
}
}

#endif