#pragma once

#include "../Model.h"

#include <carl-formula/uninterpreted/UEquality.h>
#include <carl-formula/uninterpreted/UFInstance.h>
#include <carl-formula/uninterpreted/SortValue.h>

namespace carl {
	/**
	 * Evaluates a uninterpreted variable to a ModelValue over a Model.
	 */
	template<typename Rational, typename Poly>
	void evaluate_inplace(ModelValue<Rational,Poly>& res, const UVariable& uv, const Model<Rational,Poly>& m) {
		auto uvit = m.find(uv);
		assert(uvit != m.end());
		res = uvit->second;
	}

	/**
	 * Evaluates a uninterpreted function instance to a ModelValue over a Model.
	 */
	template<typename Rational, typename Poly>
	void evaluate_inplace(ModelValue<Rational,Poly>& res, const UFInstance& ufi, const Model<Rational,Poly>& m) {
		auto ufit = m.find(ufi.uninterpretedFunction());
		assert(ufit != m.end());
		assert(ufit->second.isUFModel());
		std::vector<SortValue> args;
		for (const auto& term: ufi.args()) {
			if(term.isUVariable()) {
				auto it = m.find(term.asUVariable());
				assert(it != m.end());
				const ModelValue<Rational,Poly>& value = m.evaluated(term.asUVariable());
				assert(value.isSortValue());
				args.push_back(value.asSortValue());
			} else if(term.isUFInstance()) {
				ModelValue<Rational,Poly> value = ModelValue<Rational,Poly>();
				evaluate_inplace(value, term.asUFInstance(), m);
				assert(value.isSortValue());
				args.push_back(value.asSortValue());
			}
		}
		res = ufit->second.asUFModel().get(args);
	}

	/**
	 * Evaluates a uninterpreted variable to a ModelValue over a Model.
	 */
	template<typename Rational, typename Poly>
	void evaluate_inplace(ModelValue<Rational,Poly>& res, const UEquality& ue, const Model<Rational,Poly>& m) {
		ModelValue<Rational,Poly> lhs;
		if (ue.lhs().isUVariable()) {
			evaluate_inplace(lhs, ue.lhs().asUVariable(), m);
		} else if (ue.lhs().isUFInstance()) {
			evaluate_inplace(lhs, ue.lhs().asUFInstance(), m);
		}
		CARL_LOG_DEBUG("carl.model.evaluation", "lhs = " << lhs);
		ModelValue<Rational,Poly> rhs;
		if (ue.rhs().isUVariable()) {
			evaluate_inplace(rhs, ue.rhs().asUVariable(), m);
		} else if (ue.rhs().isUFInstance()) {
			evaluate_inplace(rhs, ue.rhs().asUFInstance(), m);
		}
		CARL_LOG_DEBUG("carl.model.evaluation", "rhs = " << rhs);
		assert(lhs.isSortValue());
		assert(rhs.isSortValue());
		assert(lhs.asSortValue().sort() == rhs.asSortValue().sort());
		if (ue.negated()) {
			CARL_LOG_DEBUG("carl.model.evaluation", "-> " << lhs << " != " << rhs);
			res = !(lhs.asSortValue() == rhs.asSortValue());
		} else {
			CARL_LOG_DEBUG("carl.model.evaluation", "-> " << lhs << " == " << rhs);
			res = (lhs.asSortValue() == rhs.asSortValue());
		}
	}

}
