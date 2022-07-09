#pragma once

#include "../Model.h"
#include <carl-formula/arithmetic/Constraint.h>
#include <carl-formula/formula/Formula.h>

#include "ModelEvaluation_Polynomial.h"

namespace carl {

	/**
	 * Substitutes all variables from a model within a constraint.
	 * May fail to substitute some variables, for example if the values are RANs or SqrtEx.
	 */
	template<typename Rational, typename Poly>
	void substitute_inplace(Constraint<Poly>& c, const Model<Rational,Poly>& m) {
		c = Constraint<Poly>(substitute(c.lhs(), m), c.relation());
	}
	
	/**
	 * Evaluates a constraint to a ModelValue over a Model.
	 * If evaluation can not be done for some variables, the result may actually be a Constraint again.
	 */
	template<typename Rational, typename Poly>
	void evaluate_inplace(ModelValue<Rational,Poly>& res, Constraint<Poly>& c, const Model<Rational,Poly>& m) {
		substitute_inplace(c, m);
		
		auto map = model::collectRANIR(carl::variables(c.lhs()).as_set(), m);
		if (map.size() == carl::variables(c.lhs()).size()) {
			auto eval_res = evaluate(c.constr(), map);
			assert(!indeterminate(eval_res));
			res = (bool)eval_res;
			return;
		}

		Poly p = c.lhs();
		// evaluate(res, p, m);
		// if (res.isRational()) {
		// 	  res = evaluate(res.asRational(), c.relation());
		// } else if (res.isRAN()) {
		// 	  res = evaluate(sgn(res.asRAN()), c.relation());
		// } else {
		res = createSubstitution<Rational,Poly,ModelFormulaSubstitution<Rational,Poly>>(Formula<Poly>(Constraint<Poly>(p, c.relation())));
		// }
	}

}
