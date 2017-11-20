#pragma once

#include "model/mvroot/MultivariateRoot.h"
#include "model/ran/RealAlgebraicNumber.h"
#include "../core/MultivariatePolynomial.h"
#include "../core/Relation.h"
#include "../core/Variable.h"
#include "../numbers/numbers.h"

#include <boost/optional.hpp>
#include <boost/variant.hpp>

namespace carl {
  /**
   * Represent an algebraic constraint/(in)equality of the form
   * 'root(p(x,y,z), i) < a'
   * where p is a multivariate polynomial, i is a root index and 'a'
   * just any variable.  This generalizes a constraint/(in)equality of the form
   * 'root(p(x), i) < a', basically comparing 'a' to an algebraic real (that
   * uses a univariate polynomial),  to a multivariate one, where you still
   * need to plug in values for 'y' and 'z' to get the univariate polynomial.
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
		struct VariableCollector: public boost::static_visitor<Variables> {
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
		boost::optional<Constraint<Poly>> asConstraint() const {
			Relation rel = negated() ? inverse(mRelation) : mRelation;
			if (boost::get<RAN>(&mValue) == nullptr) {
				const MR& mr = boost::get<MR>(mValue);
				if (mr.poly().degree(mr.var()) != 1) return boost::none;
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
			ss << "(" << (negated() ? "!" : "") << relation() << " " << var() << " " << mValue << ")";
			return ss.str();
		}

		bool operator==(const VariableComparison& vc) const {
			return mRelation == vc.mRelation && mVar == vc.mVar && mValue == vc.mValue && mNegated == vc.mNegated;
		}
	};
	template<typename Poly>
	std::ostream& operator<<(std::ostream& os, const VariableComparison<Poly>& vc) {
		return os << vc.toString();
	}
}
