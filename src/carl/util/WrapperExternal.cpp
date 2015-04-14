#include "WrapperExternal.h"
#include "stringparser.h"
#include <iostream>

namespace carl {
	void WrapperExternal::sendTest(const char* s) {
		std::cout << "Parsing in carl:" << std::endl;
		std::cout << s << std::endl;

		carl::parser::Parser<MultivariatePolynomial<Rational>> parser;
		FT result = parser.formula(s);
		std::cout << "Parsing result: " << result << std::endl;
		Variables vars;
		result.booleanVars(vars);
		std::cout << "Vars: " << vars << std::endl;
	}

	const char* WrapperExternal::getResult() {
		std::cout << "Getting result:" << std::endl;
		const char* result = "unknown";
		std::cout << result << std::endl;
		return result;
	}
}
