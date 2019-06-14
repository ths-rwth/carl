#pragma once

#include "../Model.h"

/**
 * This file provides mechanisms to substitute a model into an expression and to evaluate an expression over a model.
 */

namespace carl {
namespace model {
	
template<typename T, typename Rational, typename Poly>
T substitute(const T& t, const Model<Rational,Poly>& m);

template<typename T, typename Rational, typename Poly>
ModelValue<Rational,Poly> evaluate(const T& t, const Model<Rational,Poly>& m);

}
}

#include "ModelEvaluation_helper.h"

#include "ModelEvaluation_Bitvector.h"
#include "ModelEvaluation_Constraint.h"
#include "ModelEvaluation_Formula.h"
#include "ModelEvaluation_MVRoot.h"
#include "ModelEvaluation_Polynomial.h"
#include "ModelEvaluation_Uninterpreted.h"

namespace carl {
namespace model {

/**
 * Substitutes a model into an expression t.
 * The result is always an expression of the same type.
 * This may not be possible for some expressions, for example for uninterpreted equalities.
 */
template<typename T, typename Rational, typename Poly>
T substitute(const T& t, const Model<Rational,Poly>& m) {
	T res = t;
	substituteIn(res, m);
	return res;
}

/**
 * Evaluates a given expression t over a model.
 * The result is always a ModelValue, though it may be a ModelSubstitution in some cases.
 */
template<typename T, typename Rational, typename Poly>
ModelValue<Rational,Poly> evaluate(const T& t, const Model<Rational,Poly>& m) {
	T tmp = t;
	ModelValue<Rational,Poly> res;
	evaluate(res, tmp, m);
	if (res.isRAN() && res.asRAN().isNumeric()) {
		res = res.asRAN().value();
	}
	return res;
}

template<typename T, typename Rational, typename Poly>
unsigned satisfiedBy(const T& t, const Model<Rational,Poly>& m) {
	auto mv = evaluate(t, m);
	if (mv.isBool()) {
		return mv.asBool() ? 1 : 0;
	}
	return 2;
}

}
}
