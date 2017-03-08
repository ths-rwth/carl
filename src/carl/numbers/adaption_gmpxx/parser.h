#pragma once

#include "../parser/parser.h"
#include "operations.h"

namespace boost { namespace spirit { namespace traits {
#if BOOST_VERSION >= 105900
	template<> inline bool scale(int exp, mpz_class& r, mpz_class rin) {
		if (exp >= 0)
			r = rin * carl::pow(mpz_class(10), (unsigned)exp);
		else
			r = carl::div(rin, carl::pow(mpz_class(10), (unsigned)(-exp)));
		return true;
	}
	template<> inline bool scale(int exp, mpq_class& r, mpq_class rin) {
		if (exp >= 0)
			r = rin * carl::pow(mpq_class(10), (unsigned)exp);
		else
			r = rin / carl::pow(mpq_class(10), (unsigned)(-exp));
		return true;
	}
#else
	template<> inline void scale(int exp, mpz_class& r) {
		if (exp >= 0)
			r *= carl::pow(mpz_class(10), (unsigned)exp);
		else
			r = carl::div(r, carl::pow(mpz_class(10), (unsigned)(-exp)));
	}
	template<> inline void scale(int exp, mpq_class& r) {
		if (exp >= 0)
			r *= carl::pow(mpq_class(10), (unsigned)exp);
		else
			r /= carl::pow(mpq_class(10), (unsigned)(-exp));
	}
#endif
	template<> inline bool is_equal_to_one(const mpz_class& value) {
		return carl::isOne(value);
	}
	template<> inline bool is_equal_to_one(const mpq_class& value) {
		return carl::isOne(value);
	}
}}}
