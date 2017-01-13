
%module TermRationalT 
%{
#include <carl/core/Term.h>
#include <carl/core/MultivariatePolynomial.h>
#include <carl/core/RationalFunction.h>
#include "gmp.h"
#include "gmpxx.h"

typedef mpq_class Rational;


typedef carl::Term<Rational> Term;

typedef carl::MultivariatePolynomial<Rational> Polynomial;
typedef carl::RationalFunction<Polynomial> RationalFunction;

%}


%include "std_string.i"
%import "monomial.i"
%import "rational.i"
%import "polynomial.i"
%import "rationalfunction.i"
typedef mpq_class Rational;
typedef carl::Term<Rational> Term;
typedef carl::MultivariatePolynomial<Rational> Polynomial;
typedef carl::RationalFunction<Polynomial> RationalFunction;


namespace carl {


template<typename Coefficient>
class Term {
 public:
  Term(const Coefficient& c, const carl::Monomial::Arg& m);
  
  Coefficient& coeff();

  inline const Monomial::Arg& monomial() const;
  //template<typename C = Coefficient, DisableIf<is_interval<C>> = dummy>
  //std::string toString(bool infix=true, bool friendlyVarNames=true) const;

  carl::Term<Coefficient> pow(uint exp) const;
  

  %extend {

   std::string toString() const {
	return $self->toString();
   }
	
   Polynomial add(const carl::Term<Coefficient>& rhs) {
	return *($self)+rhs;
   } 	

   Polynomial add(const Polynomial& rhs) {
 	return *($self)+rhs;
   } 

   Polynomial add(const Monomial::Arg& rhs) {
 	return *($self)+rhs;
   } 

   Polynomial add(carl::Variable::Arg rhs) {
 	return *($self)+rhs;
   } 

   Polynomial add(const Coefficient& rhs) {
 	return *($self)+rhs;
   } 


   Polynomial sub(const carl::Term<Coefficient>& rhs) {
	return *($self)-rhs;
   } 	

   Polynomial sub(const Polynomial& rhs) {
 	return *($self)-rhs;
   } 

   Polynomial sub(const Monomial::Arg& rhs) {
 	return *($self)-rhs;
   } 

   Polynomial sub(carl::Variable::Arg rhs) {
 	return *($self)-rhs;
   } 

   Polynomial sub(const Coefficient& rhs) {
 	return *($self)-rhs;
   } 


   carl::Term<Coefficient> mul(const carl::Term<Coefficient>& rhs) {
	return *($self)*rhs;
   } 	

   Polynomial mul(const Polynomial& rhs) {
 	return *($self)*rhs;
   } 

   carl::Term<Coefficient> mul(const Monomial::Arg& rhs) {
 	return *($self)*rhs;
   } 

   carl::Term<Coefficient> mul(carl::Variable::Arg rhs) {
 	return *($self)*rhs;
   } 

   carl::Term<Coefficient> mul(const Coefficient& rhs) {
 	return *($self)*rhs;
   } 


   RationalFunction div(const RationalFunction& rhs) {
	return RationalFunction(Polynomial(*($self))) / rhs;
   }

   RationalFunction div(const Polynomial& rhs) {
	return RationalFunction(Polynomial(*($self))) / rhs;
   }

   RationalFunction div(const carl::Term<Coefficient>& rhs) {
	return RationalFunction(Polynomial(*($self))) / rhs;
   }

   RationalFunction div(const Monomial::Arg& rhs) {
	return RationalFunction(Polynomial(*($self))) / rhs;
   }

   RationalFunction div(carl::Variable::Arg rhs) {
	return RationalFunction(Polynomial(*($self))) / rhs;
   }

	carl::Term<Coefficient> div(const Rational& rhs) {
		return *$self / rhs;
	}



   carl::Term<Coefficient> neg() {
	return *$self*Coefficient(-1);
   }



  }
};
}


%template(TermRational) carl::Term<Rational>;
