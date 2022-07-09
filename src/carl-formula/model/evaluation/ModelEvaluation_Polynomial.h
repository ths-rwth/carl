#pragma once

#include "../Model.h"

#include <carl-arith/poly/umvpoly/functions/to_univariate_polynomial.h>
#include <carl-arith/poly/umvpoly/MultivariatePolynomial.h>
#include <carl-arith/poly/umvpoly/UnivariatePolynomial.h>

namespace carl {

	/**
	 * Substitutes all variables from a model within a polynomial.
	 * May fail to substitute some variables, for example if the values are RANs or SqrtEx.
	 */
	template<typename Rational, typename Poly, typename ModelPoly>
	void substitute_inplace(Poly& p, const Model<Rational,ModelPoly>& m) {
		for (auto var: carl::variables(p)) {
			auto it = m.find(var);
			if (it == m.end()) continue;
			const ModelValue<Rational,ModelPoly>& value = m.evaluated(var);
			if (value.isRational()) {
				substitute_inplace(p, var, value.asRational());
			} else if (value.isRAN()) {
				if (value.asRAN().is_numeric()) {
					substitute_inplace(p, var, value.asRAN().value());
				}
			} else if (value.isSubstitution()) {
				const auto& subs = value.asSubstitution();
				auto polysub = dynamic_cast<const ModelPolynomialSubstitution<Rational,Poly>*>(subs.get());
				if (polysub != nullptr) {
					substitute_inplace(p, var, polysub->getPoly());
				}
			}
		}
	}
	
	/**
	 * Evaluates a polynomial to a ModelValue over a Model.
	 * If evaluation can not be done for some variables, the result may actually be a ModelPolynomialSubstitution.
	 */
	template<typename Rational, typename Poly>
	void evaluate_inplace(ModelValue<Rational,Poly>& res, Poly& p, const Model<Rational,Poly>& m) {
		substitute_inplace(p, m);
		
		auto map = model::collectRANIR(carl::variables(p).as_set(), m);
		if (map.size() == carl::variables(p).size()) {
            res = *evaluate(p, map);
			return;
		}
		res = createSubstitution<Rational,Poly,ModelPolynomialSubstitution<Rational,Poly>>(p);
	}

}
