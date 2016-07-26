#include "ModelSubstitution.h"

#include "Model.h"

#include "ran/RealAlgebraicNumberEvaluation.h"

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
		RealAlgebraicNumberEvaluation::RANMap<Rational> map;
		Poly res = mPoly;
		for (const auto& var: mVars) {
			const ModelValue<Rational,Poly>& mv = model.evaluated(var);
			assert(!mv.isSubstitution());
			if (mv.isRational()) {
				CARL_LOG_WARN("carl.formula.model", "Substituting " << var << " = " << mv.asRational() << " into " << mPoly);
				res.substituteIn(var, Poly(mv.asRational()));
				CARL_LOG_WARN("carl.formula.model", "-> " << res);
			} else if (mv.isRAN()) {
				CARL_LOG_WARN("carl.formula.model", "Substituting " << var << " = " << mv.asRAN() << " into " << mPoly);
				map.emplace(var, mv.asRAN());
			} else if (mv.isPoly()) {
				CARL_LOG_WARN("carl.formula.model", "Substituting " << var << " = " << mv.asPoly() << " into " << mPoly);
				res.substituteIn(var, mv.asPoly());
				CARL_LOG_WARN("carl.formula.model", "-> " << res);
			} else {
				return this;
			}	
		}
		if (map.size() > 0) {
            RealAlgebraicNumber<Rational> ran = RealAlgebraicNumberEvaluation::evaluate(res, map);
            if (ran.isNumeric())
            	return ModelValue<Rational,Poly>(ran.value());
			return ModelValue<Rational,Poly>(ran);
		}
		return res;
	}
}
