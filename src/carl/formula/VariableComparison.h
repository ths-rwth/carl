#pragma once

#include "model/mvroot/MultivariateRoot.h"
#include "model/ran/RealAlgebraicNumber.h"
#include "../core/MultivariatePolynomial.h"
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
	public:	
		VariableComparison(Variable::Arg v, const MR& value): mVar(v), mValue(value) {
			if (mValue->isUnivariate()) {
				mValue = mValue.evaluat({});
			}
		}
		VariableComparison(Variable::Arg v, const RAN& value): mVar(v), mValue(value) {}
		
		Variable var() const {
			return mVar;
		}
		
		std::string toString(unsigned _resolveUnequal = 0, bool _infix = false, bool _friendlyNames = true) const {
			std::stringstream ss;
			ss << "(= " << mVar << " " << mValue << ")";
			return ss.str();
		}
		
		bool operator==(const VariableComparison& vc) const {
			return mVar == vc.mVar && mValue == vc.mValue;
		}
	};
}
