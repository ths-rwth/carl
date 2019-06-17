#pragma once

#include "../config.h"

#ifdef USE_CLN_NUMBERS

#include "../parser/parser.h"
#include "operations.h"
#include "typetraits.h"

namespace boost { namespace spirit { namespace traits {
#if BOOST_VERSION >= 105900
	template<> inline bool scale(int exp, cln::cl_I& r, cln::cl_I rin) {
		if (exp >= 0)
			r = rin * carl::pow(cln::cl_I(10), static_cast<unsigned>(exp));
		else
			r = carl::div(rin, carl::pow(cln::cl_I(10), static_cast<unsigned>(-exp)));
		return true;
	}
	template<> inline bool scale(int exp, cln::cl_RA& r, cln::cl_RA rin) {
		if (exp >= 0)
			r = rin * carl::pow(cln::cl_RA(10), static_cast<unsigned>(exp));
		else
			r = rin / carl::pow(cln::cl_RA(10), static_cast<unsigned>(-exp));
		return true;
	}
#else
	template<> inline void scale(int exp, cln::cl_I& r) {
		if (exp >= 0)
			r *= carl::pow(cln::cl_I(10), static_cast<unsigned>(exp));
		else
			r = carl::div(r, carl::pow(cln::cl_I(10), static_cast<unsigned>(-exp)));
	}
	template<> inline void scale(int exp, cln::cl_RA& r) {
		if (exp >= 0)
			r *= carl::pow(cln::cl_RA(10), static_cast<unsigned>(exp));
		else
			r /= carl::pow(cln::cl_RA(10), static_cast<unsigned>(-exp));
	}
#endif
#if BOOST_VERSION < 107000
	template<> inline bool is_equal_to_one(const cln::cl_I& value) {
		return carl::isOne(value);
	}
	template<> inline bool is_equal_to_one(const cln::cl_RA& value) {
		return carl::isOne(value);
	}
#endif
}}}

#endif
