#pragma once

#include "VariableComparison.h"
#include "model/ran/RealAlgebraicNumber.h"
#include "model/ModelValue.h"
#include "../core/Relation.h"
#include "../core/Variable.h"

namespace carl {
	template<typename Poly>
	class VariableAssignment {
	private:
		using Base = VariableComparison<Poly>;
	public:
		using Number = typename Base::Number;
		using MR = typename Base::MR;
		using RAN = typename Base::RAN;
	private:
		Base mComparison;
	public:	
		VariableAssignment(Variable::Arg v, const RAN& value, bool negated = false): mComparison(v, value, Relation::EQ, negated) {}
		VariableAssignment(Variable::Arg v, const Number& value, bool negated = false): mComparison(v, RAN(value), Relation::EQ, negated) {}
		
		Variable var() const {
			return mComparison.var();
		}
		const RAN& value() const {
			const auto& val = mComparison.value();
			assert(boost::get<RAN>(&val) != nullptr);
			return boost::get<RAN>(val);
		}
		bool negated() const {
			return mComparison.negated();
		}
		VariableAssignment negation() const {
			return VariableAssignment(var(), value(), !negated());
		}
		operator const VariableComparison<Poly>&() const {
			return mComparison;
		}
		void collectVariables(Variables& vars) const {
			mComparison.collectVariables(vars);
		}
		
		std::string toString(unsigned = 0, bool = false, bool = true) const {
			std::stringstream ss;
			ss << "(" << var() << (negated() ? " -!> " : " -> ") << value() << ")";
			return ss.str();
		}
		
		bool operator==(const VariableAssignment& va) const {
			return mComparison == va.mComparison;
		}
	};
	template<typename Poly>
	std::ostream& operator<<(std::ostream& os, const VariableAssignment<Poly>& va) {
		return os << va.toString();
	}
}
