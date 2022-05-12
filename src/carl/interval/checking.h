/* 
 * A struct, which contains the checking policies for FLOAT_T intervals when used as boost intervals.
 * 
 * @file   checking.h
 * @author Stefan Schupp
 * @author Benedikt Seidl
 *
 * @since	2013-12-05
 * @version 2014-01-27
 */

#pragma once

#include <carl/numbers/numbers.h>

#include <cassert>

namespace carl
{
    template<typename Number>
    struct checking
    {
        static Number pos_inf()
        {
            return Number(1);
        }
        static Number neg_inf()
        {
            return Number(-1);
        }
        static Number nan()
        {
            assert(false);
			return Number(0);
        }
        static bool is_nan(const Number& /*unused*/)
        {
            return false;
        }
        static Number empty_lower()
        {
            return Number(0);
        }
        static Number empty_upper()
        {
            return Number(0);
        }
        static bool is_empty(const Number& _left, const Number& _right)
        {
            return _left > _right;
        }
    };
}
