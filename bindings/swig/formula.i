

SWIG_JAVABODY_PROXY(public, public, SWIGTYPE)
SWIG_JAVABODY_TYPEWRAPPER(public, public, public, SWIGTYPE)

%module FormulaPolynomialT
%{
#include <carl/formula/Formula.h>
#include <carl/formula/Constraint.h>
#include <carl/core/SimpleConstraint.h>
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

typedef carl::Formula<Polynomial> FormulaPolynomial;
typedef std::vector<FormulaPolynomial> FormulaVector;




%}




%import "constraint.i"

%include "std_string.i"
%import "variable.i"
%import "polynomial.i"
%import "rationalfunction.i"
%import "factorizedpolynomial.i"
%include "std_vector.i"
%include "std_map.i"


typedef carl::MultivariatePolynomial<Rational> Polynomial;
typedef std::vector<FormulaPolynomial> FormulaVector;



%template(FormulaVector) std::vector<carl::Formula<carl::MultivariatePolynomial<Rational>>>;


typedef carl::Formula<Polynomial> FormulaPolynomial;




typedef mpq_class Rational;

typedef carl::Monomial::Arg Monomial;
typedef carl::Term<Rational> Term;
typedef carl::FactorizedPolynomial<Polynomial> FactorizedPolynomial;
typedef carl::RationalFunction<Polynomial> RationalFunction;
typedef carl::RationalFunction<FactorizedPolynomial> FactorizedRationalFunction;
typedef carl::PolynomialFactorizationPair<Polynomial> FactorizationPair;





namespace carl {


    enum FormulaType {
        // Generic
        ITE, EXISTS, FORALL,
        
        // Core Theory
        TRUE, FALSE,
        BOOL,
        NOT, IMPLIES, AND, OR, XOR,
        IFF, 

		// Arithmetic Theory
		CONSTRAINT,
		
		// Bitvector Theory
		BITVECTOR,
		
		// Uninterpreted Theory
		UEQ
    };


template<typename Pol> class Formula {
    public:
      explicit Formula( carl::Variable::Arg _booleanVar ):
        Formula( carl::FormulaPool<Pol>::getInstance().create( _booleanVar ) )
      {}

     explicit Formula( const carl::Constraint<Pol>& _constraint ):
        Formula( carl::FormulaPool<Pol>::getInstance().create( _constraint ) )
    {}

    explicit Formula( carl::FormulaType _type, const Formula& _subformula ):
        Formula(carl::FormulaPool<Pol>::getInstance().create(_type, std::move(Formula(_subformula))))
    {}

    explicit Formula( carl::FormulaType _type, const std::vector<Formula<Pol>>& _subasts  ):
        Formula( carl::FormulaPool<Pol>::getInstance().create( _type, _subasts ) )
    {}

//Apparently satisfiedBy no longer exists
    //unsigned satisfiedBy( const carl::EvaluationMap<typename Pol::NumberType>& _assignment ) const;

    std::string toString( bool _withActivity = false, unsigned _resolveUnequal = 0, const std::string _init = "", bool _oneline = true, bool _infix = false, bool _friendlyNames = true, bool _withVariableDefinition = false ) const;

    size_t size() const;



    carl::FormulaType getType() const;


//TODO: maybe find fix for the weird underscore names

	%extend {
		carl::Formula<Pol> not_() {
			return carl::Formula<Pol>(carl::FormulaType::NOT, carl::Formula<Pol>(*$self));
		}

		carl::Formula<Pol> and_(const carl::Constraint<Pol>& rhs) {
			return carl::Formula<Pol>(carl::FormulaType::AND, carl::Formula<Pol>(*$self), carl::Formula<Pol>(rhs));
		}

		carl::Formula<Pol> and_(const carl::Formula<Pol>& rhs) {
			return carl::Formula<Pol>(carl::FormulaType::AND, carl::Formula<Pol>(*$self), rhs);
		}

		carl::Formula<Pol> or_(const carl::Constraint<Pol>& rhs) {
			return carl::Formula<Pol>(carl::FormulaType::OR, carl::Formula<Pol>(*$self), carl::Formula<Pol>(rhs));
		}

		carl::Formula<Pol> or_(const carl::Formula<Pol>& rhs) {
			return carl::Formula<Pol>(carl::FormulaType::OR, carl::Formula<Pol>(*$self), rhs);
		}

		carl::Formula<Pol> xor_(const carl::Constraint<Pol>& rhs) {
			return carl::Formula<Pol>(carl::FormulaType::XOR, carl::Formula<Pol>(*$self), carl::Formula<Pol>(rhs));
		}

		carl::Formula<Pol> xor_(const carl::Formula<Pol>& rhs) {
			return carl::Formula<Pol>(carl::FormulaType::XOR, carl::Formula<Pol>(*$self), rhs);
		}

		//instead of iterator

		carl::Formula<Pol> getItem(int i) {
			return $self->subformulas().at(i);
		}

		bool equals(const carl::Formula<Pol>& rhs) {
			return *$self == rhs;
		}


	}


};

}


%template(FormulaPolynomial) carl::Formula<Polynomial>;







