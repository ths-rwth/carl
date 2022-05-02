#pragma once

#include "../ModelSubstitution.h"
#include "../evaluation/ModelEvaluation.h"

namespace carl {
	template<typename Rational, typename Poly>
	class ModelConditionalSubstitution: public ModelSubstitution<Rational,Poly> {
	private:
		using Super = ModelSubstitution<Rational,Poly>;
		std::vector<std::pair<Formula<Poly>, ModelValue<Rational,Poly>>> mValues;
	public:
		ModelConditionalSubstitution(const std::vector<std::pair<Formula<Poly>, ModelValue<Rational,Poly>>>& values): ModelSubstitution<Rational,Poly>(), mValues(values)
		{}
		ModelConditionalSubstitution(std::initializer_list<std::pair<Formula<Poly>, ModelValue<Rational,Poly>>> values): ModelSubstitution<Rational,Poly>(), mValues(values)
		{}
		virtual void multiplyBy(const Rational& n) {
			assert(false);
		}
		virtual void add(const Rational& n) {
			assert(false);
		}
		virtual ModelSubstitutionPtr<Rational,Poly> clone() const {
			return createSubstitutionPtr<Rational,Poly,ModelConditionalSubstitution>(mValues);
		}

		virtual Formula<Poly> representingFormula(const ModelVariable& mv) {
			assert(mv.isVariable());
			Formulas<Poly> subs;
			for (const auto& v: mValues) {
				//subs.emplace_back(Formula<Poly>())
			}
			return Formula<Poly>(FormulaType::AND, std::move(subs));
		}
		virtual ModelValue<Rational,Poly> evaluateSubstitution(const Model<Rational,Poly>& model) const {
			//return model::evaluate(mPoly, model);
			return true;
		}
		virtual bool dependsOn(const ModelVariable& var) const {
			if (!var.isVariable()) return false;
			for (const auto& v: mValues) {
				
			}
			return false;
		}
		virtual void print(std::ostream& os) const {
			os << "(";
			for (const auto& v: mValues) {
				os << "if " << v.first << ": " << v.second << "; ";
			}
		}
	};
}
