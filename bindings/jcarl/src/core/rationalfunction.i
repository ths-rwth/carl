

%module ModRationalFunction
%{
#include "RationalFunction.h"
#include "MultivariatePolynomial.h"


typedef mpq_class Rational;


typedef carl::Term<Rational> Term;
typedef carl::MultivariatePolynomial<Rational> Polynomial;
typedef carl::FactorizedPolynomial<Polynomial> FactorizedPolynomial;
typedef carl::RationalFunction<Polynomial> RationalFunction;
typedef carl::RationalFunction<FactorizedPolynomial> FactorizedRationalFunction;


typedef unsigned int uint;

%}



%include "std_string.i"
//TODO: for 32 bit support this has to be changed..
//%import <stddef> //for size_t maybe?
typedef long unsigned int size_t;  //this should be okay for 64 bits at least


%import "rational.i"
%import "variable.i"
%import "polynomial.i"

typedef carl::MultivariatePolynomial<Rational> Polynomial;

namespace carl {


template<typename Pol, bool AutoSimplify=false>
class RationalFunction
{
public:
typedef typename Pol::CoeffType CoeffType;
        explicit RationalFunction(const Pol& nom, const Pol& denom):
            mPolynomialQuotient(nullptr),
            mNumberQuotient(),
            mIsSimplified(false)
        {
        }

        CoeffType evaluate(const std::map<Variable, CoeffType>& substitutions) const;

        inline Pol nominator() const;

        inline Pol denominator() const;


%extend {
	std::string toString() {
	     std::stringstream ss;
	     ss << *$self;
	     return ss.str();	
	}

	bool equals(const carl::RationalFunction<Pol, AutoSimplify>& other) {
		return *$self == other;
	}
	bool notEquals(const carl::RationalFunction<Pol, AutoSimplify>& other) {
		return *$self != other;
	}

	std::vector<carl::Variable> gatherVariables() const {
	   std::set<carl::Variable> asSet = $self->gatherVariables();
   	   return std::vector<carl::Variable>(asSet.begin(),asSet.end());
        }
}

};

}



%template(RationalFunctionPoly) carl::RationalFunction<Polynomial>;

