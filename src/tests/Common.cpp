#include "Common.h"

using carl::GaloisField;
using carl::GFNumber;
using carl::Term;
using carl::MultivariatePolynomial;
using carl::MultivariateGCD;

template class carl::GaloisField<Integer>;
template class carl::GFNumber<Integer>;
template class carl::Term<Rational>;
template class carl::MultivariatePolynomial<Rational>;
template class carl::MultivariateGCD<carl::PrimitiveEuclidean, Rational>;
