/**
 * @file Parser.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <sstream>

#include <carl-logging/carl-logging.h>
#include "Common.h"
#include <carl-arith/numbers/numbers.h>
#include "FormulaParser.h"
#include "PolynomialParser.h"
#include "RationalFunctionParser.h"

namespace carl::io {
namespace parser {

template<typename Pol>
class Parser {
private:
	Skipper skipper;
	PolynomialParser<Pol> polynomialParser;
	RationalFunctionParser<Pol> ratfunParser;
	FormulaParser<Pol> formulaParser;
	
	template<typename Result, typename Parser>
	bool parse(const std::string& s, const Parser& parser, Result& res) {
		auto begin = s.begin();
		return qi::phrase_parse(begin, s.end(), parser, skipper, res);
	}
public:
	Parser():
		skipper(),
		polynomialParser(),
		ratfunParser(),
		formulaParser()
	{
	}

	Pol polynomial(const std::string& s) {
		Pol res;
		if (!parse(s, polynomialParser, res)) {
			CARL_LOG_ERROR("carl.parser", "Parsing \"" << s << "\" to a polynomial failed.");
		}
		return res;
	}
	
	RatFun<Pol> rationalFunction(const std::string& s) {
		RatFun<Pol> res;
		if (!parse(s, ratfunParser, res)) {
			CARL_LOG_ERROR("carl.parser", "Parsing \"" << s << "\" to a rational function failed.");
		}
		return res;
	}
	
	Formula<Pol> formula(const std::string& s) {
		Formula<Pol> res;
		if (!parse(s, formulaParser, res)) {
            std::cout << "NOPE!" << std::endl;
			CARL_LOG_ERROR("carl.parser", "Parsing \"" << s << "\" to a formula failed.");
		}
		return res;
	}
	
	void addVariable(Variable::Arg v) {
        if( v.type() == VariableType::VT_BOOL )
            formulaParser.addVariable(v);
        else {
            polynomialParser.addVariable(v);
			ratfunParser.addVariable(v);
		}
	}
};

}
}
