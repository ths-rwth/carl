/**
 * @file Parser.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <sstream>

#include "../../core/logging.h"
#include "Common.h"
#include "ExpressionParser.h"

namespace carl {
namespace parser {

template<typename Coeff>
class Parser {
private:
	Skipper skipper;
	ExpressionParser<Coeff> expressionParser;
	typedef typename ExpressionParser<Coeff>::expr_type expr_type;
	
	void parse(const std::string& s, expr_type& res) {
		auto begin = s.begin();
		bool parseResult = qi::phrase_parse(begin, s.end(), expressionParser, skipper, res);
		if (!parseResult) {
			CARL_LOG_ERROR("carl.parser", "Parsing \"" << s << "\" failed.");
			throw std::runtime_error("Unable to parse expression");
		}
		//boost::apply_visitor(typename ExpressionParser<Coeff>::print_expr_type(), res);
	}
public:
	
	Poly<Coeff> polynomial(const std::string& s) {
		expr_type res;
		parse(s, res);
		return boost::apply_visitor(typename ExpressionParser<Coeff>::to_poly(), res);
	}
	
	RatFun<Coeff> rationalFunction(const std::string& s) {
		expr_type res;
		parse(s, res);
		return boost::apply_visitor(typename ExpressionParser<Coeff>::to_ratfun(), res);
	}
	
	void addVariable(Variable::Arg v) {
		expressionParser.addVariable(v);
	}
};

}
}
