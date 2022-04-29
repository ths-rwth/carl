#pragma once

#include <carl-common/datastructures/hash.h>
#include <carl/ran/ran.h>
#include <carl/core/MultivariatePolynomial.h>
#include <carl/core/Relation.h>
#include <carl/core/Variable.h>
#include <carl/numbers/numbers.h>
#include <carl/core/polynomialfunctions/Representation.h>

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
		using RAN = real_algebraic_number<Number>;
	private:
		Variable mVar;
		std::variant<MR, RAN> mValue;
		Relation mRelation;
		bool mNegated;
	public:
		VariableComparison(Variable v, const std::variant<MR, RAN>& value, Relation rel, bool neg): mVar(v), mValue(value), mRelation(rel), mNegated(neg) {}
		VariableComparison(Variable v, const MR& value, Relation rel): mVar(v), mValue(value), mRelation(rel), mNegated(false) {
			if (value.isUnivariate()) {
			  // If the value of type MultivariateRoot is really just univariate, we convert it to an algebraic real.
				auto res = value.evaluate({});
				if (res) {
					mValue = *res;
					CARL_LOG_DEBUG("carl.multivariateroot", "Evaluated " << value << "-> " << mValue);
				}
			}
		}
		VariableComparison(Variable v, const RAN& value, Relation rel): mVar(v), mValue(value), mRelation(rel), mNegated(false) {}

		Variable var() const {
			return mVar;
		}
		Relation relation() const {
			return mRelation;
		}
		bool negated() const {
			return mNegated;
		}
		const std::variant<MR, RAN>& value() const {
			return mValue;
		}
		bool isEquality() const {
			return negated() ? relation() == Relation::NEQ : relation() == Relation::EQ;
		}

		/**
		 * Convert this variable comparison "v < root(..)" into a simpler
		 * polynomial (in)equality against zero "p(..) < 0" if that is possible.
		 * @return std::nullopt if conversion impossible.
		 */
		std::optional<Constraint<Poly>> asConstraint() const {
			Relation rel = negated() ? inverse(mRelation) : mRelation;
			if (std::holds_alternative<MR>(mValue)) {
				const MR& mr = std::get<MR>(mValue);
				if (mr.poly().degree(mr.var()) != 1) return std::nullopt;
				if (mr.k() != 1) return std::nullopt;
				auto lcoeff = mr.poly().coeff(mr.var(), 1);
				if (!lcoeff.isConstant()) return std::nullopt;
				auto ccoeff = mr.poly().coeff(mr.var(), 0);
				return Constraint<Poly>(Poly(mVar) + ccoeff / lcoeff, rel);
			}
			if (!std::get<RAN>(mValue).is_numeric()) return std::nullopt;
			return Constraint<Poly>(Poly(mVar) - Poly(std::get<RAN>(mValue).value()), rel);
		}

		/**
		 * Return a polynomial containing the lhs-variable that has a same root
		 * for the this lhs-variable as the value that rhs represent, e.g. if this
		 * variable comparison is 'v < 3' then a defining polynomial could be 'v-3',
		 * because it has the same root for variable v, i.e., v=3.
		 */
		Poly definingPolynomial() const {
			if (std::holds_alternative<RAN>(mValue)) {
				const auto& ran = std::get<RAN>(mValue);
				if (ran.is_numeric()) return Poly(mVar) - ran.value();
				return Poly(carl::replace_main_variable(ran.polynomial(), mVar));
			} else {
				const auto& mr = std::get<MR>(mValue);
				return mr.poly(mVar);
			}
		}
		VariableComparison negation() const {
			return VariableComparison(mVar, mValue, mRelation, !mNegated);
		}
		VariableComparison invertRelation() const {
			return VariableComparison(mVar, mValue, carl::inverse(mRelation), mNegated);
		}
		void gatherVariables(carlVariables& vars) const {
			vars.add(mVar);
			std::visit(overloaded {
				[&vars](const MR& mr) { carl::variables(mr, vars); },
				[](const RAN&) {}
			}, mValue);
		}
	};

	template<typename Pol>
    inline void variables(const VariableComparison<Pol>& f, carlVariables& vars) {
		f.gatherVariables(vars);
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
