#pragma once

#include "../parser/parser.h"
#include "operations.h"

namespace boost { namespace spirit { namespace traits {
#if BOOST_VERSION >= 105900
	template<> inline bool scale(int exp, mpz_class& n, mpz_class acc_n) {
		if (exp >= 0)
			n = acc_n * carl::pow(mpz_class(10), unsigned(exp));
		else
			n = carl::div(acc_n, carl::pow(mpz_class(10), unsigned(-exp)));
		return true;
	}
	template<> inline bool scale(int exp, mpq_class& n, mpq_class acc_n) {
		if (exp >= 0)
			n = acc_n * carl::pow(mpq_class(10), unsigned(exp));
		else
			n = acc_n / carl::pow(mpq_class(10), unsigned(-exp));
		return true;
	}
#else
	template<> inline void scale(int exp, mpz_class& n) {
		if (exp >= 0)
			n *= carl::pow(mpz_class(10), unsigned(exp));
		else
			n = carl::div(n, carl::pow(mpz_class(10), unsigned(-exp)));
	}
	template<> inline void scale(int exp, mpq_class& n) {
		if (exp >= 0)
			n *= carl::pow(mpq_class(10), unsigned(exp));
		else
			n /= carl::pow(mpq_class(10), unsigned(-exp));
	}
#endif
	template<> inline bool is_equal_to_one(const mpz_class& value) {
		return carl::isOne(value);
	}
	template<> inline bool is_equal_to_one(const mpq_class& value) {
		return carl::isOne(value);
	}
}}}
