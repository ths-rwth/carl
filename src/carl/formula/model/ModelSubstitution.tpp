#include "ModelSubstitution.h"

#include "Model.h"

#include "ran/RealAlgebraicNumberEvaluation.h"
#include "evaluation/ModelEvaluation.h"

namespace carl {

	template<typename Rational, typename Poly>
	void ModelPolynomialSubstitution<Rational,Poly>::multiplyBy( const Rational& _number )
	{
		mPoly *= _number;
	}

	template<typename Rational, typename Poly>
	void ModelPolynomialSubstitution<Rational,Poly>::add( const Rational& _number )
	{
		mPoly += _number;
	}

	template<typename Rational, typename Poly>
	ModelValue<Rational,Poly> ModelPolynomialSubstitution<Rational,Poly>::evaluateSubstitution(const Model<Rational,Poly>& model) const {
		return model::evaluate(mPoly, model);
	}

	template<typename Rational, typename Poly>
	ModelValue<Rational,Poly> ModelMVRootSubstitution<Rational,Poly>::evaluateSubstitution(const Model<Rational,Poly>& model) const {
		return model::evaluate(mRoot, model);
	}
}
