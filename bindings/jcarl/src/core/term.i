
%module ModTerm 
%{
#include "Term.h"
#include "gmp.h"
#include "gmpxx.h"

typedef mpq_class Rational;


typedef carl::Term<Rational> Term;

%}


%include "std_string.i"
%import "monomial.i"
%import "rational.i"

namespace carl {

template<typename Coefficient>
class Term {
 public:
  Term(const Coefficient& c, const carl::Monomial::Arg& m);
  
  Coefficient& coeff();

  const carl::Monomial& monomial() const;
  std::string toString(bool infix=true, bool friendlyVarNames=true) const;


//TODO: operators

};

}

%template(TermRational) carl::Term<Rational>;
