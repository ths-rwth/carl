#pragma once

#include <carl-common/util/hash.h>
#include <carl-arith/ran/ran.h>
#include <carl-arith/poly/umvpoly/MultivariatePolynomial.h>
#include <carl-arith/core/Relation.h>
#include <carl-arith/core/Variable.h>
#include <carl-arith/numbers/numbers.h>
#include <carl-arith/poly/umvpoly/functions/Representation.h>
#include <carl-arith/constraint/BasicConstraint.h>

#include <optional>
#include <tuple>
#include <variant>

#include "MultivariateRoot.h"


namespace carl {
  /**
   * Represent a sum type/variant of an (in)equality between a
   * variable on the left-hand side
   * and multivariateRoot or algebraic real on the right-hand side.
   * This is basically a special purpose atomic SMT formula.
   * The lhs-variable must does not appear on the rhs.
   */
	template<typename Poly>
	class VariableComparison {
	public:
		using Number = typename UnderlyingNumberType<Poly>::type;
		using MR = MultivariateRoot<Poly>;
		using RAN = typename MultivariateRoot<Poly>::RAN;
	private:
		Variable m_var;
		std::variant<MR, RAN> m_value;
		Relation m_relation;
		bool m_negated;
	public:
		VariableComparison(Variable v, const std::variant<MR, RAN>& value, Relation rel, bool neg): m_var(v), m_value(value), m_relation(rel), m_negated(neg) {
			assert(!std::holds_alternative<MR>(m_value) || std::get<MR>(m_value).var() == m_var);
			assert(!needs_context_type<Poly>::value || !std::holds_alternative<RAN>(m_value));
		}
		VariableComparison(Variable v, const MR& value, Relation rel): m_var(v), m_value(value), m_relation(rel), m_negated(false) {
			assert(value.var() == m_var);
			if constexpr(!needs_context_type<Poly>::value) {
				if (value.is_univariate()) {
				// If the value of type MultivariateRoot is really just univariate, we convert it to an algebraic real.
					auto res = evaluate(value, carl::Assignment<RAN>({}));
					if (res) {
						m_value = *res;
						CARL_LOG_DEBUG("carl.multivariateroot", "Evaluated " << value << "-> " << m_value);
					}
				}
			}
			
		}
		VariableComparison(Variable v, const RAN& value, Relation rel): m_var(v), m_value(value), m_relation(rel), m_negated(false) {
			static_assert(!needs_context_type<Poly>::value);
		}

		Variable var() const {
			return m_var;
		}
		Relation relation() const {
			return m_relation;
		}
		bool negated() const {
			return m_negated;
		}
		const std::variant<MR, RAN>& value() const {
			return m_value;
		}
		bool is_equality() const {
			return negated() ? relation() == Relation::NEQ : relation() == Relation::EQ;
		}

		VariableComparison negation() const {
			return VariableComparison(m_var, m_value, m_relation, !m_negated);
		}
		VariableComparison invert_relation() const {
			return VariableComparison(m_var, m_value, carl::inverse(m_relation), m_negated);
		}
	};

	/**
	 * Convert this variable comparison "v < root(..)" into a simpler
	 * polynomial (in)equality against zero "p(..) < 0" if that is possible.
	 * @return std::nullopt if conversion impossible.
	 */
	template<typename Poly>
	std::optional<BasicConstraint<Poly>> as_constraint(const VariableComparison<Poly>& f) {
		Relation rel = f.negated() ? inverse(f.relation()) : f.relation();
		if (std::holds_alternative<typename VariableComparison<Poly>::MR>(f.value())) {
			const auto& mr = std::get<typename VariableComparison<Poly>::MR>(f.value());
			if (mr.poly().degree(mr.var()) != 1) return std::nullopt;
			if (mr.k() != 1) return std::nullopt;
			auto lcoeff = mr.poly().coeff(mr.var(), 1);
			if (!lcoeff.is_constant()) return std::nullopt;
			auto ccoeff = mr.poly().coeff(mr.var(), 0);
			return BasicConstraint<Poly>(Poly(f.var()) + ccoeff / lcoeff, rel);
		}
		assert(!needs_context_type<Poly>::value);
		if constexpr(!needs_context_type<Poly>::value) {
			if (!std::get<typename VariableComparison<Poly>::RAN>(f.value()).is_numeric()) return std::nullopt;
			return BasicConstraint<Poly>(Poly(f.var()) - Poly(std::get<typename VariableComparison<Poly>::RAN>(f.value()).value()), rel);
		} else {
			static_assert(dependent_false_v<Poly>);
		}
	}

	/**
	 * Return a polynomial containing the lhs-variable that has a same root
	 * for the this lhs-variable as the value that rhs represent, e.g. if this
	 * variable comparison is 'v < 3' then a defining polynomial could be 'v-3',
	 * because it has the same root for variable v, i.e., v=3.
	 */
	template<typename Poly, std::enable_if_t<!needs_context_type<Poly>::value, bool> = true>
	Poly defining_polynomial(const VariableComparison<Poly>& f) {
		if (std::holds_alternative<typename VariableComparison<Poly>::RAN>(f.value())) {
			const auto& ran = std::get<typename VariableComparison<Poly>::RAN>(f.value());
			if (ran.is_numeric()) return Poly(f.var()) - ran.value();
			else return Poly(carl::replace_main_variable(ran.polynomial(), f.var()));
		} else {
			const auto& mr = std::get<typename VariableComparison<Poly>::MR>(f.value());
			assert(mr.var() == f.var());
			return mr.poly();
		}
	}

	template<typename Poly, std::enable_if_t<needs_context_type<Poly>::value, bool> = true>
	Poly defining_polynomial(const VariableComparison<Poly>& f) {
		assert (std::holds_alternative<typename VariableComparison<Poly>::MR>(f.value()));
		const auto& mr = std::get<typename VariableComparison<Poly>::MR>(f.value());
		assert(mr.var() == f.var());
		return mr.poly();
	}

	template<typename Poly>
	boost::tribool evaluate(const VariableComparison<Poly>& f, const Assignment<typename VariableComparison<Poly>::RAN>& a) {
		typename VariableComparison<Poly>::RAN lhs;
		typename VariableComparison<Poly>::RAN rhs = a.at(f.var());
		if (std::holds_alternative<typename VariableComparison<Poly>::RAN>(f.value())) {
			lhs = std::get<typename VariableComparison<Poly>::RAN>(f.value());
		} else {
			auto res = carl::evaluate(std::get<typename VariableComparison<Poly>::MR>(f.value()), a);
			if (!res) return boost::indeterminate;
			else lhs = *res;
		}
		if (!f.negated()) return evaluate(rhs, f.relation(), lhs);
		else return !evaluate(rhs, f.relation(), lhs);
	}

	template<typename Pol>
    inline void variables(const VariableComparison<Pol>& f, carlVariables& vars) {
		vars.add(f.var());
		std::visit(overloaded {
			[&vars](const typename VariableComparison<Pol>::MR& mr) { carl::variables(mr, vars); },
			[](const typename VariableComparison<Pol>::RAN&) {}
		}, f.value());
	}

	template<typename Poly>
	bool operator==(const VariableComparison<Poly>& lhs, const VariableComparison<Poly>& rhs) {
		return lhs.relation() == rhs.relation() && lhs.var() == rhs.var() && lhs.negated() == rhs.negated() && lhs.value() == rhs.value();
	}
	template<typename Poly>
	bool operator<(const VariableComparison<Poly>& lhs, const VariableComparison<Poly>& rhs) {
		if (lhs.negated() != rhs.negated()) return !lhs.negated();
		if (lhs.var() != rhs.var()) return lhs.var() < rhs.var();
		if (lhs.relation() != rhs.relation()) return lhs.relation() < rhs.relation();
		return lhs.value() < rhs.value();
	}
	template<typename Poly>
	std::ostream& operator<<(std::ostream& os, const VariableComparison<Poly>& vc) {
		return os << "(" << vc.var() << " " << (vc.negated() ? "! " : "") << vc.relation() << " " << vc.value() << ")";
	}
}

namespace std {
	template<typename Pol>
	struct hash<carl::VariableComparison<Pol>> {
		std::size_t operator()(const carl::VariableComparison<Pol>& vc) const {
			return carl::hash_all(vc.var(), vc.value(), vc.relation(), vc.negated());
		}
	};
}
