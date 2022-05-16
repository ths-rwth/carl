#pragma once

#include "../Model.h"
#include <carl-formula/bitvector/BVConstraint.h>
#include <carl-formula/bitvector/BVTerm.h>

namespace carl {

	/**
	 * Substitutes all variables from a model within a bitvector term.
	 */
	template<typename Rational, typename Poly>
	void substitute_inplace(BVTerm& bvt, const Model<Rational,Poly>& m) {
		BVTermType type = bvt.type();
		if (type == BVTermType::CONSTANT) {
		} else if (type == BVTermType::VARIABLE) {
			auto it = m.find(bvt.variable());
			if (it == m.end()) return;
			assert(it->second.isBVValue());
			bvt = BVTerm(BVTermType::CONSTANT, it->second.asBVValue());
		} else if (typeIsUnary(type)) {
            bvt = BVTerm(type, substitute(bvt.operand(), m), bvt.index());
		} else if (typeIsBinary(type)) {
			bvt = BVTerm(type, substitute(bvt.first(), m), substitute(bvt.second(), m));
		} else if (type == BVTermType::EXTRACT) {
			bvt = BVTerm(type, substitute(bvt.operand(), m), bvt.highest(), bvt.lowest());
		} else {
			CARL_LOG_ERROR("carl.model.evaluation", "Evaluation of unknown bitvector term " << bvt << " failed.");
			assert(false);
		}
	}
	
	/**
	 * Substitutes all variables from a model within a bitvector constraint.
	 */
	template<typename Rational, typename Poly>
	void substitute_inplace(BVConstraint& bvc, const Model<Rational,Poly>& m) {
		bvc = BVConstraint::create(bvc.relation(), substitute(bvc.lhs(), m), substitute(bvc.rhs(), m));
	}
	
	/**
	 * Evaluates a bitvector term to a ModelValue over a Model.
	 */
	template<typename Rational, typename Poly>
	void evaluate_inplace(ModelValue<Rational,Poly>& res, BVTerm& bvt, const Model<Rational,Poly>& m) {
		substitute_inplace(bvt, m);
		if (bvt.type() == BVTermType::CONSTANT) {
			res = bvt.value();
		} else {
			CARL_LOG_ERROR("carl.model.evaluation", "Evaluation of bitvector term did not result in a constant but " << bvt << ".");
			assert(bvt.isConstant());
		}
	}
	
	/**
	 * Evaluates a bitvector constraint to a ModelValue over a Model.
	 */
	template<typename Rational, typename Poly>
	void evaluate_inplace(ModelValue<Rational,Poly>& res, BVConstraint& bvc, const Model<Rational,Poly>& m) {
		substitute_inplace(bvc, m);
		if (bvc.isAlwaysConsistent()) res = true;
		else if (bvc.isAlwaysInconsistent()) res = false;
		else {
			CARL_LOG_ERROR("carl.model.evaluation", "Evaluation of bitvector constraint " << bvc << " was not possible.");
			assert(false);
		}
	}

}
