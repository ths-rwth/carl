#pragma once

#include "ModelValue.h"
#include "ModelVariable.h"

#include <iostream>
#include <map>
#include <memory>

namespace carl {
	template<typename Rational, typename Poly>
	class Model;
	template<typename Rational, typename Poly>
	class ModelSubstitution {
	protected:
	public:
		ModelSubstitution() {}
		virtual ~ModelSubstitution() {}
		
		/// Evaluates this substitution with respect to the given model.
		virtual ModelValue<Rational, Poly> evaluate(Model<Rational, Poly>& model) = 0;
		
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
		std::set<carl::Variable> mVars;
	public:
		ModelPolynomialSubstitution(const Poly& p): ModelSubstitution<Rational,Poly>(), mPoly(p), mVars(p.gatherVariables())
		{}
		virtual void multiplyBy( const Rational& _number );
		virtual void add( const Rational& _number );
		virtual ModelValue<Rational,Poly> evaluate(Model<Rational,Poly>& model);
		virtual bool dependsOn(const ModelVariable& var) const {
			if (!var.isVariable()) return false;
			return mPoly.degree(var.asVariable()) > 0;
		}
		virtual void print(std::ostream& os) const {
			os << mPoly;
		}
	};
	
}

#include "ModelSubstitution.tpp"
