/**
* @file WrapperExternal.h
* @author Matthias Volk
* Defines the exported functions for the library
*/
#pragma once

#include "parser/Parser.h"
#include "platform.h"

#ifdef __WIN
#include <mpirxx.h>
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
		DLL_EXPORT void sendTest(const char* s);

		DLL_EXPORT const char* getResult();
	};
}

