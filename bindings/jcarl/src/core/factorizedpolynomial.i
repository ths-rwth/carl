



%module ModFactorizedPolynomial
%{
#include "MultivariatePolynomial.h"
#include "FactorizedPolynomial.h"
#include "gmp.h"
#include "gmpxx.h"




typedef mpq_class Rational;


typedef carl::Term<Rational> Term;
typedef carl::MultivariatePolynomial<Rational> Polynomial;
typedef carl::FactorizedPolynomial<Polynomial> FactorizedPolynomial;
typedef carl::PolynomialFactorizationPair<Polynomial> FactorizationPair;

typedef unsigned int uint;
typedef std::pair<carl::Variable,uint> VarIntPair;
%}

%include "std_string.i"
%import "variable.i"
%import "polynomial.i"

typedef mpq_class Rational;
typedef carl::MultivariatePolynomial<Rational> Polynomial;

namespace carl {

template<typename P>
class FactorizedPolynomial {
public:
typedef typename P::CoeffType CoeffType;


explicit FactorizedPolynomial( const CoeffType& );
//TODO: wrap the CACHE properly!
explicit FactorizedPolynomial( const P& _polynomial, const std::shared_ptr<carl::FactorizedPolynomial::CACHE>&, bool = false );
CoeffType constantPart() const;
template<typename SubstitutionType = CoeffType>
SubstitutionType evaluate(const std::map<carl::Variable, SubstitutionType>& substitutions) const;
        
//std::set<carl::Variable> gatherVariables() const;

std::string toString( bool _infix = true, bool _friendlyVarNames = true ) const;

%extend {
std::vector<carl::Variable> gatherVariables() const {
   std::set<carl::Variable> asSet = $self->gatherVariables();
   return std::vector<carl::Variable>(asSet.begin(),asSet.end());
}

}


//TODO: operators

};

%template(evaluate) carl::FactorizedPolynomial::evaluate<Rational>;

}

%template(FactorizedPolynomialPoly) carl::FactorizedPolynomial<Polynomial>;
