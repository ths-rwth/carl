#pragma once

#include "model/mvroot/MultivariateRoot.h"
#include "model/ran/RealAlgebraicNumber.h"
#include "model/ModelValue.h"
#include "../core/MultivariatePolynomial.h"
#include "../core/Relation.h"
#include "../core/Variable.h"
#include "../numbers/numbers.h"

#include <boost/variant.hpp>

namespace carl {
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
		struct ValueToModelValue: public boost::static_visitor<ModelValue<Number,Poly>> {
			ModelValue<Number,Poly> operator()(const MR& mr) const {
				return mr;
			}
			ModelValue<Number,Poly> operator()(const RAN& ran) const {
				return ran;
			}
		};
	protected:
		VariableComparison(Variable::Arg v, const boost::variant<MR, RAN>& value, Relation rel): mVar(v), mValue(value), mRelation(rel) {}
	public:	
		VariableComparison(Variable::Arg v, const MR& value, Relation rel): mVar(v), mValue(value), mRelation(rel) {
			if (value.isUnivariate()) {
				mValue = value.evaluate({});
				CARL_LOG_DEBUG("carl.multivariateroot", "Evaluated " << value << "-> " << mValue);
			}
		}
		VariableComparison(Variable::Arg v, const RAN& value, Relation rel): mVar(v), mValue(value), mRelation(rel) {}
		
		Variable var() const {
			return mVar;
		}
		Relation relation() const {
			return mRelation;
		}
		ModelValue<Number,Poly> value() const {
			return boost::apply_visitor(ValueToModelValue(), mValue);
		}
		VariableComparison negation() const {
			return VariableComparison(mVar, mValue, inverse(mRelation));
		}
		
		std::string toString(unsigned _resolveUnequal = 0, bool _infix = false, bool _friendlyNames = true) const {
			std::stringstream ss;
			ss << "(" << mRelation << " " << mVar << " " << mValue << ")";
			return ss.str();
		}
		
		bool operator==(const VariableComparison& vc) const {
			return mRelation == vc.mRelation && mVar == vc.mVar && mValue == vc.mValue;
		}
	};
	template<typename Poly>
	std::ostream& operator<<(std::ostream& os, const VariableComparison<Poly>& vc) {
		return os << vc.toString();
	}
}
