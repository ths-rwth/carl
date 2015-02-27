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

template<typename Coeff>
struct RationalFunctionParser: public qi::grammar<Iterator, RatFun<Coeff>(), Skipper> {
	RationalFunctionParser(): RationalFunctionParser<Coeff>::base_type(main, "rationalfunction") {
		main = (polynomial >> -("/" > polynomial))[qi::_val = px::bind(&RationalFunctionParser<Coeff>::construct, px::ref(*this), qi::_1, qi::_2)];
	}
private:
	RatFun<Coeff> construct(const Poly<Coeff>& lhs, const boost::optional<Poly<Coeff>>& rhs) {
		if (!rhs) return RatFun<Coeff>(lhs);
		return RatFun<Coeff>(lhs, rhs.get());
	}
	PolynomialParser<Coeff> polynomial;
	qi::rule<Iterator, RatFun<Coeff>(), Skipper> main;
};

}
}
