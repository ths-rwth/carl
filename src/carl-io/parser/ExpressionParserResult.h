/*
 * ExpressionParserResult.h
 * Declares the result type of the ExpressionParser, which is used by
 * at least the Python modules.
 *
 *  Created on: 14 Apr 2016
 *      Author: hbruintjes
 */

#pragma once

#include <string>
#include <boost/variant/variant.hpp>

#include <carl/core/Variable.h>
#include <carl/poly/umvpoly/Monomial.h>
#include <carl/poly/umvpoly/Term.h>
#include <carl-extpolys/RationalFunction.h>
#include <carl-formula/formula/Formula.h>

namespace carl::io {
namespace parser {

template<typename Pol>
using ExpressionType = boost::variant< typename Pol::CoeffType, carl::Variable, carl::Monomial::Arg,
		carl::Term<typename Pol::CoeffType>, Pol, RationalFunction<Pol>,
		carl::Formula<Pol> >;

}
}
