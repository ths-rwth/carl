#pragma once

#include "../Model.h"

#include "../../uninterpreted/UEquality.h"
#include "../../uninterpreted/UFInstance.h"
#include "../uninterpreted/SortValue.h"

namespace carl {
namespace model {
	/**
	 * Evaluates a uninterpreted variable to a ModelValue over a Model.
	 */
	template<typename Rational, typename Poly>
	void evaluate(ModelValue<Rational,Poly>& res, const UVariable& uv, const Model<Rational,Poly>& m) {
		auto uvit = m.find(uv);
		assert(uvit != m.end());
		res = uvit->second;
	}
	
	/**
	 * Evaluates a uninterpreted function instance to a ModelValue over a Model.
	 */
	template<typename Rational, typename Poly>
	void evaluate(ModelValue<Rational,Poly>& res, const UFInstance& ufi, const Model<Rational,Poly>& m) {
		auto ufit = m.find(ufi.uninterpretedFunction());
		assert(ufit != m.end());
		assert(ufit->second.isUFModel());
		std::vector<SortValue> args;
		for (const auto& var: ufi.args()) {
			auto it = m.find(var);
			assert(it != m.end());
			const ModelValue<Rational,Poly>& value = m.evaluated(var);
			assert(value.isSortValue());
			args.push_back(value.asSortValue());
		}
		res = ufit->second.asUFModel().get(args);
	}
	
	/**
	 * Evaluates a uninterpreted variable to a ModelValue over a Model.
	 */
	template<typename Rational, typename Poly>
	void evaluate(ModelValue<Rational,Poly>& res, const UEquality& ue, const Model<Rational,Poly>& m) {
		ModelValue<Rational,Poly> lhs;
		if (ue.lhsIsUV()) evaluate(lhs, ue.lhsAsUV(), m);
		else if (ue.lhsIsUF()) evaluate(lhs, ue.lhsAsUF(), m);
		ModelValue<Rational,Poly> rhs;
		if (ue.rhsIsUV()) evaluate(rhs, ue.rhsAsUV(), m);
		else if (ue.rhsIsUF()) evaluate(rhs, ue.rhsAsUF(), m);
		assert(lhs.isSortValue());
		assert(rhs.isSortValue());
		assert(lhs.asSortValue().sort() == rhs.asSortValue().sort());
		res = (lhs.asSortValue() == rhs.asSortValue());
	}
}
}
