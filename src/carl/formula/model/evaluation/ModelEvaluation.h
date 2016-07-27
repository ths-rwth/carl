#pragma once

#include "../Model.h"

#include "ModelEvaluation_helper.h"

#include "ModelEvaluation_MVRoot.h"
#include "ModelEvaluation_Polynomial.h"

namespace carl {
namespace model {

template<typename T, typename Rational, typename Poly>
T substitute(const T& t, const Model<Rational,Poly>& m) {
	T res = t;
	substitute(res, m);
	return res;
}

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

}
}
