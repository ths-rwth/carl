/**
* @file WrapperExternal.h
* @author Matthias Volk
* Defines the exported functions for the library
*/
#pragma once

#include "parser/Parser.h"
#include "platform.h"

#ifdef __WIN
	#pragma warning(push, 0)
	#include <mpirxx.h>
	#pragma warning(pop)
	typedef mpq_class Rational;
	typedef mpz_class Integer;
	#define DLL_EXPORT __declspec(dllexport)
#else
	#include <gmpxx.h>
	typedef mpq_class Rational;
	typedef mpz_class Integer;
	#define DLL_EXPORT 
#endif

namespace carl {
	class WrapperExternal
	{
	private:
		typedef Formula<MultivariatePolynomial<Rational>> FT;
	public:
        /**
         * Parse formula.
         * @param input String to parse
         * @param buffer Buffer for resulting string
         * @param bufferSize Size of buffer
         */
        DLL_EXPORT void parseFormula(const char* input, char* buffer, int bufferSize);
	};
}
