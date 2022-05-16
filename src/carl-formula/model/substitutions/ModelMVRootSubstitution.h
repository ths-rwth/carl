#pragma once

#include "../ModelSubstitution.h"
#include "../evaluation/ModelEvaluation.h"

namespace carl {
	template<typename Rational, typename Poly>
	class ModelMVRootSubstitution: public ModelSubstitution<Rational,Poly> {
	public:
		using MVRoot = MultivariateRoot<Poly>;
	private:
		using Super = ModelSubstitution<Rational,Poly>;
		MVRoot mRoot;
	public:
		ModelMVRootSubstitution(const MVRoot& r): ModelSubstitution<Rational,Poly>(), mRoot(r)
		{}
		virtual void multiplyBy(const Rational&) {}
		virtual void add(const Rational&) {}
		virtual ModelSubstitutionPtr<Rational,Poly> clone() const {
			return createSubstitutionPtr<Rational,Poly,ModelMVRootSubstitution>(mRoot);
		}
		virtual Formula<Poly> representingFormula( const ModelVariable& mv ) {
			assert(mv.isVariable());
			return Formula<Poly>(VariableComparison<Poly>(mv.asVariable(), mRoot, Relation::EQ));
		}
		virtual ModelValue<Rational,Poly> evaluateSubstitution(const Model<Rational,Poly>& m) const {
			return evaluate(mRoot, m);
		}
		virtual bool dependsOn(const ModelVariable& var) const {
			if (!var.isVariable()) return false;
			return mRoot.poly().degree(var.asVariable()) > 0;
		}
		virtual void print(std::ostream& os) const {
			os << mRoot;
		}
	};
}
