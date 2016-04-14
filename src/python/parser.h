/*
 * parser.h
 *
 *  Created on: 14 Apr 2016
 *      Author: hbruintjes
 */

#pragma once

#include <string>
#include <boost/variant/variant.hpp>

typedef boost::variant< Rational, carl::Variable, carl::Monomial::Arg, carl::Term<Rational>, Polynomial, RationalFunction > expr_type;

expr_type parse(const std::string& input);
