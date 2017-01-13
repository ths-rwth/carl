


%module MonomialT
%{
#include <carl/core/Monomial.h>
#include <carl/core/Term.h>
#include <carl/core/MultivariatePolynomial.h>
#include <carl/core/RationalFunction.h>
#include "gmp.h"
#include "gmpxx.h"
typedef mpq_class Rational;


typedef unsigned int uint;
typedef std::pair<carl::Variable,uint> VarIntPair;



typedef carl::Term<Rational> Term;

typedef carl::MultivariatePolynomial<Rational> Polynomial;
typedef carl::RationalFunction<Polynomial> RationalFunction;


%}

%include "std_string.i"
//TODO: for 32 bit support this has to be changed..
//%import <stddef> //for size_t maybe?
typedef long unsigned int size_t;  //this should be okay for 64 bits at least

typedef unsigned int uint;


%include "std_pair.i"
%include "std_vector.i"



%import "variable.i"
%import "term.i"
%import "polynomial.i"
%import "rationalfunction.i"




%include "std_shared_ptr.i"
%shared_ptr(carl::Monomial);

typedef std::pair<carl::Variable,uint> VarIntPair;
namespace std {
 %template(VarIntPair) pair<carl::Variable,uint>;
 %template(VarIntPairVector) vector<VarIntPair>;
}

typedef carl::Term<Rational> Term;

typedef carl::MultivariatePolynomial<Rational> Polynomial;
typedef carl::RationalFunction<Polynomial> RationalFunction;







%nodefaultctor carl::Monomial;
namespace carl {

typedef unsigned int exponent;





class Monomial {
public:
typedef std::shared_ptr<const carl::Monomial> Arg;
typedef std::vector<VarIntPair> Content;

/*
explicit Monomial(Variable::Arg v, carl::exponent e = 1) :
	mExponents(1, std::make_pair(v,e)),
	mTotalDegree(e)
{
}
*/

 carl::exponent tdeg() const;

const carl::Monomial::Content& exponents() const;


std::string toString(bool infix = true, bool friendlyVarNames = true) const;

size_t nrVariables() const;



%extend {


	static carl::Monomial::Arg createMonomial(Variable::Arg v, carl::exponent e = 1) {
		return carl::MonomialPool::getInstance().create(v, e);	
	}

	Polynomial add(const Polynomial& rhs) {
	    carl::Monomial::Content exp($self->exponents());	
	    const std::shared_ptr<const carl::Monomial> ptr = std::make_shared<const carl::Monomial>(std::move(exp));
	    return ptr+rhs;
	}

	Polynomial add(const Term& rhs) {
	    carl::Monomial::Content exp($self->exponents());	
	    const std::shared_ptr<const carl::Monomial> ptr = std::make_shared<const carl::Monomial>(std::move(exp));
	    return ptr+rhs;
	}

	Polynomial add(const Monomial::Arg& rhs) {
	    carl::Monomial::Content exp($self->exponents());	
	    const std::shared_ptr<const carl::Monomial> ptr = std::make_shared<const carl::Monomial>(std::move(exp));
	    return carl::operator+<Rational,carl::GrLexOrdering,carl::StdMultivariatePolynomialPolicies<>>(ptr,rhs);
	} 

	Polynomial add(carl::Variable::Arg rhs) {
	    carl::Monomial::Content exp($self->exponents());	
	    const std::shared_ptr<const carl::Monomial> ptr = std::make_shared<const carl::Monomial>(std::move(exp));
	    return carl::operator+<Rational,carl::GrLexOrdering,carl::StdMultivariatePolynomialPolicies<>>(ptr,rhs);
	} 

	Polynomial add(const Rational& rhs) {
	    carl::Monomial::Content exp($self->exponents());	
	    const std::shared_ptr<const carl::Monomial> ptr = std::make_shared<const carl::Monomial>(std::move(exp));
	    return ptr+rhs;
	}




	Polynomial sub(const Polynomial& rhs) {
	    carl::Monomial::Content exp($self->exponents());	
	    const std::shared_ptr<const carl::Monomial> ptr = std::make_shared<const carl::Monomial>(std::move(exp));
	    return ptr-rhs;
	}

	Polynomial sub(const Term& rhs) {
	    carl::Monomial::Content exp($self->exponents());	
	    const std::shared_ptr<const carl::Monomial> ptr = std::make_shared<const carl::Monomial>(std::move(exp));
	    return ptr-rhs;
	}

	Polynomial sub(const Monomial::Arg& rhs) {
	    carl::Monomial::Content exp($self->exponents());	
	    const std::shared_ptr<const carl::Monomial> ptr = std::make_shared<const carl::Monomial>(std::move(exp));
	    return carl::operator-<Rational,carl::GrLexOrdering,carl::StdMultivariatePolynomialPolicies<>>(ptr,rhs);
	} 

	Polynomial sub(carl::Variable::Arg rhs) {
	    carl::Monomial::Content exp($self->exponents());	
	    const std::shared_ptr<const carl::Monomial> ptr = std::make_shared<const carl::Monomial>(std::move(exp));
	    return carl::operator-<Rational,carl::GrLexOrdering,carl::StdMultivariatePolynomialPolicies<>>(ptr,rhs);
	} 

	Polynomial sub(const Rational& rhs) {
	    carl::Monomial::Content exp($self->exponents());	
	    const std::shared_ptr<const carl::Monomial> ptr = std::make_shared<const carl::Monomial>(std::move(exp));
	    return ptr-rhs;
	}



	Polynomial mul(const Polynomial& rhs) {
	    carl::Monomial::Content exp($self->exponents());	
	    const std::shared_ptr<const carl::Monomial> ptr = std::make_shared<const carl::Monomial>(std::move(exp));
	    return rhs*ptr;
	}

	Term mul(const Term& rhs) {
	    carl::Monomial::Content exp($self->exponents());	
	    const std::shared_ptr<const carl::Monomial> ptr = std::make_shared<const carl::Monomial>(std::move(exp));
	    return ptr*rhs;
	}

	Polynomial mul(const Monomial::Arg& rhs) {
	    carl::Monomial::Content exp($self->exponents());	
	    const std::shared_ptr<const carl::Monomial> ptr = std::make_shared<const carl::Monomial>(std::move(exp));
	    //const std::shared_ptr<const carl::Monomial> ptr2(rhs);
	    return carl::operator*(ptr,Polynomial(rhs));
	} 

	Polynomial mul(carl::Variable::Arg rhs) {
	    carl::Monomial::Content exp($self->exponents());	
	    const std::shared_ptr<const carl::Monomial> ptr = std::make_shared<const carl::Monomial>(std::move(exp));
	    return carl::operator*(ptr,Polynomial(rhs));
	}  

	Term mul(const Rational& rhs) {
	    carl::Monomial::Content exp($self->exponents());	
	    const std::shared_ptr<const carl::Monomial> ptr = std::make_shared<const carl::Monomial>(std::move(exp));
	    return ptr*rhs;
	}


	RationalFunction div(const RationalFunction& rhs) {
	    carl::Monomial::Content exp($self->exponents());	
	    const std::shared_ptr<const carl::Monomial> ptr = std::make_shared<const carl::Monomial>(std::move(exp));
		return RationalFunction(Polynomial(ptr)) / rhs;
	}

	RationalFunction div(const Polynomial& rhs) {
	    carl::Monomial::Content exp($self->exponents());	
	    const std::shared_ptr<const carl::Monomial> ptr = std::make_shared<const carl::Monomial>(std::move(exp));
		return RationalFunction(Polynomial(ptr)) / rhs;
	}

	RationalFunction div(const Term& rhs) {
	    carl::Monomial::Content exp($self->exponents());	
	    const std::shared_ptr<const carl::Monomial> ptr = std::make_shared<const carl::Monomial>(std::move(exp));
		return RationalFunction(Polynomial(ptr)) / rhs;
	}

	RationalFunction div(const Monomial::Arg& rhs) {
	    carl::Monomial::Content exp($self->exponents());	
	    const std::shared_ptr<const carl::Monomial> ptr = std::make_shared<const carl::Monomial>(std::move(exp));
		return RationalFunction(Polynomial(ptr)) / rhs;
	}

	RationalFunction div(carl::Variable::Arg rhs) {
	        carl::Monomial::Content exp($self->exponents());	
	        const std::shared_ptr<const carl::Monomial> ptr = std::make_shared<const carl::Monomial>(std::move(exp));
		return RationalFunction(Polynomial(ptr)) / rhs;
	}

	Term div(const Rational& rhs) {
	    carl::Monomial::Content exp($self->exponents());	
	    const std::shared_ptr<const carl::Monomial> ptr = std::make_shared<const carl::Monomial>(std::move(exp));
		return Term(ptr) / rhs;
	}

	Term neg() {
		carl::Monomial::Content exp($self->exponents());	
	        const std::shared_ptr<const carl::Monomial> ptr = std::make_shared<const carl::Monomial>(std::move(exp));
		return ptr*Rational(-1);
	}

	carl::Monomial::Arg pow(uint exp) {
		return $self->pow(exp);
	} 



	VarIntPair getItem(std::size_t index) {
		return *($self->begin()+index);
	}

}


};

}
