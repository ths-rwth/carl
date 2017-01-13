

%module ModRationalFunction
%{
#include <carl/core/RationalFunction.h>
#include <carl/core/MultivariatePolynomial.h>
#include <carl/core/FactorizedPolynomial.h>



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
%import "monomial.i"
%import "term.i"
%import "polynomial.i"
%import "factorizedpolynomial.i"

typedef carl::Term<Rational> Term;
typedef carl::MultivariatePolynomial<Rational> Polynomial;
typedef carl::FactorizedPolynomial<Polynomial> FactorizedPolynomial;

typedef carl::RationalFunction<FactorizedPolynomial> FactorizedRationalFunction;


%rename(RationalFunction) carl::RationalFunction<Polynomial>;
%rename(FactorizedRationalFunction) carl::RationalFunction<FactorizedPolynomial>;
namespace carl {



class RationalFunction<Polynomial>
{
public:
typedef typename Rational CoeffType;
        explicit RationalFunction(const Polynomial& nom, const Polynomial& denom):
            mPolynomialQuotient(nullptr),
            mNumberQuotient(),
            mIsSimplified(false)
        {
        }

        CoeffType evaluate(const std::map<Variable, CoeffType>& substitutions) const;

        inline Polynomial nominator() const;

        inline Polynomial denominator() const;


%extend {
	std::string toString() {
	     std::stringstream ss;
	     ss << *$self;
	     return ss.str();	
	}
	
	Polynomial numerator() const {
	    return $self->nominator();
	}

	std::vector<carl::Variable> gatherVariables() const {
	   std::set<carl::Variable> asSet = $self->gatherVariables();
   	   return std::vector<carl::Variable>(asSet.begin(),asSet.end());
        }
	

	bool equals(const carl::RationalFunction<Polynomial>& other) {
		return *$self == other;
	}
	bool notEquals(const carl::RationalFunction<Polynomial>& other) {
		return *$self != other;
	}




	carl::RationalFunction<Polynomial> add(const Polynomial& rhs) {
		return *($self)+rhs;
	}
	

	carl::RationalFunction<Polynomial> add(const Term& rhs) {
		return *($self)+rhs;
	}

	carl::RationalFunction<Polynomial> add(const Monomial::Arg& rhs) {
		return *($self)+rhs;
	}

	carl::RationalFunction<Polynomial> add(carl::Variable::Arg rhs) {
		return *($self)+rhs;
	}

	carl::RationalFunction<Polynomial> add(Rational rhs) {
		return *($self)+rhs;
	}


	carl::RationalFunction<Polynomial> sub(const Polynomial& rhs) {
		return *($self)-rhs;
	}

	carl::RationalFunction<Polynomial> sub(const Term& rhs) {
		return *($self)-rhs;
	}

	carl::RationalFunction<Polynomial> sub(const Monomial::Arg& rhs) {
		return *($self)-rhs;
	}

	carl::RationalFunction<Polynomial> sub(carl::Variable::Arg rhs) {
		return *($self)-rhs;
	}

	carl::RationalFunction<Polynomial> sub(Rational rhs) {
		return *($self)-rhs;
	}



	carl::RationalFunction<Polynomial> mul(const Polynomial& rhs) {
		return *($self)*rhs;
	}

	carl::RationalFunction<Polynomial>mul(const Term& rhs) {
		return *($self)*rhs;
	}

	carl::RationalFunction<Polynomial> mul(const Monomial::Arg& rhs) {
		return *($self)*rhs;
	}

	carl::RationalFunction<Polynomial> mul(carl::Variable::Arg rhs) {
		return *($self)*rhs;
	}

	carl::RationalFunction<Polynomial> mul(Rational rhs) {
		return *($self)*rhs;
	}

	carl::RationalFunction<Polynomial> div(const Polynomial& rhs) {
		return *($self)/rhs;
	}

	carl::RationalFunction<Polynomial> div(const Term& rhs) {
		return *($self)/rhs;
	}

	carl::RationalFunction<Polynomial> div(const Monomial::Arg& rhs) {
		return *($self)/rhs;
	}

	carl::RationalFunction<Polynomial> div(carl::Variable::Arg rhs) {
		return *($self)/rhs;
	}

	carl::RationalFunction<Polynomial> div(Rational rhs) {
		return *($self)/rhs;
	}

	carl::RationalFunction<Polynomial> div(const RationalFunction& rhs) {
		return *($self)/rhs;
	}


        carl::RationalFunction<Polynomial> pow(uint exp) {
	    return carl::pow(*($self),exp);
	}

	carl::RationalFunction<Polynomial> neg() {
		return *$self*Rational(-1);
	}
}

};



class RationalFunction<FactorizedPolynomial> {
public:
typedef typename Rational CoeffType;
        explicit RationalFunction(const FactorizedPolynomial& nom, const FactorizedPolynomial& denom):
            mPolynomialQuotient(nullptr),
            mNumberQuotient(),
            mIsSimplified(false)
        {
        }

        CoeffType evaluate(const std::map<Variable, CoeffType>& substitutions) const;

        inline FactorizedPolynomial nominator() const;

        inline FactorizedPolynomial denominator() const;


%extend {
	std::string toString() {
	     std::stringstream ss;
	     ss << *$self;
	     return ss.str();	
	}
	
	FactorizedPolynomial numerator() const {
	    return $self->nominator();
	}

	std::vector<carl::Variable> gatherVariables() const {
	   std::set<carl::Variable> asSet = $self->gatherVariables();
   	   return std::vector<carl::Variable>(asSet.begin(),asSet.end());
        }

	bool equals(const carl::RationalFunction<FactorizedPolynomial>& other) {
		return *$self == other;
	}
	
}
};

}

typedef carl::RationalFunction<Polynomial> RationalFunction;


/*
%template(RationalFunctionPoly) carl::RationalFunction<Polynomial>;
%template(FactorizedRationalFunction) carl::RationalFunction<carl::FactorizedPolynomial<Polynomial>,false>; */

