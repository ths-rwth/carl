

%module ConstraintRationalT
%{
#include <carl/formula/Constraint.h>
#include <carl/core/SimpleConstraint.h>
#include <carl/formula/Formula.h>
#include "gmpxx.h"
#include <carl/core/RationalFunction.h>
#include <carl/core/Relation.h>


typedef mpq_class Rational;

typedef carl::Monomial::Arg Monomial;
typedef carl::Term<Rational> Term;
typedef carl::MultivariatePolynomial<Rational> Polynomial;
typedef carl::FactorizedPolynomial<Polynomial> FactorizedPolynomial;
typedef carl::RationalFunction<Polynomial> RationalFunction;
typedef carl::RationalFunction<FactorizedPolynomial> FactorizedRationalFunction;
typedef carl::PolynomialFactorizationPair<Polynomial> FactorizationPair;

%}

%import "formula.i"

%include "std_string.i"
%import "variable.i"
%import "polynomial.i"
%import "rationalfunction.i"
%import "factorizedpolynomial.i"
%include "std_vector.i"
%include "std_map.i"


typedef carl::MultivariatePolynomial<Rational> Polynomial;








typedef mpq_class Rational;

typedef carl::Monomial::Arg Monomial;
typedef carl::Term<Rational> Term;
typedef carl::FactorizedPolynomial<Polynomial> FactorizedPolynomial;
typedef carl::RationalFunction<Polynomial> RationalFunction;
typedef carl::RationalFunction<FactorizedPolynomial> FactorizedRationalFunction;
typedef carl::PolynomialFactorizationPair<Polynomial> FactorizationPair;




namespace carl {

enum class Relation { EQ = 0, NEQ = 1, LESS = 2, LEQ = 4, GREATER = 3, GEQ = 5 };



template<typename Pol> class Constraint {
	    public:
	        Constraint( bool _valid = true );
  
            explicit Constraint( carl::Variable::Arg _var, carl::Relation _rel,  const typename Pol::NumberType& _bound = constant_zero<typename Pol::NumberType>::get() );
            
            explicit Constraint( const Pol& _lhs, carl::Relation _rel );
		
	    unsigned satisfiedBy( const std::map<Variable,Rational>& _assignment ) const;

            std::string toString( unsigned _unequalSwitch = 0, bool _infix = true, bool _friendlyVarNames = true ) const;

	    const Pol& lhs() const;

            carl::Relation relation() const;

//TODO: maybe find fix for the weird underscore names
	%extend {
		carl::Formula<Pol> not_() {
			return carl::Formula<Pol>(carl::FormulaType::NOT, carl::Formula<Pol>(*$self));
		}

		carl::Formula<Pol> and_(const Constraint& rhs) {
			return carl::Formula<Pol>(carl::FormulaType::AND, carl::Formula<Pol>(*$self), carl::Formula<Pol>(rhs));
		}

		carl::Formula<Pol> and_(const carl::Formula<Pol>& rhs) {
			return carl::Formula<Pol>(carl::FormulaType::AND, carl::Formula<Pol>(*$self), rhs);
		}

		carl::Formula<Pol> or_(const Constraint& rhs) {
			return carl::Formula<Pol>(carl::FormulaType::OR, carl::Formula<Pol>(*$self), carl::Formula<Pol>(rhs));
		}

		carl::Formula<Pol> or_(const carl::Formula<Pol>& rhs) {
			return carl::Formula<Pol>(carl::FormulaType::OR, carl::Formula<Pol>(*$self), rhs);
		}

		carl::Formula<Pol> xor_(const Constraint& rhs) {
			return carl::Formula<Pol>(carl::FormulaType::XOR, carl::Formula<Pol>(*$self), carl::Formula<Pol>(rhs));
		}

		carl::Formula<Pol> xor_(const carl::Formula<Pol>& rhs) {
			return carl::Formula<Pol>(carl::FormulaType::XOR, carl::Formula<Pol>(*$self), rhs);
		}

	}

	

	};

template<typename LhsType> class SimpleConstraint {
         public:
		SimpleConstraint(bool v) : mLhs(v ? 0 : 1), mRelation(carl::Relation::EQ) {}
		SimpleConstraint(const LhsType& lhs, carl::Relation rel) : mLhs(lhs), mRelation(rel)
		{}
	
		const LhsType& lhs() const {return mLhs;}
		const carl::Relation& rel() const {return mRelation;}
		
		%extend {
			std::string toString() {
			     std::stringstream ss;
    			     ss << *$self;
    			     return ss.str();	
			}
		}

};
}


%include "std_map.i"

%template(ConstraintRational) carl::Constraint<Polynomial>;
%template(SimpleConstraintRational) carl::SimpleConstraint<Polynomial>;
%template(SimpleConstraintFunc) carl::SimpleConstraint<FactorizedRationalFunction>;
