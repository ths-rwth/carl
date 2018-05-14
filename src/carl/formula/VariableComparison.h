#pragma once

#include "model/mvroot/MultivariateRoot.h"
#include "model/ran/RealAlgebraicNumber.h"
#include "../core/MultivariatePolynomial.h"
#include "../core/Relation.h"
#include "../core/Variable.h"
#include "../numbers/numbers.h"
#include "../util/hash.h"

#include <boost/optional.hpp>
#include <boost/variant.hpp>

#include <tuple>

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
		Variable mVar;
		boost::variant<MR, RAN> mValue;
		Relation mRelation;
		bool mNegated;
		//struct ValueToModelValue: public boost::static_visitor<ModelValue<Number,Poly>> {
		//	ModelValue<Number,Poly> operator()(const MR& mr) const {
		//		return mr;
		//	}
		//	ModelValue<Number,Poly> operator()(const RAN& ran) const {
		//		return ran;
		//	}
		//};
		struct VariableCollector: boost::static_visitor<Variables> {
			Variables operator()(const MR& mr) const {
				return mr.gatherVariables();
			}
			Variables operator()(const RAN&) const {
				return Variables();
			}
		};
	public:
		VariableComparison(Variable v, const boost::variant<MR, RAN>& value, Relation rel, bool neg): mVar(v), mValue(value), mRelation(rel), mNegated(neg) {}
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
		const boost::variant<MR, RAN>& value() const {
			return mValue;
		}
		bool isEquality() const {
			return negated() ? relation() == Relation::NEQ : relation() == Relation::EQ;
		}

		/**
		 * Convert this variable comparison "v < root(..)" into a simpler
		 * polynomial (in)equality against zero "p(..) < 0" if that is possible.
		 * @return boost::none if conversion impossible.
		 */
		boost::optional<Constraint<Poly>> asConstraint() const {
			Relation rel = negated() ? inverse(mRelation) : mRelation;
			if (boost::get<RAN>(&mValue) == nullptr) {
				const MR& mr = boost::get<MR>(mValue);
				if (mr.poly().degree(mr.var()) != 1) return boost::none;
				if (mr.k() != 1) return boost::none;
				auto lcoeff = mr.poly().coeff(mr.var(), 1);
				if (!lcoeff.isConstant()) return boost::none;
				auto ccoeff = mr.poly().coeff(mr.var(), 0);
				return Constraint<Poly>(Poly(mVar) + ccoeff / lcoeff, rel);
			}
			if (!boost::get<RAN>(mValue).isNumeric()) return boost::none;
			return Constraint<Poly>(Poly(mVar) - Poly(boost::get<RAN>(mValue).value()), rel);
		}
		Poly definingPolynomial() const {
			if (boost::get<RAN>(&mValue) != nullptr) {
				const auto& ran = boost::get<RAN>(mValue);
				if (ran.isNumeric()) return Poly(mVar) - ran.value();
				return Poly(ran.getIRPolynomial().replaceVariable(mVar));
			} else {
				const auto& mr = boost::get<MR>(mValue);
				return mr.poly(mVar);
			}
		}
		VariableComparison negation() const {
			return VariableComparison(mVar, mValue, mRelation, !mNegated);
		}
		VariableComparison invertRelation() const {
			return VariableComparison(mVar, mValue, carl::inverse(mRelation), mNegated);
		}
		void collectVariables(Variables& vars) const {
			vars.insert(mVar);
			auto newVars = boost::apply_visitor(VariableCollector(), mValue);
			vars.insert(newVars.begin(), newVars.end());
		}

		std::string toString(unsigned = 0, bool = false, bool = true) const {
			std::stringstream ss;
			ss << "(" << var() << " " << (negated() ? "! " : "") << relation() << " " << mValue << ")";
			return ss.str();
		}
	};

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
		return os << vc.toString();
	}
}

namespace std {
	template<typename Pol>
	struct hash<carl::VariableComparison<Pol>> {
		std::size_t operator()(const carl::VariableComparison<Pol>& vc) const {
			return carl::hash_all(vc.var(), variant_hash(vc.value()), vc.relation(), vc.negated());
		}
	};
}
