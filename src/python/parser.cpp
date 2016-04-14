
#include "carl/core/MultivariatePolynomial.h"
#include "carl/core/RationalFunction.h"
#include "carl/util/parser/Parser.h"

#include <gmpxx.h>
typedef mpq_class Rational;

//typedef double Rational;
typedef carl::MultivariatePolynomial<Rational> Polynomial;
typedef carl::RationalFunction<Polynomial> RationalFunction;

Polynomial parsePolynomial(const std::string& text) {
	carl::parser::Parser<Rational> p;
	return p.polynomial(text);
}

RationalFunction parseRationalFunction(const std::string& text) {
	carl::parser::Parser<Rational> p;
	return p.rationalFunction(text);
}
