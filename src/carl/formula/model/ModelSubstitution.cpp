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
	ModelValue<Rational,Poly> ModelPolynomialSubstitution<Rational,Poly>::evaluate(Model<Rational,Poly>& model) {
		RealAlgebraicNumberEvaluation::RANMap<Rational> map;
		for (const auto& var: mVars) {
			auto it = model.find(ModelVariable(var));
			if (it == model.end()) {
				return Poly(var);
			}
			const ModelValue<Rational,Poly>& mv = getModelValue(it,model);
			assert( !mv.isSubstitution() );
			if (mv.isRational()) {
				CARL_LOG_WARN("carl.formula.model", "Substituting " << var << " = " << mv.asRational() << " into " << mPoly);
				mPoly.substituteIn(var, Poly(mv.asRational()));
				CARL_LOG_WARN("carl.formula.model", "-> " << mPoly);
			} else if (mv.isRAN()) {
				CARL_LOG_WARN("carl.formula.model", "Substituting " << var << " = " << mv.asRAN() << " into " << mPoly);
				map.emplace(var, mv.asRAN());
			} else if (mv.isPoly()) {
				CARL_LOG_WARN("carl.formula.model", "Substituting " << var << " = " << mv.asPoly() << " into " << mPoly);
				mPoly.substituteIn(var, mv.asPoly());
				CARL_LOG_WARN("carl.formula.model", "-> " << mPoly);
			} else {
                            return this;
			}	
		}
		if (map.size() > 0) {
            RealAlgebraicNumber<Rational> ran = RealAlgebraicNumberEvaluation::evaluate(mPoly, map);
            if (ran.isNumeric())
            	return ModelValue<Rational,Poly>(ran.value());
			return ModelValue<Rational,Poly>(ran);
		}
		return mPoly;
	}
}
