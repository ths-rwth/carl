
%module VariableT
%{
#include <carl/core/Variable.h>
#include <carl/core/Monomial.h>
#include <carl/core/Term.h>
#include <carl/core/MultivariatePolynomial.h>
#include <carl/core/RationalFunction.h>


typedef mpq_class Rational;


typedef carl::Term<Rational> Term;

typedef carl::MultivariatePolynomial<Rational> Polynomial;
typedef carl::RationalFunction<Polynomial> RationalFunction;

%}



%include "std_string.i"

%import "monomial.i"
%import "term.i"
%import "polynomial.i"
%import "rationalfunction.i"



//TODO: for 32 bit support this has to be changed..
//%import <stddef> //for size_t maybe?
typedef long unsigned int size_t;  //this should be okay for 64 bits at least


typedef mpq_class Rational;


typedef carl::Term<Rational> Term;

typedef carl::MultivariatePolynomial<Rational> Polynomial;
//typedef carl::RationalFunction<Polynomial> RationalFunction;


namespace carl {


%include "enums.swg"
%javaconst(1);
//maybe also define the toString for this enum, if needed
//enum VariableType { VT_BOOL = 0, VT_REAL = 1, VT_INT = 2, VT_UNINTERPRETED = 3, VT_BITVECTOR = 4, MIN_TYPE = VT_BOOL, MAX_TYPE = VT_BITVECTOR};

enum class VariableType { VT_BOOL = 0, VT_REAL = 1, VT_INT = 2, VT_UNINTERPRETED = 3, VT_BITVECTOR = 4, MIN_TYPE = VT_BOOL, MAX_TYPE = VT_BITVECTOR, TYPE_SIZE = MAX_TYPE - MIN_TYPE + 1 };



class Variable {
public:
#ifdef VARIABLE_PASS_BY_VALUE
	/// Argument type for variables being function arguments.
	typedef Variable Arg;
#else
	/// Argument type for variables being function arguments.
	typedef const Variable& Arg;
#endif
	%extend {
		 Variable(std::string name, carl::VariableType type) {
		   //try and generate new variable from pool
		     carl::Variable newVar;
	 	     auto& pool = carl::VariablePool::getInstance();
    		     carl::Variable res = pool.findVariableWithName(name);
		     if (res != carl::Variable::NO_VARIABLE) {
			 newVar = res;
		     } else {
		     	 newVar = freshVariable(name, type);
		     }	
		     return new carl::Variable(newVar);		
		}

		Variable(carl::VariableType type)  {
	       		 carl::Variable newVar = freshVariable(type);
			 return new carl::Variable(newVar);
	    	}

		
		std::string toString() {
		     std::stringstream ss;
		     ss << *$self;
		     return ss.str();	
		}

	}
	
std::size_t getId() const; 


VariableType getType() const;


std::string getName() const;


std::size_t getRank();

%extend {
	bool equals(carl::Variable::Arg other) {
		return *$self == other;			
	}

	bool notEquals(carl::Variable::Arg other) {
		return *$self != other;			
	}
	bool less(carl::Variable::Arg other) {
		return *$self < other;			
	}
	bool lessEquals(carl::Variable::Arg other) {
		return *$self <= other;			
	}
	bool greater(carl::Variable::Arg other) {
		return *$self > other;			
	}
	bool greaterEquals(carl::Variable::Arg other) {
		return *$self >= other;			
	}


	Polynomial add(const Polynomial& rhs) {
		return *($self)+rhs;
	}

	Polynomial add(const Term& rhs) {
		return *($self)+rhs;
	}


	Polynomial add(const carl::Monomial::Arg& rhs) {
		return carl::operator+<Rational,carl::GrLexOrdering,carl::StdMultivariatePolynomialPolicies<>>(*($self),rhs);
	} 

	Polynomial add(carl::Variable::Arg rhs) {
		return carl::operator+<Rational,carl::GrLexOrdering,carl::StdMultivariatePolynomialPolicies<>>(*($self),rhs);
	} 

	Polynomial add(const Rational& rhs) {
		return *($self)+Term(rhs);
	}



	Polynomial sub(const Polynomial& rhs) {
		return *($self)-rhs;
	}

	Polynomial sub(const Term& rhs) {
		return *($self)-rhs;
	}

	Polynomial sub(const carl::Monomial::Arg& rhs) {
		return carl::operator-<Rational,carl::GrLexOrdering,carl::StdMultivariatePolynomialPolicies<>>(*($self),rhs);
	} 


	Polynomial sub(carl::Variable::Arg rhs) {
		return carl::operator-<Rational,carl::GrLexOrdering,carl::StdMultivariatePolynomialPolicies<>>(*($self),rhs);
	}  

	Polynomial sub(const Rational& rhs) {
		return *($self)-Term(rhs);
	}




	Polynomial mul(const Polynomial& rhs) {
		return *($self)*rhs;
	}

	Term mul(const Term& rhs) {
		return *($self)*rhs;
	}

	Polynomial mul(const Monomial::Arg& rhs) {
		//const std::shared_ptr<const carl::Monomial> ptr(rhs);
		return carl::operator*(*($self),Polynomial(rhs));
	} 

	Polynomial mul(carl::Variable::Arg rhs) {
		return carl::operator*(*($self),Polynomial(rhs));
	} 

	Term mul(const Rational& rhs) {
		return *($self)*Term(rhs);
	}



	RationalFunction div(const carl::RationalFunction<Polynomial>& rhs) {
		return RationalFunction(*($self)) / rhs;
	}

	RationalFunction div(const Polynomial& rhs) {
		return RationalFunction(*($self)) / rhs;
	}

	RationalFunction div(const Term& rhs) {
		return RationalFunction(*($self)) / rhs;
	}



	RationalFunction div(const carl::Monomial::Arg& rhs) {
		return RationalFunction(*($self)) / rhs;
	} 

	RationalFunction div(carl::Variable::Arg rhs) {
		return RationalFunction(*($self)) / rhs;
	}

	Polynomial div(const Rational& rhs) {
		if (carl::isZero(rhs)) throw std::runtime_error("Div by zero");
		return Polynomial(*($self)) / rhs;
	}

	Term neg() {
		return *$self*Rational(-1);
	}



	carl::Monomial::Arg pow(uint exp) {
		carl::Monomial m(*$self);
		return m.pow(exp);
	}  

}	
};

}

