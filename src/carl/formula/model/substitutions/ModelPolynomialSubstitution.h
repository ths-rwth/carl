#pragma once

#include "../ModelSubstitution.h"
#include "../evaluation/ModelEvaluation.h"

namespace carl {
	template<typename Rational, typename Poly>
	class ModelPolynomialSubstitution: public ModelSubstitution<Rational,Poly> {
	private:
		using Super = ModelSubstitution<Rational,Poly>;
		Poly mPoly;
	public:
		ModelPolynomialSubstitution(const Poly& p): ModelSubstitution<Rational,Poly>(), mPoly(p)
		{}
		const auto& getPoly() const {
			return mPoly;
		}
		virtual void multiplyBy(const Rational& n) {
			mPoly *= n;
		}
		virtual void add(const Rational& n) {
			mPoly += n;
		}

		virtual ModelSubstitutionPtr<Rational,Poly> clone() const {
			return createSubstitutionPtr<Rational,Poly,ModelPolynomialSubstitution>(mPoly);
		}

		virtual Formula<Poly> representingFormula( const ModelVariable& mv ) {
			assert(mv.isVariable());
			return Formula<Poly>(mPoly - mv.asVariable(), Relation::EQ);
		}
		virtual ModelValue<Rational,Poly> evaluateSubstitution(const Model<Rational,Poly>& model) const {
			return model::evaluate(mPoly, model);
		}
		virtual bool dependsOn(const ModelVariable& var) const {
			if (!var.isVariable()) return false;
			return mPoly.degree(var.asVariable()) > 0;
		}
		virtual void print(std::ostream& os) const {
			os << mPoly;
		}
	};
}
