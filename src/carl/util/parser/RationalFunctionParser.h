/**
 * @file RationalFunctionParser.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include "Common.h"
#include "RationalFunctionParser.h"
#include "PolynomialParser.h"

namespace carl {
namespace parser {

template<typename Pol>
struct RationalFunctionParser: public qi::grammar<Iterator, RatFun<Pol>(), Skipper> {
	RationalFunctionParser(): RationalFunctionParser<Pol>::base_type(main, "rationalfunction") {
		main = (polynomial >> -("/" >> polynomial))[qi::_val = px::bind(&RationalFunctionParser<Pol>::construct, px::ref(*this), qi::_1, qi::_2)];
	}
	void addVariable(Variable::Arg v) {
		polynomial.addVariable(v);
    }
private:
	RatFun<Pol> construct(const Pol& lhs, const boost::optional<Pol>& rhs) {
		if (rhs) return RatFun<Pol>(lhs, *rhs);
		return RatFun<Pol>(lhs);
	}
	PolynomialParser<Pol> polynomial;
	qi::rule<Iterator, RatFun<Pol>(), Skipper> main;
};

}
}
