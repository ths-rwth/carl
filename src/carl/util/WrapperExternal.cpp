#include "WrapperExternal.h"
#include "stringparser.h"
#include <iostream>

namespace carl {
	void WrapperExternal::parseFormula(const char* input, char* buffer, int bufferSize) {
		carl::parser::Parser<MultivariatePolynomial<Rational>> parser;
		FT parseResult = parser.formula(input);
		// Copy result in buffer for external program
		strcpy_s(buffer, bufferSize, parseResult.toString().c_str());
    }
}
