

%module ModPolynomial
%{
#include "MultivariatePolynomial.h"
#include "gmp.h"
#include "gmpxx.h"

typedef mpq_class Rational;


typedef carl::Term<Rational> Term;
typedef carl::MultivariatePolynomial<Rational> Polynomial;


typedef unsigned int uint;
typedef std::pair<carl::Variable,uint> VarIntPair;
%}


%include "std_string.i"
//TODO: for 32 bit support this has to be changed..
//%import <stddef> //for size_t maybe?
typedef long unsigned int size_t;  //this should be okay for 64 bits at least

%import "rational.i"
%import "variable.i"
%import "term.i"
%import "monomial.i"


%include "std_vector.i"
typedef mpq_class Rational;

namespace std {
 %template(VarVector) vector<carl::Variable>;
}

namespace carl {

template<typename Coeff>
class MultivariatePolynomial
{
public:
typedef Coeff CoeffType;
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



}

//TODO: getItem, iterator, operators

};
}

%include "std_map.i"

namespace std {
 %template(VarRationalMap) map<carl::Variable,Rational>;
}


%template(evaluate) carl::MultivariatePolynomial::evaluate<Rational>;
%template(PolynomialRational) carl::MultivariatePolynomial<Rational>;

