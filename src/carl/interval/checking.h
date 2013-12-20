/* 
 * A struct, which contains the checking policies for FLOAT_T intervals when used as boost intervals.
 * 
 * @file   checking.h
 * @author Stefan Schupp
 *
 * @since	2013-12-05
 * @version 2013-12-20
 */

#pragma once
//#include "../numbers/FLOAT_T.h"
#include <assert.h>


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
        }
        static bool is_nan(const Number&)
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