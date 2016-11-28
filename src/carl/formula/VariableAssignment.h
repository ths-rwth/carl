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
		VariableAssignment(Variable::Arg v, const RAN& value): mComparison(v, value, Relation::EQ, false) {}
		
		Variable var() const {
			return mComparison.var();
		}
		RAN value() const {
			auto mv = mComparison.value();
			assert(mv.isRational() || mv.isRAN());
			if (mv.isRational()) return RAN(mv.asRational());
			else return mv.asRAN();
		}
		void collectVariables(Variables& vars) const {
			mComparison.collectVariables(vars);
		}
		
		std::string toString(unsigned = 0, bool = false, bool = true) const {
			return mComparison.toString();
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
