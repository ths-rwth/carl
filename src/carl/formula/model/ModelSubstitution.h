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
	template<typename Rational, typename Poly> class Model;
	template<typename Rational, typename Poly> class ModelFormulaSubstitution;
	template<typename Rational, typename Poly> class ModelMVRootSubstitution;
	template<typename Rational, typename Poly> class ModelPolynomialSubstitution;

	/**
	 * Represent a expression for a ModelValue with variables as placeholders,
	 * where the final expression's value depends on the bindings/values of these
	 * variables. The values are given in the (abstract) form of a "carl::Model".
	 */
	template<typename Rational, typename Poly>
	class ModelSubstitution {
	private:
		mutable boost::optional<ModelValue<Rational, Poly>> mCachedValue;
		
	protected:
		/// Evaluate this substitution with respect to the given model.
		virtual ModelValue<Rational, Poly> evaluateSubstitution(const Model<Rational, Poly>& model) const = 0;
	public:
		ModelSubstitution() = default;
		virtual ~ModelSubstitution() noexcept = default;
		
		const ModelValue<Rational, Poly>& evaluate(const Model<Rational, Poly>& model) const {
			if (mCachedValue == boost::none) {
				mCachedValue = evaluateSubstitution(model);
			}
			return *mCachedValue;
		}
		void resetCache() const {
			mCachedValue = boost::none;
		}
		
		/// Check if this substitution needs the given model variable.
		virtual bool dependsOn(const ModelVariable&) const {
			return true;
		}
		/// Print this substitution to the given output stream.
		virtual void print(std::ostream& os) const {
			os << "substitution";
		}
		/// Multiply this model substitution by a rational.
		virtual void multiplyBy( const Rational& _number ) = 0;
		/// Add a rational to this model substitution.
		virtual void add( const Rational& _number ) = 0;

		/// Create a copy of this model substitution.
		virtual ModelSubstitutionPtr<Rational,Poly> clone() const = 0;
		
		virtual Formula<Poly> representingFormula( const ModelVariable& mv ) = 0;
        
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
	};
	template<typename Rational, typename Poly>
	inline std::ostream& operator<<(std::ostream& os, const ModelSubstitution<Rational,Poly>& ms) {
		ms.print(os);
		return os;
	}
	template<typename Rational, typename Poly>
	inline std::ostream& operator<<(std::ostream& os, const ModelSubstitutionPtr<Rational,Poly>& ms) {
		ms->print(os);
		return os;
	}
	
	
	template<typename Rational, typename Poly, typename Substitution, typename... Args>
	inline ModelValue<Rational,Poly> createSubstitution(Args&&... args) {
		return ModelValue<Rational,Poly>(std::make_unique<Substitution>(std::forward<Args>(args)...));
	}
	template<typename Rational, typename Poly, typename Substitution, typename... Args>
	inline ModelSubstitutionPtr<Rational,Poly> createSubstitutionPtr(Args&&... args) {
		return std::make_unique<Substitution>(std::forward<Args>(args)...);
	}
	template<typename Rational, typename Poly>
	inline ModelValue<Rational,Poly> createSubstitution(const MultivariateRoot<Poly>& mr) {
		return createSubstitution<Rational,Poly,ModelMVRootSubstitution<Rational,Poly>>(mr);
	}
}

#include "substitutions/ModelFormulaSubstitution.h"
#include "substitutions/ModelMVRootSubstitution.h"
#include "substitutions/ModelPolynomialSubstitution.h"
