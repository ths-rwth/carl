#pragma once

#include "../Model.h"
#include <carl/extended/MultivariateRoot.h>

namespace carl {

	/**
	 * Substitutes all variables from a model within a MultivariateRoot.
	 * May fail to substitute some variables, for example if the values are RANs or SqrtEx.
	 */
	template<typename Rational, typename Poly>
	void substitute_inplace(MultivariateRoot<Poly>& mvr, const Model<Rational,Poly>& m) {
		for (auto var: carl::variables(mvr)) {
			auto it = m.find(var);
			if (it == m.end()) continue;
			const ModelValue<Rational,Poly>& value = m.evaluated(var);
			if (value.isRational()) {
				substitute_inplace(mvr, var, Poly(value.asRational()));
			} else if (value.isRAN()) {
				if (value.asRAN().is_numeric()) substitute_inplace(mvr, var, Poly(value.asRAN().value()));
			}
		}
	}
	
	/**
	 * Evaluates a MultivariateRoot to a ModelValue over a Model.
	 * If evaluation can not be done for some variables, the result may actually be a ModelMVRootSubstitution.
	 */
	template<typename Rational, typename Poly>
	void evaluate_inplace(ModelValue<Rational,Poly>& res, MultivariateRoot<Poly>& mvr, const Model<Rational,Poly>& m) {
		CARL_LOG_DEBUG("carl.model.evaluation", "Substituting " << m << " into " << mvr);
		substitute_inplace(mvr, m);
		
		auto map = model::collectRANIR(carl::variables(mvr).as_set(), m);
		if (map.size() == carl::variables(mvr).size()) {
			CARL_LOG_DEBUG("carl.model.evaluation", "Fully evaluating " << mvr << " over " << map);
			auto r = evaluate(mvr,map);
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
