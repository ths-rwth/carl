


%module ModMonomial
%{
#include "Monomial.h"
#include "gmp.h"
#include "gmpxx.h"
typedef mpq_class Rational;


typedef unsigned int uint;
typedef std::pair<carl::Variable,uint> VarIntPair;


%}

%include "std_string.i"
//TODO: for 32 bit support this has to be changed..
//%import <stddef> //for size_t maybe?
typedef long unsigned int size_t;  //this should be okay for 64 bits at least

typedef unsigned int uint;

%import "variable.i"

typedef std::pair<carl::Variable,uint> VarIntPair;
%include "std_pair.i"
%include "std_vector.i"
%include "std_shared_ptr.i"
%shared_ptr(carl::Monomial);






namespace std {
 %template(VarIntPair) pair<carl::Variable,uint>;
 %template(VarIntPairVector) vector<VarIntPair>;
}




namespace carl {

typedef unsigned int exponent;





class Monomial {
public:
typedef std::shared_ptr<const carl::Monomial> Arg;
typedef std::vector<VarIntPair> Content;
/*%extend {
 Monomial(carl::Variable v, carl::exponent e) {
    auto m = carl::MonomialPool::getInstance().create(v, e);
    return m;
 }
} */

explicit Monomial(Variable::Arg v, carl::exponent e = 1) :
	mExponents(1, std::make_pair(v,e)),
	mTotalDegree(e)
{
}


 carl::exponent tdeg() const;

const carl::Monomial::Content& exponents() const;


std::string toString(bool infix = true, bool friendlyVarNames = true) const;

size_t nrVariables() const;


//TODO: operators, getItem, iterator


};

}
