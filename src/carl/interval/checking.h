/* 
 * A struct, which contains the checking policies for FLOAT_T intervals when used as boost intervals.
 * 
 * @file   checking.h
 * @author Stefan Schupp
 *
 * @since 2013-12-05
 * @version 2013-12-05
 */

#pragma once
#include "../../numbers/FLOAT_T.h"
#include <assert.h>

namespace carl
{
    /* requirements for checking policy */
    template<FloatImplementation>
    struct checking
    {
        static FLOAT_T<FloatImplementation> pos_inf()
        {
            return FLOAT_T<FloatImplementation>(1);
        }
        static FLOAT_T<FloatImplementation> neg_inf()
        {
            return FLOAT_T<FloatImplementation>(-1);
        }
        static FLOAT_T<FloatImplementation> nan()
        {
            assert(false);
        }
        static bool is_nan(const FLOAT_T<FloatImplementation>&)
        {
            return false;
        }
        static FLOAT_T<FloatImplementation> empty_lower()
        {
            return FLOAT_T<FloatImplementation>(0);
        }
        static FLOAT_T<FloatImplementation> empty_upper()
        {
            return FLOAT_T<FloatImplementation>(0);
        }
        static bool is_empty(const FLOAT_T<FloatImplementation>& _left, const FLOAT_T<FloatImplementation>& _right)
        {
            return _left > _right;
        }
    };
}