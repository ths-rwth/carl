/**
 * @file Parser.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <sstream>

#include "../../core/logging.h"
#include "Common.h"
#include "PolynomialParser.h"
#include "RationalFunctionParser.h"

namespace carl {
namespace parser {

template<typename Coeff>
class Parser {
private:
	Skipper skipper;
	PolynomialParser<Coeff> polynomialParser;
	RationalFunctionParser<Coeff> ratfunParser;
	
	template<typename Result, typename Parser>
	bool parse(const std::string& s, const Parser& parser, Result& res) {
		auto begin = s.begin();
		return qi::phrase_parse(begin, s.end(), parser, skipper, res);
	}
public:
	
	Poly<Coeff> polynomial(const std::string& s) {
		Poly<Coeff> res;
		if (!parse(s, polynomialParser, res)) {
			CARL_LOG_ERROR("carl.parser", "Parsing \"" << s << "\" to a polynomial failed.");
		}
		return res;
	}
	
	RatFun<Coeff> rationalFunction(const std::string& s) {
		RatFun<Coeff> res;
		if (!parse(s, ratfunParser, res)) {
			CARL_LOG_ERROR("carl.parser", "Parsing \"" << s << "\" to a rational function failed.");
		}
		return res;
	}
	
	void addVariable(Variable::Arg v) {
		polynomialParser.addVariable(v);
	}
};

}
}