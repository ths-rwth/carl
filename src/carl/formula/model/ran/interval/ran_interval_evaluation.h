#pragma once

#include <carl/interval/Interval.h>
#include <carl/interval/IntervalEvaluation.h>
#include <carl/interval/evaluate.h>
#include <carl/formula/Constraint.h>

#include "ran_interval.h"
#include "AlgebraicSubstitution.h"


namespace carl {

/**
 * Evaluate the given polynomial with the given values for the variables.
 * Asserts that all variables of p have an assignment in m and that m has no additional assignments.
 *
 * @param p Polynomial to be evaluated
 * @param m Variable assignment
 * @return Evaluation result
 */
template<typename Number>
real_algebraic_number_interval<Number> evaluate(MultivariatePolynomial<Number> p, const std::map<Variable, real_algebraic_number_interval<Number>>& m, bool refine_model = true) {
	CARL_LOG_DEBUG("carl.ran", "Evaluating " << p << " on " << m);
	
	std::map<Variable, Interval<Number>> var_to_interval;
	for (const auto& [var, ran] : m) {
		if (!p.has(var)) continue;
		if (refine_model) {
			static Number min_width = Number(1) / (Number(1048576)); // 1/2^20, taken from libpoly
			while (!ran.is_numeric() && ran.interval().diameter() > min_width) {
				ran.refine();
			}
		}
		if (ran.is_numeric()) {
			substitute_inplace(p, var, MultivariatePolynomial<Number>(ran.value()));
		} else {
			var_to_interval.emplace(var, ran.interval());
		}
	}
    if (p.isNumber()) {
        return real_algebraic_number_interval<Number>(p.constantPart());
    }

    assert(!var_to_interval.empty());
	if (var_to_interval.size() == 1) {
		auto poly = carl::to_univariate_polynomial(p, m.begin()->first);
		if (m.begin()->second.sgn(poly.toNumberCoefficients()) == Sign::ZERO) {
			CARL_LOG_DEBUG("carl.ran", "Returning " << real_algebraic_number_interval<Number>());
			return real_algebraic_number_interval<Number>();
		}
	}
	
	Interval<Number> interval = IntervalEvaluation::evaluate(p, var_to_interval);

	if (interval.isPointInterval()) {
		CARL_LOG_DEBUG("carl.ran", "Interval is point interval " << interval);
		return real_algebraic_number_interval<Number>(interval.lower());
	}

	// compute the result polynomial
	Variable v = freshRealVariable();
	std::vector<UnivariatePolynomial<MultivariatePolynomial<Number>>> algebraic_information;
	for (const auto& cur : m) {
		algebraic_information.emplace_back(replace_main_variable(cur.second.polynomial_int(), cur.first).template convert<MultivariatePolynomial<Number>>());
	}
	UnivariatePolynomial<Number> res = carl::algebraic_substitution(UnivariatePolynomial<MultivariatePolynomial<Number>>(v, {MultivariatePolynomial<Number>(-p), MultivariatePolynomial<Number>(1)}), algebraic_information);
	res = carl::squareFreePart(res);
	// Note that res cannot be zero as v is a fresh variable in v-p.

	CARL_LOG_DEBUG("carl.ran", "res = " << res);
	CARL_LOG_DEBUG("carl.ran", "var_to_interval = " << var_to_interval);
	CARL_LOG_DEBUG("carl.ran", "p = " << p);
	CARL_LOG_DEBUG("carl.ran", "-> " << interval);

	auto sturm_seq = sturm_sequence(res);
	// the interval should include at least one root.
	assert(!carl::isZero(res));
	assert(carl::is_root_of(res, interval.lower()) || carl::is_root_of(res, interval.upper()) || count_real_roots(sturm_seq, interval) >= 1);
	while (!interval.isPointInterval() && (carl::is_root_of(res, interval.lower()) || carl::is_root_of(res, interval.upper()) || count_real_roots(sturm_seq, interval) != 1)) {
		// refine the result interval until it isolates exactly one real root of the result polynomial
		for (const auto& [var, ran] : m) {
			if (var_to_interval.find(var) == var_to_interval.end()) continue;
			ran.refine();
			if (ran.is_numeric()) {
				substitute_inplace(p, var, MultivariatePolynomial<Number>(ran.value()));
				var_to_interval.erase(var);
			} else {
				var_to_interval[var] = ran.interval();
			}
		}
		interval = IntervalEvaluation::evaluate(p, var_to_interval);
	}
	CARL_LOG_DEBUG("carl.ran", "Result is " << res << " " << interval);
	if (interval.isPointInterval()) {
		return real_algebraic_number_interval<Number>(interval.lower());
	} else {
		return real_algebraic_number_interval<Number>(res, interval);
	}
}

template<typename Number, typename Poly>
bool evaluate(const Constraint<Poly>& c, const std::map<Variable, real_algebraic_number_interval<Number>>& m, bool refine_model = true, bool use_root_bounds = true) {
	CARL_LOG_DEBUG("carl.ran", "Evaluating " << c << " on " << m);
	
	if (!use_root_bounds) {
		CARL_LOG_DEBUG("carl.ran", "Evaluate constraint by evaluating poly");
		real_algebraic_number_interval<Number> res = evaluate(c.lhs(), m);
		return evaluate(res.sgn(), c.relation());
	} else {
		Poly p = c.lhs();

		// collect intervals and substitute rationals
		std::map<Variable, Interval<Number>> var_to_interval;
		for (const auto& [var, ran] : m) {
			if (!p.has(var)) continue;
			if (refine_model) {
				static Number min_width = Number(1) / (Number(1048576)); // 1/2^20, taken from libpoly
				while (!ran.is_numeric() && ran.interval().diameter() > min_width) {
					ran.refine();
				}
			}
			if (ran.is_numeric()) {
				substitute_inplace(p, var, MultivariatePolynomial<Number>(ran.value()));
			} else {
				var_to_interval.emplace(var, ran.interval());
			}
		}

		// check for simplification
		if (p.isNumber()) {
			CARL_LOG_DEBUG("carl.ran", "Left hand side is constant");
			return carl::evaluate(p.constantPart(), c.relation());
		}
		Constraint<Poly> constr(p, c.relation());
		if (constr.isConsistent() != 2) {
			CARL_LOG_DEBUG("carl.ran", "Constraint already evaluates to value");
			return constr.isConsistent();
		}
		p = constr.lhs(); // Constraint simplifies polynomial
		Interval<Number> interval = IntervalEvaluation::evaluate(p, var_to_interval);
		{
			auto int_res = carl::evaluate(interval, c.relation());
			if (!indeterminate(int_res)) {
				CARL_LOG_DEBUG("carl.ran", "Result obtained by interval evaluation");
				return (bool)int_res;
			}
		}

		CARL_LOG_DEBUG("carl.ran", "Evaluate constraint using resultants and root bounds");
		assert(var_to_interval.size() > 0);
		if (var_to_interval.size() == 1) {
			auto poly = carl::to_univariate_polynomial(p, var_to_interval.begin()->first);
			if (m.at(var_to_interval.begin()->first).sgn(poly.toNumberCoefficients()) == Sign::ZERO) {
				CARL_LOG_DEBUG("carl.ran", "Got " << real_algebraic_number_interval<Number>());
				return evaluate(Sign::ZERO, c.relation());
			}
		}

		// compute the result polynomial	
		Variable v = freshRealVariable();
		std::vector<UnivariatePolynomial<MultivariatePolynomial<Number>>> algebraic_information;
		for (const auto& cur : m) {
			algebraic_information.emplace_back(replace_main_variable(cur.second.polynomial_int(), cur.first).template convert<MultivariatePolynomial<Number>>());
		}
		UnivariatePolynomial<Number> res = carl::algebraic_substitution(UnivariatePolynomial<MultivariatePolynomial<Number>>(v, {MultivariatePolynomial<Number>(-p), MultivariatePolynomial<Number>(1)}), algebraic_information);
		// Note that res cannot be zero as v is a fresh variable in v-p.

		CARL_LOG_DEBUG("carl.ran", "res = " << res);
		CARL_LOG_DEBUG("carl.ran", "var_to_interval = " << var_to_interval);
		CARL_LOG_DEBUG("carl.ran", "p = " << p);
		CARL_LOG_DEBUG("carl.ran", "-> " << interval);

		// Let pos_lb a lower bound on the positive real roots and neg_ub an upper bound on the negative real roots
		// Then if the zero of res is in the interval (neg_ub,pos_lb), then it must be zero.

		// compute root bounds
		auto pos_lb = lagrangePositiveLowerBound(res);
		CARL_LOG_TRACE("carl.ran", "positive root lower bound: " << pos_lb);
		if (pos_lb == 0) {
			// no positive root exists
			CARL_LOG_DEBUG("carl.ran", "p <= 0");
			if (c.relation() == Relation::GREATER) {
				return false;
			} else if (c.relation() == Relation::LEQ) {
				return true;
			}
		}
		auto neg_ub = lagrangeNegativeUpperBound(res);
		CARL_LOG_TRACE("carl.ran", "negative root upper bound: " << neg_ub);
		if (neg_ub == 0) {
			// no negative root exists
			CARL_LOG_DEBUG("carl.ran", "p >= 0");
			if (c.relation() == Relation::LESS) {
				return false;
			} else if (c.relation() == Relation::GEQ) {
				return true;
			}
		}

		if (pos_lb == 0 && neg_ub == 0) {
			// no positive or negative zero exists
			CARL_LOG_DEBUG("carl.ran", "p = 0");
			return evaluate(Sign::ZERO, c.relation());
		}

		assert(!carl::isZero(res));

		// refine the interval until it is either positive or negative or is contained in (neg_ub,pos_lb)
		CARL_LOG_DEBUG("carl.ran", "Refine until interval is in (" << neg_ub << "," << pos_lb << ") or interval is positive or negative");
		while (!((neg_ub < interval.lower() || neg_ub == 0) && (interval.upper() < pos_lb || pos_lb == 0))) {
			for (const auto& [var, ran] : m) {
				ran.refine();
				if (ran.is_numeric()) {
					substitute_inplace(p, var, MultivariatePolynomial<Number>(ran.value()));
					var_to_interval.erase(var);
				} else {
					var_to_interval[var] = ran.interval();
				}
			}
			interval = IntervalEvaluation::evaluate(p, var_to_interval);
			auto int_res = carl::evaluate(interval, c.relation());
			if (!indeterminate(int_res)) {
				CARL_LOG_DEBUG("carl.ran", "Got result");
				return (bool)int_res;
			}
		}

		CARL_LOG_DEBUG("carl.ran", "p = 0");
		return evaluate(Sign::ZERO, c.relation());
	}
}

}
