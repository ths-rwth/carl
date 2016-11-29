#pragma once

#include "../Model.h"
#include "../mvroot/MultivariateRoot.h"

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
		for (auto var: mvr.gatherVariables()) {
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
		substituteIn(mvr, m);
		
		auto map = collectRANIR(mvr.gatherVariables(), m);
		if (map.size() == mvr.gatherVariables().size()) {
			auto r = mvr.evaluate(map);
			if (r) {
				res = *r;
				return;
			}
		}
		res = createSubstitution<Rational,Poly,ModelMVRootSubstitution<Rational,Poly>>(mvr);
	}
}
}
