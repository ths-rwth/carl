#pragma once

#include "../Model.h"
#include <carl/formula/model/mvroot/MultivariateRoot.h>

namespace carl {
namespace model {
	/**
	 * Substitutes a variable with a rational within a MultivariateRoot.
	 */
	template<typename Rational, typename Poly>
	void substituteIn(MultivariateRoot<Poly>& mvr, Variable::Arg var, const Rational& r) {
		mvr.substituteIn(var, Poly(r));
	}

	/**
	 * Substitutes a variable with a real algebraic number within a MultivariateRoot.
	 * Only works if the real algebraic number is actually numeric.
	 */
	template<typename Rational, typename Poly>
	void substituteIn(MultivariateRoot<Poly>& mvr, Variable::Arg var, const RealAlgebraicNumber<Rational>& r) {
		if (r.isNumeric()) substituteIn(mvr, var, r.value());
	}

	/**
	 * Substitutes all variables from a model within a MultivariateRoot.
	 * May fail to substitute some variables, for example if the values are RANs or SqrtEx.
	 */
	template<typename Rational, typename Poly>
	void substituteIn(MultivariateRoot<Poly>& mvr, const Model<Rational,Poly>& m) {
		for (auto var: carl::variables(mvr).underlyingVariables()) {
			auto it = m.find(var);
			if (it == m.end()) continue;
			const ModelValue<Rational,Poly>& value = m.evaluated(var);
			if (value.isRational()) {
				substituteIn(mvr, var, value.asRational());
			} else if (value.isRAN()) {
				substituteIn(mvr, var, value.asRAN());
			}
		}
	}
	
	/**
	 * Evaluates a MultivariateRoot to a ModelValue over a Model.
	 * If evaluation can not be done for some variables, the result may actually be a ModelMVRootSubstitution.
	 */
	template<typename Rational, typename Poly>
	void evaluate(ModelValue<Rational,Poly>& res, MultivariateRoot<Poly>& mvr, const Model<Rational,Poly>& m) {
		CARL_LOG_DEBUG("carl.model.evaluation", "Substituting " << m << " into " << mvr);
		substituteIn(mvr, m);
		
		auto map = collectRANIR(carl::variables(mvr).underlyingVariableSet(), m);
		if (map.size() == carl::variables(mvr).size()) {
			CARL_LOG_DEBUG("carl.model.evaluation", "Fully evaluating " << mvr << " over " << map);
			auto r = mvr.evaluate(map);
			if (r) {
				CARL_LOG_DEBUG("carl.model.evaluation", "Got result " << *r);
				res = *r;
				return;
			} else {
				CARL_LOG_DEBUG("carl.model.evaluation", "MVRoot does not exist.");
				res = false;
				return;
			}
		}
		CARL_LOG_DEBUG("carl.model.evaluation", "Could not evaluate, returning " << mvr);
		res = createSubstitution<Rational,Poly,ModelMVRootSubstitution<Rational,Poly>>(mvr);
	}
}
}
