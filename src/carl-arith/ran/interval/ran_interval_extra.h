#pragma once


#include <map>
#include <vector>

#include "Ran.h"
#include "RealRoots.h"
#include "helper/LazardEvaluation.h"

#include <carl-common/meta/SFINAE.h>

namespace carl::ran::interval {


template<typename Number>
class ran_evaluator {
private:
	Variable m_var;
	MultivariatePolynomial<Number> m_original_poly;
	UnivariatePolynomial<MultivariatePolynomial<Number>> m_poly;
	std::map<Variable, const IntRepRealAlgebraicNumber<Number>&> m_ir_assignments;

public:
	ran_evaluator(const MultivariatePolynomial<Number>& p)
		: m_var(fresh_real_variable()),
		  m_original_poly(p),
		  m_poly(m_var, {MultivariatePolynomial<Number>(-m_original_poly), MultivariatePolynomial<Number>(1)})
	{}

	bool assign(const std::map<Variable, IntRepRealAlgebraicNumber<Number>>& m, bool refine_model = true) {
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

	bool assign(Variable var, const IntRepRealAlgebraicNumber<Number>& ran, bool refine_model = true) {
		assert(m_ir_assignments.find(var) == m_ir_assignments.end());
		if (m_original_poly.is_constant()) return true;
		if (!m_poly.has(var)) return false;

		CARL_LOG_TRACE("carl.ran.interval", "Assign " << var << " -> " << ran);

		if (refine_model) {
			static Number min_width = Number(1)/(Number(1048576)); // 1/2^20, taken from libpoly
			while (!ran.is_numeric() && ran.interval().diameter() > min_width) {
				ran.refine();
			}
		}

		if (ran.is_numeric()) {
			CARL_LOG_TRACE("carl.ran.interval", "Numeric: " << ran);
			carl::substitute_inplace(m_poly, var, MultivariatePolynomial<Number>(ran.value()));
			carl::substitute_inplace(m_original_poly, var, MultivariatePolynomial<Number>(ran.value()));
			CARL_LOG_TRACE("carl.ran.interval", "Remainung poly: " << m_poly << "; original: " << m_original_poly);
			assert(carl::variables(m_poly).size() > 1 || carl::variables(m_poly) == carlVariables({ m_var }));
			return carl::variables(m_poly).size() == 1 || m_original_poly.is_constant();
		} else {
			CARL_LOG_TRACE("carl.ran.interval", "Still an interval: " << ran);
			m_ir_assignments.emplace(var, ran);
			const auto poly = replace_main_variable(ran.polynomial(), var).template convert<MultivariatePolynomial<Number>>();
			m_poly = pseudo_remainder(switch_main_variable(m_poly, var), poly);
			m_poly = carl::resultant(m_poly, poly);
			m_poly = switch_main_variable(m_poly, m_var);
			CARL_LOG_TRACE("carl.ran.interval", "Remainung poly: " << m_poly << "; original: " << m_original_poly);
			assert(carl::variables(m_poly).size() > 1 || carl::variables(m_poly) == carlVariables({ m_var }));
			return carl::variables(m_poly).size() == 1 || m_original_poly.is_constant();
		}
	}

	bool has_value() const {
		return carl::variables(m_poly).size() == 1 || m_original_poly.is_constant();
	}

	auto value() {
		if (m_original_poly.is_constant()) {
			CARL_LOG_TRACE("carl.ran.interval", "Poly is constant: " << m_original_poly);
			return IntRepRealAlgebraicNumber<Number>(m_original_poly.constant_part());
		}

		assert(carl::variables(m_poly).size() == 1 && m_poly.has(m_var));

		UnivariatePolynomial<Number> res = carl::squareFreePart(m_poly.toNumberCoefficients());

		CARL_LOG_TRACE("carl.ran.interval", "Computing value of " << m_original_poly << " at " << m_ir_assignments << " using " << res);

		std::map<Variable, Interval<Number>> var_to_interval;
		for (const auto& [var, ran] : m_ir_assignments) {
			if (!m_original_poly.has(var)) continue;
			var_to_interval.emplace(var, ran.interval());
		}
		CARL_LOG_TRACE("carl.ran.interval", "Got intervals " << var_to_interval);
		assert(!var_to_interval.empty());
		Interval<Number> interval = carl::evaluate(m_original_poly, var_to_interval);

		if (interval.is_point_interval()) {
			return IntRepRealAlgebraicNumber<Number>(interval.lower());
		}

		auto sturm_seq = sturm_sequence(res);
		// the interval should include at least one root.
		assert(!carl::is_zero(res));
		assert(carl::is_root_of(res, interval.lower()) || carl::is_root_of(res, interval.upper()) || count_real_roots(sturm_seq, interval) >= 1);
		while (!interval.is_point_interval() && (carl::is_root_of(res, interval.lower()) || carl::is_root_of(res, interval.upper()) || count_real_roots(sturm_seq, interval) != 1)) {
			// refine the result interval until it isolates exactly one real root of the result polynomial
			for (const auto& [var, ran] : m_ir_assignments) {
				if (var_to_interval.find(var) == var_to_interval.end()) continue;
				ran.refine();
				if (ran.is_numeric()) {
					carl::substitute_inplace(m_original_poly, var, MultivariatePolynomial<Number>(ran.value()));
					for (const auto& entry : m_ir_assignments) {
						if (!m_original_poly.has(entry.first)) var_to_interval.erase(entry.first);
					}
				} else {
					var_to_interval[var] = ran.interval();
				}
			}
			interval = carl::evaluate(m_original_poly, var_to_interval);
		}
		if (interval.is_point_interval()) {
			return IntRepRealAlgebraicNumber<Number>(interval.lower());
		} else {
			return IntRepRealAlgebraicNumber<Number>(res, interval);
		}
	}
};

}