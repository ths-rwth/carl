#pragma once

#include "../Model.h"
#include <carl/ran/real_roots.h>
#include <carl/poly/umvpoly/functions/to_univariate_polynomial.h>
#include <carl/poly/umvpoly/MultivariatePolynomial.h>
#include <carl/poly/umvpoly/UnivariatePolynomial.h>

namespace carl {
namespace model {
	/**
	 * Substitutes a variable with a rational within a polynomial.
	 */
	template<typename Rational>
	void substituteIn(MultivariatePolynomial<Rational>& p, Variable var, const Rational& r) {
		carl::substitute_inplace(p, var, MultivariatePolynomial<Rational>(r));
	}
	template<typename Poly, typename Rational>
	void substituteIn(UnivariatePolynomial<Poly>& p, Variable var, const Rational& r) {
		carl::substitute_inplace(p, var, Poly(r));
	}

	/**
	 * Substitutes a variable with a real algebraic number within a polynomial.
	 * Only works if the real algebraic number is actually numeric.
	 */
	template<typename Rational>
	void substituteIn(MultivariatePolynomial<Rational>& p, Variable var, const RealAlgebraicNumber<Rational>& r) {
		if (r.is_numeric()) substituteIn(p, var, r.value());
	}
	template<typename Poly, typename Rational>
	void substituteIn(UnivariatePolynomial<Poly>& p, Variable var, const RealAlgebraicNumber<Rational>& r) {
		if (r.is_numeric()) substituteIn(p, var, r.value());
	}

	/**
	 * Substitutes a variable with a polynomial within a polynomial.
	 */
	template<typename Rational>
	void substituteIn(MultivariatePolynomial<Rational>& p, Variable var, const MultivariatePolynomial<Rational>& r) {
		carl::substitute_inplace(p, var, r);
	}
	template<typename Poly, typename Rational>
	void substituteIn(UnivariatePolynomial<Poly>& p, Variable var, const Poly& r) {
		carl::substitute_inplace(p, var, r);
	}

	/**
	 * Substitutes all variables from a model within a polynomial.
	 * May fail to substitute some variables, for example if the values are RANs or SqrtEx.
	 */
	template<typename Rational, typename Poly, typename ModelPoly>
	void substituteIn(Poly& p, const Model<Rational,ModelPoly>& m) {
		for (auto var: carl::variables(p)) {
			auto it = m.find(var);
			if (it == m.end()) continue;
			const ModelValue<Rational,ModelPoly>& value = m.evaluated(var);
			if (value.isRational()) {
				substituteIn(p, var, value.asRational());
			} else if (value.isRAN()) {
				substituteIn(p, var, value.asRAN());
			} else if (value.isSubstitution()) {
				const auto& subs = value.asSubstitution();
				auto polysub = dynamic_cast<const ModelPolynomialSubstitution<Rational,Poly>*>(subs.get());
				if (polysub != nullptr) {
					substituteIn(p, var, polysub->getPoly());
				}
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
		
		auto map = collectRANIR(carl::variables(p).as_set(), m);
		if (map.size() == carl::variables(p).size()) {
            res = *evaluate(p, map);
			return;
		}
		res = createSubstitution<Rational,Poly,ModelPolynomialSubstitution<Rational,Poly>>(p);
	}
}
}
