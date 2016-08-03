#pragma once

#include "../Model.h"
#include "../../../core/rootfinder/RootFinder.h"

namespace carl {
namespace model {
	/**
	 * Substitutes a variable with a rational within a polynomial.
	 */
	template<typename Rational, typename Poly>
	void substituteIn(Poly& p, Variable::Arg var, const Rational& r) {
		p.substituteIn(var, Poly(r));
	}

	/**
	 * Substitutes a variable with a real algebraic number within a polynomial.
	 * Only works if the real algebraic number is actually numeric.
	 */
	template<typename Rational, typename Poly>
	void substituteIn(Poly& p, Variable::Arg var, const RealAlgebraicNumber<Rational>& r) {
		if (r.isNumeric()) substituteIn(p, var, r.value());
	}

	/**
	 * Substitutes all variables from a model within a polynomial.
	 * May fail to substitute some variables, for example if the values are RANs or SqrtEx.
	 */
	template<typename Rational, typename Poly>
	void substituteIn(Poly& p, const Model<Rational,Poly>& m) {
		for (auto var: p.gatherVariables()) {
			auto it = m.find(var);
			if (it == m.end()) continue;
			const ModelValue<Rational,Poly>& value = m.evaluated(var);
			if (value.isRational()) {
				substituteIn(p, var, value.asRational());
			} else if (value.isRAN()) {
				substituteIn(p, var, value.asRAN());
			}
		}
	}
	
	/**
	 * Evaluates a polynomial to a ModelValue over a Model.
	 * If evaluation can not be done for some variables, the result may actually be a ModelPolynomialSubstitution.
	 */
	template<typename Rational, typename Poly>
	void evaluate(ModelValue<Rational,Poly>& res, Poly& p, const Model<Rational,Poly>& m) {
		substituteIn(p, m);
		
		auto map = collectRANIR(p.gatherVariables(), m);
		if (map.size() == p.gatherVariables().size()) {
            res = RealAlgebraicNumberEvaluation::evaluate(p, map);
			return;
		}
		res = ModelSubstitution<Rational,Poly>::template create<ModelPolynomialSubstitution<Rational,Poly>>(p);
	}
	
	template<typename Rational, typename Poly>
	std::list<RealAlgebraicNumber<Rational>> realRoots(const Poly& p, carl::Variable v, const Model<Rational,Poly>& m) {
		Poly tmp = substitute(p, m);
		auto map = collectRANIR(tmp.gatherVariables(), m);
		return carl::rootfinder::realRoots(tmp.toUnivariatePolynomial(v), map);
	}
}
}
