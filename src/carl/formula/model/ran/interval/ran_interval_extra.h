#pragma once


#include <map>
#include <vector>

#include "ran_interval.h"
#include "LazardEvaluation.h"
#include "AlgebraicSubstitution.h"

#include <carl/util/SFINAE.h>

namespace carl {
namespace ran {
namespace interval {

// TODO move somewhere else, integrate into evaluation
template<typename Coeff, typename Number>
bool vanishes(
		const UnivariatePolynomial<Coeff>& poly,
		const std::map<Variable, real_algebraic_number_interval<Number>>& varToRANMap
) {
	CARL_LOG_FUNC("carl.ran", "Check if " << poly << " vanishes under " << varToRANMap);
	assert(varToRANMap.find(poly.mainVar()) == varToRANMap.end());

	if (carl::isZero(poly)) {
		CARL_LOG_TRACE("carl.ran", "poly is 0");
		return true;
	}
	if (poly.isNumber()) {
		CARL_LOG_TRACE("carl.ran", "poly is constant but not zero");
		return false;
	}

	UnivariatePolynomial<Coeff> polyCopy(poly);
	std::map<Variable, real_algebraic_number_interval<Number>> IRmap;

	for (Variable v: polyCopy.gatherVariables()) {
		if (v == poly.mainVar()) continue;
		if (varToRANMap.count(v) == 0) {
			CARL_LOG_TRACE("carl.ran", "poly still contains unassigned variable " << v);
			return false;
		}
		assert(varToRANMap.count(v) > 0);
		if (varToRANMap.at(v).is_numeric()) {
			substitute_inplace(polyCopy, v, Coeff(varToRANMap.at(v).value()));
		} else {
			IRmap.emplace(v, varToRANMap.at(v));
		}
	}
	if (carl::isZero(polyCopy)) {
		CARL_LOG_TRACE("carl.ran", "poly is 0 after substituting rational assignments");
		return true;
	}
	if (IRmap.empty()) {
		assert(polyCopy.isUnivariate());
		CARL_LOG_TRACE("carl.ran", "poly is still univariate");
		return false;
	} else {
		CARL_LOG_TRACE("carl.ran", poly << " in " << poly.mainVar() << ", " << varToRANMap);
		assert(IRmap.find(polyCopy.mainVar()) == IRmap.end());

		LazardEvaluation<Number,MultivariatePolynomial<Number>> le((MultivariatePolynomial<Number>(polyCopy)));
		for(auto const& [var, val] : IRmap) {
			CARL_LOG_TRACE("carl.ran", "Substitute " << var << " -> " << val << " into " << le.getLiftingPoly());
			le.substitute(var, val, false);
			CARL_LOG_TRACE("carl.ran", "LE got " << le.getLiftingPoly());
			if (carl::isZero(le.getLiftingPoly())) {
				CARL_LOG_TRACE("carl.ran", "poly vanishes after plugging in " << var << " -> " << val);
				return true;
			}
		}

		return false;
	}
}

template<typename Number, typename Coeff>
UnivariatePolynomial<Number> substitute_rans_into_polynomial(
		const UnivariatePolynomial<Coeff>& p,
		const std::map<Variable, real_algebraic_number_interval<Number>>& m,
		bool use_lazard = true
) {
	std::vector<MultivariatePolynomial<Number>> polys;
	std::vector<Variable> varOrder;

	if (use_lazard) {
		auto le = LazardEvaluation<Number, MultivariatePolynomial<Number>>(MultivariatePolynomial<Number>(p));
		for (const auto& vic: m) {
			varOrder.emplace_back(vic.first);
			auto res = le.substitute(vic.first, vic.second, true);
			if (res.first) {
				polys.emplace_back(vic.first - res.second);
			} else {
				polys.emplace_back(res.second);
			}
		}
		polys.emplace_back(le.getLiftingPoly());
		varOrder.emplace_back(p.mainVar());
	} else {
		FieldExtensions<Number, MultivariatePolynomial<Number>> fe;
		for (const auto& vic: m) {
			varOrder.emplace_back(vic.first);
			auto res = fe.extend(vic.first, vic.second);
			if (res.first) {
				polys.emplace_back(vic.first - res.second);
			} else {
				polys.emplace_back(res.second);
			}
		}
		polys.emplace_back(p);
		varOrder.emplace_back(p.mainVar());
	}

	return algebraic_substitution(polys, varOrder);
}

template<typename Number>
class ran_evaluator {
private:
	Variable m_var;
	MultivariatePolynomial<Number> m_original_poly;
	UnivariatePolynomial<MultivariatePolynomial<Number>> m_poly;
	std::map<Variable, const real_algebraic_number_interval<Number>&> m_ir_assignments;

public:

	ran_evaluator(const MultivariatePolynomial<Number>& p) : m_original_poly(p) {
		Variable m_var = freshRealVariable();
		m_poly = UnivariatePolynomial<MultivariatePolynomial<Number>>(m_var, {MultivariatePolynomial<Number>(-m_original_poly), MultivariatePolynomial<Number>(1)});
	}

	bool assign(const std::map<Variable, real_algebraic_number_interval<Number>>& m, bool refine_model = true) {
		bool evaluated = false;

		for (const auto& [var, ran] : m) {
			if (refine_model) {
				static Number min_width = Number(1)/(Number(1048576)); // 1/2^20, taken from libpoly
				while (!ran.is_numeric() && ran.interval().diameter() > min_width) {
					ran.refine();
				}
			}

			if (ran.is_numeric()) {
				evaluated |= assign(var, ran, false);
				if (evaluated) return true;
			}
		}

		for (const auto& [var, ran] : m) {
			if (!ran.is_numeric()) {
				evaluated |= assign(var, ran, false);
				if (evaluated) return true;
			}
		}

		return false;
	}

	bool assign(Variable var, const real_algebraic_number_interval<Number>& value, bool refine_model = true) {
		assert(m_ir_assignments.find(var) == m_ir_assignments.end());
		if (!m_poly.has(var)) return false;

		if (refine_model) {
			static Number min_width = Number(1)/(Number(1048576)); // 1/2^20, taken from libpoly
			while (!value.is_numeric() && value.interval().diameter() > min_width) {
				value.refine();
			}
		}

		if (value.is_numeric()) {
			carl::substitute_inplace(m_poly, var, Poly(value.value()));
			carl::substitute_inplace(m_original_poly, var, Poly(value.value()));
			return m_poly.gatherVariables() == carlVariables({ m_var });
		} else {
			m_ir_assignments.emplace(var, value);

			const auto poly = replace_main_variable(value.polynomial(), var).template convert<MultivariatePolynomial<Number>>();
			m_poly = pseudo_remainder(switch_main_variable(m_poly, var), poly);
			m_poly = carl::resultant(m_poly, poly);

			return m_poly.gatherVariables() == carlVariables({ m_var });
		}
	}

	auto value() {
		assert(m_poly.gatherVariables() == carlVariables({ m_var }) || m_original_poly.isUnivariate());

		UnivariatePolynomial<Number> res = switch_main_variable(m_poly, m_var).toNumberCoefficients();
		res = carl::squareFreePart(res);
		// Note that res cannot be zero as v is a fresh variable in v-p

		std::map<Variable, Interval<Number>> var_to_interval;
		for (const auto& [var, ran] : m_ir_assignments) {
			if (!m_original_poly.has(var)) continue;
			var_to_interval.emplace(var, ran.interval());
		}
		assert(!var_to_interval.empty());
		Interval<Number> interval = IntervalEvaluation::evaluate(m_original_poly, var_to_interval);

		if (interval.isPointInterval()) {
			return real_algebraic_number_interval<Number>(interval.lower());
		}

		auto sturm_seq = sturm_sequence(res);
		// the interval should include at least one root.
		assert(!carl::isZero(res));
		assert(carl::is_root_of(res, interval.lower()) || carl::is_root_of(res, interval.upper()) || count_real_roots(sturm_seq, interval) >= 1);
		while (!interval.isPointInterval() && (carl::is_root_of(res, interval.lower()) || carl::is_root_of(res, interval.upper()) || count_real_roots(sturm_seq, interval) != 1)) {
			// refine the result interval until it isolates exactly one real root of the result polynomial
			for (const auto& [var, ran] : m_ir_assignments) {
				if (var_to_interval.find(var) == var_to_interval.end()) continue;
				ran.refine();
				if (ran.is_numeric()) {
					substitute_inplace(m_original_poly, var, MultivariatePolynomial<Number>(ran.value()));
					for (const auto& entry : m) {
						if (!m_original_poly.has(entry.first)) var_to_interval.erase(entry.first);
					}
				} else {
					var_to_interval[var] = ran.interval();
				}
			}
			interval = IntervalEvaluation::evaluate(m_original_poly, var_to_interval);
		}
		if (interval.isPointInterval()) {
			return real_algebraic_number_interval<Number>(interval.lower());
		} else {
			return real_algebraic_number_interval<Number>(res, interval);
		}
	}
};

}
}
}