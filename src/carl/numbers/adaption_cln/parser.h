#pragma once

#include "../parser/parser.h"
#include "operations.h"

namespace boost { namespace spirit { namespace traits {
#if BOOST_VERSION >= 105900
    template<> inline void scale(int exp, cln::cl_I& r, cln::cl_I rin) {
        assert(exp >= 0);
        r = rin * carl::pow(cln::cl_I(10), (unsigned)exp);
    }
    template<> inline bool scale(int exp, cln::cl_RA& r, cln::cl_RA rin) {
        if (exp >= 0)
            r = rin * carl::pow(cln::cl_RA(10), (unsigned)exp);
        else
            r = rin / carl::pow(cln::cl_RA(10), (unsigned)(-exp));
		return true;
    }
#else
    template<> inline void scale(int exp, cln::cl_I& r) {
        assert(exp >= 0);
        r *= carl::pow(cln::cl_I(10), (unsigned)exp);
    }
    template<> inline void scale(int exp, cln::cl_RA& r) {
        if (exp >= 0)
            r *= carl::pow(cln::cl_RA(10), (unsigned)exp);
        else
            r /= carl::pow(cln::cl_RA(10), (unsigned)(-exp));
    }
#endif
    template<> inline bool is_equal_to_one(const cln::cl_I& value) {
        return carl::isOne(value);
    }
    template<> inline bool is_equal_to_one(const cln::cl_RA& value) {
        return carl::isOne(value);
    }
}}}
