#pragma once

#include <iostream>
#include <map>
#include <memory>

#include <boost/optional.hpp>

#include "../Formula.h"
#include "mvroot/MultivariateRoot.h"
#include "ModelValue.h"
#include "ModelVariable.h"

namespace carl {
	template<typename Rational, typename Poly>
	class Model;
	template<typename Rational, typename Poly>
	class ModelSubstitution {
	protected:
		mutable boost::optional<ModelValue<Rational, Poly>> mCachedValue;
		
		/// Evaluates this substitution with respect to the given model.
		virtual ModelValue<Rational, Poly> evaluateSubstitution(const Model<Rational, Poly>& model) const = 0;
	public:
		ModelSubstitution() {}
		virtual ~ModelSubstitution() {}
		
		const ModelValue<Rational, Poly>& evaluate(const Model<Rational, Poly>& model) const {
			if (mCachedValue == boost::none) {
				mCachedValue = evaluateSubstitution(model);
			}
			return *mCachedValue;
		}
		void resetCache() const {
			mCachedValue = boost::none;
		}
		
		/// Checks whether this substitution needs the given model variable.
		virtual bool dependsOn(const ModelVariable&) const {
			return true;
		}
		/// Prints this substitution to the given output stream.
		virtual void print(std::ostream& os) const {
			os << "substitution";
		}
		/// Multiplies this model substitution by a rational.
		virtual void multiplyBy( const Rational& _number ) = 0;
		/// Adds a rational to this model substitution.
		virtual void add( const Rational& _number ) = 0;
        
		template<typename Iterator>
		const ModelValue<Rational,Poly>& getModelValue( Iterator _mvit, Model<Rational,Poly>& _model )
        {
            ModelValue<Rational,Poly>& mv = _mvit->second;
            if( mv.isSubstitution() )
            {
                mv = mv.asSubstitution()->evaluate( _model );
            }
            return mv;
        }
		
		template<typename Substitution, typename... Args>
		static ModelValue<Rational,Poly> create(Args&&... args) {
			return ModelValue<Rational,Poly>(std::make_shared<Substitution>(std::forward<Args>(args)...));
		}
	};
	template<typename Rational, typename Poly>
	inline std::ostream& operator<<(std::ostream& os, const ModelSubstitution<Rational,Poly>& ms) {
		ms.print(os);
		return os;
	}
	template<typename Rational, typename Poly>
	inline std::ostream& operator<<(std::ostream& os, const std::shared_ptr<ModelSubstitution<Rational,Poly>>& ms) {
		ms->print(os);
		return os;
	}
	
	template<typename Rational, typename Poly>
	class ModelPolynomialSubstitution: public ModelSubstitution<Rational,Poly> {
	private:
		using Super = ModelSubstitution<Rational,Poly>;
		Poly mPoly;
	public:
		ModelPolynomialSubstitution(const Poly& p): ModelSubstitution<Rational,Poly>(), mPoly(p)
		{}
		virtual void multiplyBy( const Rational& _number );
		virtual void add( const Rational& _number );
		virtual ModelValue<Rational,Poly> evaluateSubstitution(const Model<Rational,Poly>& model) const;
		virtual bool dependsOn(const ModelVariable& var) const {
			if (!var.isVariable()) return false;
			return mPoly.degree(var.asVariable()) > 0;
		}
		virtual void print(std::ostream& os) const {
			os << mPoly;
		}
	};
	
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
		virtual ModelValue<Rational,Poly> evaluateSubstitution(const Model<Rational,Poly>& model) const;
		virtual bool dependsOn(const ModelVariable& var) const {
			if (!var.isVariable()) return false;
			return mRoot.poly().degree(var.asVariable()) > 0;
		}
		virtual void print(std::ostream& os) const {
			os << mRoot;
		}
	};
	
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
		virtual ModelValue<Rational,Poly> evaluateSubstitution(const Model<Rational,Poly>& model) const;
		virtual bool dependsOn(const ModelVariable& var) const {
			return mFormula.variables().count(var) > 0;
		}
		virtual void print(std::ostream& os) const {
			os << mFormula;
		}
	};
}

#include "ModelSubstitution.tpp"
