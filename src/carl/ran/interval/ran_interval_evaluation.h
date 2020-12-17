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
 * Returns std::nullopt if some unassigned variables are still contained in p after plugging in m.
 *
 * @param p Polynomial to be evaluated
 * @param m Variable assignment
 * @return Evaluation result
 */
template<typename Number>
std::optional<real_algebraic_number_interval<Number>> evaluate(MultivariatePolynomial<Number> p, const ran::ran_assignment_t<real_algebraic_number_interval<Number>>& m, bool refine_model = true) {
	CARL_LOG_DEBUG("carl.ran.evaluation", "Evaluating " << p << " on " << m);
	
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
		} 
	}
    if (p.isNumber()) {
        return real_algebraic_number_interval<Number>(p.constantPart());
    }

	std::map<Variable, Interval<Number>> var_to_interval;
	for (const auto& [var, ran] : m) {
		if (p.has(var)) {
			assert(!ran.is_numeric());
			var_to_interval.emplace(var, ran.interval());
		}
	}

    assert(!var_to_interval.empty());
	if (var_to_interval.size() == 1) {
		auto poly = carl::to_univariate_polynomial(p, m.begin()->first);
		if (m.begin()->second.sgn(poly.toNumberCoefficients()) == Sign::ZERO) {
			CARL_LOG_DEBUG("carl.ran.evaluation", "Returning " << real_algebraic_number_interval<Number>());
			return real_algebraic_number_interval<Number>();
		}
	}
	
	Interval<Number> interval = IntervalEvaluation::evaluate(p, var_to_interval);

	if (interval.isPointInterval()) {
		CARL_LOG_DEBUG("carl.ran.evaluation", "Interval is point interval " << interval);
		return real_algebraic_number_interval<Number>(interval.lower());
	}

	// compute the result polynomial
	Variable v = freshRealVariable();
	std::vector<UnivariatePolynomial<MultivariatePolynomial<Number>>> algebraic_information;
	for (const auto& cur : m) {
		algebraic_information.emplace_back(replace_main_variable(cur.second.polynomial_int(), cur.first).template convert<MultivariatePolynomial<Number>>());
	}
	// substitute RANs with low degrees first
	std::sort(algebraic_information.begin(), algebraic_information.end(), [](const auto& a, const auto& b){ 
		return a.degree() > b.degree();
	});
	auto result = ran::interval::algebraic_substitution(UnivariatePolynomial<MultivariatePolynomial<Number>>(v, {MultivariatePolynomial<Number>(-p), MultivariatePolynomial<Number>(1)}), algebraic_information);
	if (!result) {
		return std::nullopt;
	}
	UnivariatePolynomial<Number> res = *result;
	res = carl::squareFreePart(res);
	// Note that res cannot be zero as v is a fresh variable in v-p.

	CARL_LOG_DEBUG("carl.ran.evaluation", "res = " << res);
	CARL_LOG_DEBUG("carl.ran.evaluation", "var_to_interval = " << var_to_interval);
	CARL_LOG_DEBUG("carl.ran.evaluation", "p = " << p);
	CARL_LOG_DEBUG("carl.ran.evaluation", "-> " << interval);

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
				for (const auto& entry : m) {
					if (!p.has(entry.first)) var_to_interval.erase(entry.first);
				}
			} else {
				var_to_interval[var] = ran.interval();
			}
		}
		interval = IntervalEvaluation::evaluate(p, var_to_interval);
	}
	CARL_LOG_DEBUG("carl.ran.evaluation", "Result is " << res << " " << interval);
	if (interval.isPointInterval()) {
		return real_algebraic_number_interval<Number>(interval.lower());
	} else {
		return real_algebraic_number_interval<Number>(res, interval);
	}
}

template<typename Number, typename Poly>
std::optional<bool> evaluate(const Constraint<Poly>& c, const ran::ran_assignment_t<real_algebraic_number_interval<Number>>& m, bool refine_model = true, bool use_root_bounds = true) {
	CARL_LOG_DEBUG("carl.ran.evaluation", "Evaluating " << c << " on " << m);
	
	if (!use_root_bounds) {
		CARL_LOG_DEBUG("carl.ran.evaluation", "Evaluate constraint by evaluating poly");
		auto res = evaluate(c.lhs(), m);
		if (!res) return std::nullopt;
		else return evaluate(res->sgn(), c.relation());
	} else {
		Poly p = c.lhs();

		CARL_LOG_TRACE("carl.ran.evaluation", "p = " << p);

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
				CARL_LOG_TRACE("carl.ran.evaluation", "Substituting numeric value p["<<ran.value()<<"/"<<var<<"] = " << p);
			}
		}
		
		if (p.isNumber()) {
			CARL_LOG_DEBUG("carl.ran.evaluation", "Left hand side is constant");
			return carl::evaluate(p.constantPart(), c.relation());
		}
		Constraint<Poly> constr(p, c.relation());
		if (constr.isConsistent() != 2) {
			CARL_LOG_DEBUG("carl.ran.evaluation", "Constraint already evaluates to value");
			return constr.isConsistent();
		}
		p = constr.lhs(); // Constraint simplifies polynomial
		CARL_LOG_TRACE("carl.ran.evaluation", "p = " << p << " (after simplification)");

		std::map<Variable, Interval<Number>> var_to_interval;
		for (const auto& [var, ran] : m) {
			if (p.has(var)) {
				assert(!ran.is_numeric());
				var_to_interval.emplace(var, ran.interval());
			}
		}

		Interval<Number> interval = IntervalEvaluation::evaluate(p, var_to_interval);
		{
			CARL_LOG_TRACE("carl.ran.evaluation", "Interval evaluation of " << p << " under " << var_to_interval << " results in " << interval);
			auto int_res = carl::evaluate(interval, constr.relation());
			CARL_LOG_TRACE("carl.ran.evaluation", "Obtained " << interval << " " << constr.relation() << " 0 -> " << (indeterminate(int_res) ? -1 : (bool)int_res));
			if (!indeterminate(int_res)) {
				CARL_LOG_DEBUG("carl.ran.evaluation", "Result obtained by interval evaluation");
				return (bool)int_res;
			}
		}

		CARL_LOG_DEBUG("carl.ran.evaluation", "Evaluate constraint using resultants and root bounds");
		assert(var_to_interval.size() > 0);
		if (var_to_interval.size() == 1) {
			auto poly = carl::to_univariate_polynomial(p, var_to_interval.begin()->first);
			if (m.at(var_to_interval.begin()->first).sgn(poly.toNumberCoefficients()) == Sign::ZERO) {
				CARL_LOG_DEBUG("carl.ran.evaluation", "Got " << real_algebraic_number_interval<Number>());
				return evaluate(Sign::ZERO, constr.relation());
			}
		}

		// compute the result polynomial	
		Variable v = freshRealVariable();
		std::vector<UnivariatePolynomial<MultivariatePolynomial<Number>>> algebraic_information;
		for (const auto& [var, ran] : m) {
			if (var_to_interval.find(var) == var_to_interval.end()) continue;
			assert(!ran.is_numeric());
			algebraic_information.emplace_back(replace_main_variable(ran.polynomial_int(), var).template convert<MultivariatePolynomial<Number>>());
		}
		// substitute RANs with low degrees first
		std::sort(algebraic_information.begin(), algebraic_information.end(), [](const auto& a, const auto& b){ 
			return a.degree() > b.degree();
		});
		auto result = ran::interval::algebraic_substitution(UnivariatePolynomial<MultivariatePolynomial<Number>>(v, {MultivariatePolynomial<Number>(-p), MultivariatePolynomial<Number>(1)}), algebraic_information);
		// Note that res cannot be zero as v is a fresh variable in v-p.

		if (!result) {
			return std::nullopt;
		}
		UnivariatePolynomial<Number> res = *result;

		CARL_LOG_DEBUG("carl.ran.evaluation", "res = " << res);
		CARL_LOG_DEBUG("carl.ran.evaluation", "var_to_interval = " << var_to_interval);
		CARL_LOG_DEBUG("carl.ran.evaluation", "p = " << p);
		CARL_LOG_DEBUG("carl.ran.evaluation", "-> " << interval);

		// Let pos_lb a lower bound on the positive real roots and neg_ub an upper bound on the negative real roots
		// Then if the zero of res is in the interval (neg_ub,pos_lb), then it must be zero.

		// compute root bounds
		auto pos_lb = lagrangePositiveLowerBound(res);
		CARL_LOG_TRACE("carl.ran.evaluation", "positive root lower bound: " << pos_lb);
		if (pos_lb == 0) {
			// no positive root exists
			CARL_LOG_DEBUG("carl.ran.evaluation", "p <= 0");
			if (constr.relation() == Relation::GREATER) {
				return false;
			} else if (constr.relation() == Relation::LEQ) {
				return true;
			}
		}
		auto neg_ub = lagrangeNegativeUpperBound(res);
		CARL_LOG_TRACE("carl.ran.evaluation", "negative root upper bound: " << neg_ub);
		if (neg_ub == 0) {
			// no negative root exists
			CARL_LOG_DEBUG("carl.ran.evaluation", "p >= 0");
			if (constr.relation() == Relation::LESS) {
				return false;
			} else if (constr.relation() == Relation::GEQ) {
				return true;
			}
		}

		if (pos_lb == 0 && neg_ub == 0) {
			// no positive or negative zero exists
			CARL_LOG_DEBUG("carl.ran.evaluation", "p = 0");
			return evaluate(Sign::ZERO, constr.relation());
		}

		assert(!carl::isZero(res));

		// refine the interval until it is either positive or negative or is contained in (neg_ub,pos_lb)
		CARL_LOG_DEBUG("carl.ran.evaluation", "Refine until interval is in (" << neg_ub << "," << pos_lb << ") or interval is positive or negative");
		while (!((neg_ub < interval.lower() || neg_ub == 0) && (interval.upper() < pos_lb || pos_lb == 0))) {
			for (const auto& [var, ran] : m) {
				if (var_to_interval.find(var) == var_to_interval.end()) continue;
				ran.refine();
				if (ran.is_numeric()) {
					substitute_inplace(p, var, MultivariatePolynomial<Number>(ran.value()));
					for (const auto& entry : m) {
						if (!p.has(entry.first)) var_to_interval.erase(entry.first);
					}
				} else {
					var_to_interval[var] = ran.interval();
				}
			}
			interval = IntervalEvaluation::evaluate(p, var_to_interval);
			auto int_res = carl::evaluate(interval, constr.relation());
			if (!indeterminate(int_res)) {
				CARL_LOG_DEBUG("carl.ran.evaluation", "Got result");
				return (bool)int_res;
			}
		}

		CARL_LOG_DEBUG("carl.ran.evaluation", "p = 0");
		return evaluate(Sign::ZERO, constr.relation());
	}
}

}
