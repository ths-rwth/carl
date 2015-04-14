#include "WrapperExternal.h"
#include "stringparser.h"
#include <iostream>

namespace carl {
	void WrapperExternal::sendTest(const char* s) {
		std::cout << "Parsing in carl:" << std::endl;

		StringParser sp;
		sp.setImplicitMultiplicationMode(false);
		sp.setVariables({ "x", "y", "z" });
		Term<mpq_class> term1 = sp.parseTerm<mpq_class>("x*y^3*z^2");
		std::cout << term1 << std::endl;
		Term<mpq_class> term2 = sp.parseTerm<mpq_class>(s);
		std::cout << term2 << std::endl;
	}

	const char* WrapperExternal::getResult() {
		std::cout << "Getting result:" << std::endl;
		const char* result = "unknown";
		std::cout << result << std::endl;
		return result;
	}
}