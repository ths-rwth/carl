#pragma once

#include "../Model.h"
#include "../../Constraint.h"
#include "../../Formula.h"

#include "ModelEvaluation_Polynomial.h"

namespace carl {
namespace model {

	/**
	 * Substitutes all variables from a model within a constraint.
	 * May fail to substitute some variables, for example if the values are RANs or SqrtEx.
	 */
	template<typename Rational, typename Poly>
	void substituteIn(Constraint<Poly>& c, const Model<Rational,Poly>& m) {
		c = Constraint<Poly>(substitute(c.lhs(), m), c.relation());
	}
	
	/**
	 * Evaluates a constraint to a ModelValue over a Model.
	 * If evaluation can not be done for some variables, the result may actually be a Constraint again.
	 */
	template<typename Rational, typename Poly>
	void evaluate(ModelValue<Rational,Poly>& res, Constraint<Poly>& c, const Model<Rational,Poly>& m) {
		Poly p = c.lhs();
		evaluate(res, p, m);
		if (res.isRational()) {
			res = evaluate(res.asRational(), c.relation());
		} else if (res.isRAN()) {
			res = evaluate(res.asRAN().sgn(), c.relation());
		} else {
			res = createSubstitution<Rational,Poly,ModelFormulaSubstitution<Rational,Poly>>(Formula<Poly>(Constraint<Poly>(p, c.relation())));
		}
	}
}
}
