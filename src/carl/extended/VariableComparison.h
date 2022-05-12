#pragma once

#include <carl-common/util/hash.h>
#include <carl/ran/ran.h>
#include <carl/poly/umvpoly/MultivariatePolynomial.h>
#include <carl/core/Relation.h>
#include <carl/core/Variable.h>
#include <carl/numbers/numbers.h>
#include <carl/poly/umvpoly/functions/Representation.h>
#include <carl/constraint/BasicConstraint.h>

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
		using RAN = RealAlgebraicNumber<Number>;
	private:
		Variable m_var;
		std::variant<MR, RAN> m_value;
		Relation m_relation;
		bool m_negated;
	public:
		VariableComparison(Variable v, const std::variant<MR, RAN>& value, Relation rel, bool neg): m_var(v), m_value(value), m_relation(rel), m_negated(neg) {}
		VariableComparison(Variable v, const MR& value, Relation rel): m_var(v), m_value(value), m_relation(rel), m_negated(false) {
			if (value.isUnivariate()) {
			  // If the value of type MultivariateRoot is really just univariate, we convert it to an algebraic real.
				auto res = evaluate(value, carl::Assignment<RAN>({}));
				if (res) {
					m_value = *res;
					CARL_LOG_DEBUG("carl.multivariateroot", "Evaluated " << value << "-> " << m_value);
				}
			}
		}
		VariableComparison(Variable v, const RAN& value, Relation rel): m_var(v), m_value(value), m_relation(rel), m_negated(false) {}

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

		/**
		 * Convert this variable comparison "v < root(..)" into a simpler
		 * polynomial (in)equality against zero "p(..) < 0" if that is possible.
		 * @return std::nullopt if conversion impossible.
		 */
		std::optional<BasicConstraint<Poly>> as_constraint() const {
			Relation rel = negated() ? inverse(m_relation) : m_relation;
			if (std::holds_alternative<MR>(m_value)) {
				const MR& mr = std::get<MR>(m_value);
				if (mr.poly().degree(mr.var()) != 1) return std::nullopt;
				if (mr.k() != 1) return std::nullopt;
				auto lcoeff = mr.poly().coeff(mr.var(), 1);
				if (!lcoeff.isConstant()) return std::nullopt;
				auto ccoeff = mr.poly().coeff(mr.var(), 0);
				return BasicConstraint<Poly>(Poly(m_var) + ccoeff / lcoeff, rel);
			}
			if (!std::get<RAN>(m_value).is_numeric()) return std::nullopt;
			return BasicConstraint<Poly>(Poly(m_var) - Poly(std::get<RAN>(m_value).value()), rel);
		}

		/**
		 * Return a polynomial containing the lhs-variable that has a same root
		 * for the this lhs-variable as the value that rhs represent, e.g. if this
		 * variable comparison is 'v < 3' then a defining polynomial could be 'v-3',
		 * because it has the same root for variable v, i.e., v=3.
		 */
		Poly defining_polynomial() const {
			if (std::holds_alternative<RAN>(m_value)) {
				const auto& ran = std::get<RAN>(m_value);
				if (ran.is_numeric()) return Poly(m_var) - ran.value();
				return Poly(carl::replace_main_variable(ran.polynomial(), m_var));
			} else {
				const auto& mr = std::get<MR>(m_value);
				return mr.poly(m_var);
			}
		}
		VariableComparison negation() const {
			return VariableComparison(m_var, m_value, m_relation, !m_negated);
		}
		VariableComparison invert_relation() const {
			return VariableComparison(m_var, m_value, carl::inverse(m_relation), m_negated);
		}
	};

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
