/**
 * @file parser/Common.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <tuple>
#include <vector>

#define BOOST_SPIRIT_USE_PHOENIX_V3
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

#include <carl-formula/formula/Formula.h>
#include <carl/core/MonomialPool.h>
#include <carl/core/MultivariatePolynomial.h>
#include <carl-extpolys/FactorizedPolynomial.h>
#include <carl-extpolys/RationalFunction.h>
#include <carl/core/Variable.h>
#include <carl/core/VariablePool.h>
#include <carl-common/util/streamingOperators.h>

namespace spirit = boost::spirit;
namespace qi = boost::spirit::qi;
namespace px = boost::phoenix;

namespace carl {
namespace parser {

using carl::operator<<;

using Iterator = std::string::const_iterator;
using Skipper = boost::spirit::qi::space_type;

template<typename Pol>
using RatFun = RationalFunction<Pol>;

template<typename Coeff>
struct RationalPolicies : qi::ureal_policies<Coeff> {
    template <typename It, typename Attr>
    static bool parse_nan(It&, It const&, Attr&) { return false; }
    template <typename It, typename Attr>
    static bool parse_inf(It&, It const&, Attr&) { return false; }
};

}
}
