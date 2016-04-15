#include "mod_core.h"
#include "mod_func.h"

#include "parser.h"

//#include "carl/util/parser/Parser.h"
#include "carl/util/parser/ExpressionParser.h"

carl::parser::ExpressionType<Polynomial> parse(const std::string& input) {
	typedef std::string::const_iterator It;
	It f(begin(input)), l(end(input));
	try {
		carl::parser::ExpressionParser<Polynomial> parser;
		carl::parser::Skipper skipper;
		carl::parser::ExpressionType<Polynomial> data;

		bool parse_result = qi::phrase_parse(f, l, parser, skipper, data);
		//bool parse_result = qi::parse(f, l, parser, data);

		if (!parse_result) {
			CARL_LOG_ERROR("carl.parser", "Parsing \"" << input << "\" failed.");
		}
		if (f != l) {
			CARL_LOG_ERROR("carl.parser", "Parsing \"" << input << "\" leaves trailing " << std::string(f,l));
		}
		return data;
	} catch(const qi::expectation_failure<It>& e) {
		std::string frag(e.first, e.last);
		throw std::runtime_error(std::string(e.what()) + ": " + frag);
	}
}
