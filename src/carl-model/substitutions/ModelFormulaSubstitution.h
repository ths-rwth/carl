#pragma once

#include "../ModelSubstitution.h"
#include "../evaluation/ModelEvaluation.h"

namespace carl {
	template<typename Rational, typename Poly>
	class ModelFormulaSubstitution: public ModelSubstitution<Rational,Poly> {
	private:
		using Super = ModelSubstitution<Rational,Poly>;
		Formula<Poly> mFormula;
	public:
		ModelFormulaSubstitution(const Formula<Poly>& f): ModelSubstitution<Rational,Poly>(), mFormula(f)
		{}
		virtual void multiplyBy(const Rational&) {}
		virtual void add(const Rational&) {}
		virtual ModelSubstitutionPtr<Rational,Poly> clone() const {
			return createSubstitutionPtr<Rational,Poly,ModelFormulaSubstitution>(mFormula);
		}
		virtual Formula<Poly> representingFormula( const ModelVariable& mv ) {
			assert(mv.isVariable());
			return Formula<Poly>(FormulaType::IFF, Formula<Poly>(mv.asVariable()), mFormula);
		}
		virtual ModelValue<Rational,Poly> evaluateSubstitution(const Model<Rational,Poly>& m) const {
			return model::evaluate(mFormula, m);
		}
		virtual bool dependsOn(const ModelVariable& var) const {
			if (var.isVariable()) {
				return mFormula.variables().count(var.asVariable()) > 0;
			} else if (var.isBVVariable()) {
				
			} else if (var.isUVariable()) {
				
			} else if (var.isFunction()) {
				
			}
			return false;
		}
		virtual void print(std::ostream& os) const {
			os << mFormula;
		}
		
		const Formula<Poly>& getFormula() const {
			return mFormula;
		}
	};
}
