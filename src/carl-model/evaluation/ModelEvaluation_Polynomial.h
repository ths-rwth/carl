#pragma once

#include "../Model.h"
#include <carl/core/polynomialfunctions/RootFinder.h>
#include <carl/core/polynomialfunctions/to_univariate_polynomial.h>
#include <carl/core/MultivariatePolynomial.h>
#include <carl/core/UnivariatePolynomial.h>

namespace carl {
namespace model {
	/**
	 * Substitutes a variable with a rational within a polynomial.
	 */
	template<typename Rational>
	void substituteIn(MultivariatePolynomial<Rational>& p, Variable var, const Rational& r) {
		p.substituteIn(var, MultivariatePolynomial<Rational>(r));
	}
	template<typename Poly, typename Rational>
	void substituteIn(UnivariatePolynomial<Poly>& p, Variable var, const Rational& r) {
		p.substituteIn(var, Poly(r));
	}

	/**
	 * Substitutes a variable with a real algebraic number within a polynomial.
	 * Only works if the real algebraic number is actually numeric.
	 */
	template<typename Rational>
	void substituteIn(MultivariatePolynomial<Rational>& p, Variable var, const RealAlgebraicNumber<Rational>& r) {
		if (r.isNumeric()) substituteIn(p, var, r.value());
	}
	template<typename Poly, typename Rational>
	void substituteIn(UnivariatePolynomial<Poly>& p, Variable var, const RealAlgebraicNumber<Rational>& r) {
		if (r.isNumeric()) substituteIn(p, var, r.value());
	}

	/**
	 * Substitutes a variable with a polynomial within a polynomial.
	 */
	template<typename Rational>
	void substituteIn(MultivariatePolynomial<Rational>& p, Variable var, const MultivariatePolynomial<Rational>& r) {
		p.substituteIn(var, r);
	}
	template<typename Poly, typename Rational>
	void substituteIn(UnivariatePolynomial<Poly>& p, Variable var, const Poly& r) {
		p.substituteIn(var, r);
	}

	/**
	 * Substitutes all variables from a model within a polynomial.
	 * May fail to substitute some variables, for example if the values are RANs or SqrtEx.
	 */
	template<typename Rational, typename Poly, typename ModelPoly>
	void substituteIn(Poly& p, const Model<Rational,ModelPoly>& m) {
		for (auto var: carl::variables(p).underlyingVariables()) {
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
		
		auto map = collectRANIR(carl::variables(p).underlyingVariableSet(), m);
		if (map.size() == carl::variables(p).size()) {
            res = RealAlgebraicNumberEvaluation::evaluate(p, map);
			return;
		}
		res = createSubstitution<Rational,Poly,ModelPolynomialSubstitution<Rational,Poly>>(p);
	}
	
	template<typename Rational, typename Poly>
	auto realRoots(const MultivariatePolynomial<Rational>& p, carl::Variable v, const Model<Rational,Poly>& m) {
		Poly tmp = substitute(p, m);
		auto map = collectRANIR(carl::variables(tmp).underlyingVariableSet(), m);
		return carl::rootfinder::realRoots(carl::to_univariate_polynomial(tmp, v), map);
	}
	template<typename Rational, typename Poly>
	auto realRoots(const UnivariatePolynomial<Poly>& p, const Model<Rational,Poly>& m) {
		UnivariatePolynomial<Poly> tmp = substitute(p, m);
		auto map = collectRANIR(carl::variables(tmp).underlyingVariableSet(), m);
		return carl::rootfinder::realRoots(tmp, map);
	}
	
	template<typename Rational, typename Poly>
	boost::optional<std::vector<RealAlgebraicNumber<Rational>>> tryRealRoots(const MultivariatePolynomial<Rational>& p, carl::Variable v, const Model<Rational,Poly>& m) {
		Poly tmp = substitute(p, m);
		CARL_LOG_DEBUG("carl.formula.model", p << " over " << m << " = " << tmp);
		auto map = collectRANIR(carl::variables(tmp).underlyingVariableSet(), m);
		CARL_LOG_DEBUG("carl.formula.model", "Remaining: " << map);
		if (map.size() + 1 != carl::variables(tmp).size()) {
			CARL_LOG_DEBUG("carl.formula.model", "Sizes of " << map << " and " << tmp.gatherVariables() << " do not match. This will not work...");
			return boost::none;
		}
		return carl::rootfinder::realRoots(carl::to_univariate_polynomial(tmp, v), map);
	}
}
}
