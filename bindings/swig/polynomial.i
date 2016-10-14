

%module PolynomialT
%{
#include <carl/core/MultivariatePolynomial.h>
#include <carl/core/RationalFunction.h>
#include "gmp.h"
#include "gmpxx.h"

typedef mpq_class Rational;


typedef carl::Term<Rational> Term;
typedef carl::MultivariatePolynomial<Rational> Polynomial;
typedef carl::RationalFunction<Polynomial> RationalFunction;


typedef unsigned int uint;
typedef std::pair<carl::Variable,uint> VarIntPair;
%}


%include "std_string.i"
%include "std_vector.i"

namespace std {
 %template(VarVector) vector<carl::Variable>;
}

//TODO: for 32 bit support this has to be changed..
//%import <stddef> //for size_t maybe?
typedef long unsigned int size_t;  //this should be okay for 64 bits at least

%import "rational.i"
%import "variable.i"
%import "term.i"
%import "monomial.i"
%import "rationalfunction.i"



typedef mpq_class Rational;
typedef carl::RationalFunction<Polynomial> RationalFunction;



namespace carl {

template<typename Coeff>
class MultivariatePolynomial
{
public:
typedef Coeff CoeffType;
typedef Coeff NumberType; //ATTENTION: This is only correct if polynomials are never instantiated with a type that's not a number
explicit MultivariatePolynomial(const carl::Term<Coeff>& t);
explicit MultivariatePolynomial(const std::shared_ptr<const carl::Monomial>& m);
explicit MultivariatePolynomial(Variable::Arg v);
explicit MultivariatePolynomial(const Coeff& c);
const Coeff& constantPart() const;
template<typename SubstitutionType = Coeff>
SubstitutionType evaluate(const std::map<Variable, SubstitutionType>& substitutions) const;
std::size_t totalDegree() const;
std::size_t degree(Variable::Arg var) const;
size_t nrTerms() const;
std::string toString(bool infix=true, bool friendlyVarNames=true) const;

size_t size() const;

%extend{
	bool equals(const MultivariatePolynomial<Coeff>& other) {
		return *$self == other;
	}

	bool notEquals(const MultivariatePolynomial<Coeff>& other) {
		return *$self != other;
	}

	std::vector<carl::Variable> gatherVariables() const {
	   std::set<carl::Variable> asSet = $self->gatherVariables();
   	   return std::vector<carl::Variable>(asSet.begin(),asSet.end());
        }


	Polynomial add(const Polynomial& rhs) {
		return *$self+rhs;
	}

	Polynomial add(const Term& rhs) {
		return *$self+rhs;
	}

	Polynomial add(const Monomial::Arg& rhs) {
		return *$self+rhs;
	}


	Polynomial add(carl::Variable::Arg rhs) {
		return *$self+rhs;
	}

	Polynomial add(const Rational& rhs) {
		return *$self+rhs;
	}



	Polynomial sub(const Polynomial& rhs) {
		return *$self-rhs;
	}

	Polynomial sub(const Term& rhs) {
		return *$self-rhs;
	}

	Polynomial sub(const Monomial::Arg& rhs) {
		return *$self-rhs;
	}


	Polynomial sub(carl::Variable::Arg rhs) {
		return *$self-rhs;
	}

	Polynomial sub(const Rational& rhs) {
		return *$self-rhs;
	}


	Polynomial mul(const Polynomial& rhs) {
		return *$self*rhs;
	}

	Polynomial mul(const Term& rhs) {
		return *$self*rhs;
	}

	Polynomial mul(const Monomial::Arg& rhs) {
		return *$self*rhs;
	}


	Polynomial mul(carl::Variable::Arg rhs) {
		return *$self*rhs;
	}

	Polynomial mul(const Rational& rhs) {
		return *$self*rhs;
	}


	RationalFunction div(const RationalFunction& rhs) {
		return RationalFunction(*$self) / rhs;
	}

	RationalFunction div(const Polynomial& rhs) {
		return RationalFunction(*$self) / rhs;
	}

	RationalFunction div(const Term& rhs) {
		return RationalFunction(*$self) / rhs;
	}

	RationalFunction div(const Monomial::Arg& rhs) {
		return RationalFunction(*$self) / rhs;
	}


	RationalFunction div(carl::Variable::Arg rhs) {
		return RationalFunction(*$self) / rhs;
	}

	Polynomial div(const Rational& rhs) {
		return *$self / rhs;
	} 

	Polynomial pow(uint exp) {
	    return $self->pow(exp);
	}

	Polynomial neg() {
		return *$self*Rational(-1);
	}


	Term getItem(std::size_t index) {
	    return *($self->begin()+index); 
	}



}



};
}

%include "std_map.i"

namespace std {
 %template(VarRationalMap) map<carl::Variable,Rational>;
}


%template(evaluate) carl::MultivariatePolynomial::evaluate<Rational>;
%template(Polynomial) carl::MultivariatePolynomial<Rational>;

