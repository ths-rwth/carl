/* 
 * This file contains the rounding policies needed from the boost interval class for the general
 * FLOAT_T<FloatImplementation> type used in carl.
 * 
 * @file   rounding.h
 * @author Stefan Schupp
 *
 * @since 2013-12-05
 * @version 2013-12-05
 */

#pragma once
#include "../numbers/FLOAT_T.h"

namespace carl
{
    template<typename FloatImplementation>
    struct rounding 
    {
        // default constructor, destructor
//        rounding();
//        ~rounding();
        // mathematical operations
        FLOAT_T<FloatImplementation> add_down(FLOAT_T<FloatImplementation> _lhs, FLOAT_T<FloatImplementation> _rhs) // [-∞;+∞][-∞;+∞]
        {
            FLOAT_T<FloatImplementation> result;
            _lhs.add(result, _rhs, CARL_RND::CARL_RNDD);
            return result;
        }

        FLOAT_T<FloatImplementation> add_up  (FLOAT_T<FloatImplementation> _lhs, FLOAT_T<FloatImplementation> _rhs) // [-∞;+∞][-∞;+∞]
        {
            FLOAT_T<FloatImplementation> result;
            _lhs.add(result, _rhs, CARL_RND::CARL_RNDU);
            return result;
        }

        FLOAT_T<FloatImplementation> sub_down(FLOAT_T<FloatImplementation> _lhs, FLOAT_T<FloatImplementation> _rhs) // [-∞;+∞][-∞;+∞]
        {
            FLOAT_T<FloatImplementation> result;
            _lhs.sub(result, _rhs, CARL_RND::CARL_RNDD);
            return result;
        }

        FLOAT_T<FloatImplementation> sub_up  (FLOAT_T<FloatImplementation> _lhs, FLOAT_T<FloatImplementation> _rhs) // [-∞;+∞][-∞;+∞]
        {
            FLOAT_T<FloatImplementation> result;
            _lhs.sub(result, _rhs, CARL_RND::CARL_RNDU);
            return result;
        }

        FLOAT_T<FloatImplementation> mul_down(FLOAT_T<FloatImplementation> _lhs, FLOAT_T<FloatImplementation> _rhs) // [-∞;+∞][-∞;+∞]
        {
            FLOAT_T<FloatImplementation> result;
            _lhs.mul(result, _rhs, CARL_RND::CARL_RNDD);
            return result;
        }

        FLOAT_T<FloatImplementation> mul_up  (FLOAT_T<FloatImplementation> _lhs, FLOAT_T<FloatImplementation> _rhs) // [-∞;+∞][-∞;+∞]
        {
            FLOAT_T<FloatImplementation> result;
            _lhs.mul(result, _rhs, CARL_RND::CARL_RNDU);
            return result;
        }

        FLOAT_T<FloatImplementation> div_down(FLOAT_T<FloatImplementation> _lhs, FLOAT_T<FloatImplementation> _rhs) // [-∞;+∞]([-∞;+∞]-{0})
        {
            FLOAT_T<FloatImplementation> result;
            _lhs.div(result, _rhs, CARL_RND::CARL_RNDD);
            return result;
        }

        FLOAT_T<FloatImplementation> div_up  (FLOAT_T<FloatImplementation> _lhs, FLOAT_T<FloatImplementation> _rhs) // [-∞;+∞]([-∞;+∞]-{0})
        {
            FLOAT_T<FloatImplementation> result;
            _lhs.div(result, _rhs, CARL_RND::CARL_RNDU);
            return result;
        }

        FLOAT_T<FloatImplementation> sqrt_down(FLOAT_T<FloatImplementation> _val)   // ]0;+∞]
        {
            FLOAT_T<FloatImplementation> result;
            _val.sqrt(result, CARL_RND::CARL_RNDD);
            return result;
        }
        FLOAT_T<FloatImplementation> sqrt_up  (FLOAT_T<FloatImplementation> _val)   // ]0;+∞]
        {
            FLOAT_T<FloatImplementation> result;
            _val.sqrt(result, CARL_RND::CARL_RNDU);
            return result;
        }
        FLOAT_T<FloatImplementation> exp_down(FLOAT_T<FloatImplementation> _val)    // [-∞;+∞]
        {
            FLOAT_T<FloatImplementation> result;
            _val.exp(result, CARL_RND::CARL_RNDD);
            return result;
        }
        FLOAT_T<FloatImplementation> exp_up  (FLOAT_T<FloatImplementation> _val)    // [-∞;+∞]
        {
            FLOAT_T<FloatImplementation> result;
            _val.exp(result, CARL_RND::CARL_RNDU);
            return result;
        }
        FLOAT_T<FloatImplementation> log_down(FLOAT_T<FloatImplementation> _val)    // ]0;+∞]
        {
            FLOAT_T<FloatImplementation> result;
            _val.log(result, CARL_RND::CARL_RNDD);
            return result;
        }
        FLOAT_T<FloatImplementation> log_up  (FLOAT_T<FloatImplementation> _val)    // ]0;+∞]
        {
            FLOAT_T<FloatImplementation> result;
            _val.log(result, CARL_RND::CARL_RNDU);
            return result;
        }
        FLOAT_T<FloatImplementation> cos_down(FLOAT_T<FloatImplementation> _val)    // [0;2π]
        {
            FLOAT_T<FloatImplementation> result;
            _val.cos(result, CARL_RND::CARL_RNDD);
            return result;
        }
        FLOAT_T<FloatImplementation> cos_up  (FLOAT_T<FloatImplementation> _val)    // [0;2π]
        {
            FLOAT_T<FloatImplementation> result;
            _val.cos(result, CARL_RND::CARL_RNDU);
            return result;
        }
        FLOAT_T<FloatImplementation> tan_down(FLOAT_T<FloatImplementation> _val)    // ]-π/2;π/2[
        {
            FLOAT_T<FloatImplementation> result;
            _val.tan(result, CARL_RND::CARL_RNDD);
            return result;
        }
        FLOAT_T<FloatImplementation> tan_up  (FLOAT_T<FloatImplementation> _val)    // ]-π/2;π/2[
        {
            FLOAT_T<FloatImplementation> result;
            _val.tan(result, CARL_RND::CARL_RNDU);
            return result;
        }
        FLOAT_T<FloatImplementation> asin_down(FLOAT_T<FloatImplementation> _val)   // [-1;1]
        {
            FLOAT_T<FloatImplementation> result;
            _val.asin(result, CARL_RND::CARL_RNDD);
            return result;
        }
        FLOAT_T<FloatImplementation> asin_up  (FLOAT_T<FloatImplementation> _val)   // [-1;1]
        {
            FLOAT_T<FloatImplementation> result;
            _val.asin(result, CARL_RND::CARL_RNDU);
            return result;
        }
        FLOAT_T<FloatImplementation> acos_down(FLOAT_T<FloatImplementation> _val)   // [-1;1]
        {
            FLOAT_T<FloatImplementation> result;
            _val.acos(result, CARL_RND::CARL_RNDD);
            return result;
        }
        FLOAT_T<FloatImplementation> acos_up  (FLOAT_T<FloatImplementation> _val)   // [-1;1]
        {
            FLOAT_T<FloatImplementation> result;
            _val.acos(result, CARL_RND::CARL_RNDU);
            return result;
        }
        FLOAT_T<FloatImplementation> atan_down(FLOAT_T<FloatImplementation> _val)   // [-∞;+∞]
        {
            FLOAT_T<FloatImplementation> result;
            _val.atan(result, CARL_RND::CARL_RNDD);
            return result;
        }
        FLOAT_T<FloatImplementation> atan_up  (FLOAT_T<FloatImplementation> _val)   // [-∞;+∞]
        {
            FLOAT_T<FloatImplementation> result;
            _val.atan(result, CARL_RND::CARL_RNDU);
            return result;
        }
        FLOAT_T<FloatImplementation> sinh_down(FLOAT_T<FloatImplementation> _val)   // [-∞;+∞]
        {
            FLOAT_T<FloatImplementation> result;
            _val.sinh(result, CARL_RND::CARL_RNDD);
            return result;
        }
        FLOAT_T<FloatImplementation> sinh_up  (FLOAT_T<FloatImplementation> _val)   // [-∞;+∞]
        {
            FLOAT_T<FloatImplementation> result;
            _val.sinh(result, CARL_RND::CARL_RNDU);
            return result;
        }
        FLOAT_T<FloatImplementation> cosh_down(FLOAT_T<FloatImplementation> _val)   // [-∞;+∞]
        {
            FLOAT_T<FloatImplementation> result;
            _val.cosh(result, CARL_RND::CARL_RNDD);
            return result;
        }
        FLOAT_T<FloatImplementation> cosh_up  (FLOAT_T<FloatImplementation> _val)   // [-∞;+∞]
        {
            FLOAT_T<FloatImplementation> result;
            _val.cosh(result, CARL_RND::CARL_RNDU);
            return result;
        }
        FLOAT_T<FloatImplementation> tanh_down(FLOAT_T<FloatImplementation> _val)   // [-∞;+∞]
        {
            FLOAT_T<FloatImplementation> result;
            _val.tanh(result, CARL_RND::CARL_RNDD);
            return result;
        }
        FLOAT_T<FloatImplementation> tanh_up  (FLOAT_T<FloatImplementation> _val)   // [-∞;+∞]
        {
            FLOAT_T<FloatImplementation> result;
            _val.tanh(result, CARL_RND::CARL_RNDU);
            return result;
        }
        FLOAT_T<FloatImplementation> asinh_down(FLOAT_T<FloatImplementation> _val)  // [-∞;+∞]
        {
            FLOAT_T<FloatImplementation> result;
            _val.asinh(result, CARL_RND::CARL_RNDD);
            return result;
        }
        FLOAT_T<FloatImplementation> asinh_up  (FLOAT_T<FloatImplementation> _val)  // [-∞;+∞]
        {
            FLOAT_T<FloatImplementation> result;
            _val.asinh(result, CARL_RND::CARL_RNDU);
            return result;
        }
        FLOAT_T<FloatImplementation> acosh_down(FLOAT_T<FloatImplementation> _val)  // [1;+∞]
        {
            FLOAT_T<FloatImplementation> result;
            _val.acosh(result, CARL_RND::CARL_RNDD);
            return result;
        }
        FLOAT_T<FloatImplementation> acosh_up  (FLOAT_T<FloatImplementation> _val)  // [1;+∞]
        {
            FLOAT_T<FloatImplementation> result;
            _val.acosh(result, CARL_RND::CARL_RNDU);
            return result;
        }
        FLOAT_T<FloatImplementation> atanh_down(FLOAT_T<FloatImplementation> _val)  // [-1;1]
        {
            FLOAT_T<FloatImplementation> result;
            _val.atanh(result, CARL_RND::CARL_RNDD);
            return result;
        }
        FLOAT_T<FloatImplementation> atanh_up  (FLOAT_T<FloatImplementation> _val)  // [-1;1] 
        {
            FLOAT_T<FloatImplementation> result;
            _val.atanh(result, CARL_RND::CARL_RNDU);
            return result;
        }
        FLOAT_T<FloatImplementation> median(FLOAT_T<FloatImplementation> _val1, FLOAT_T<FloatImplementation> _val2)   // [-∞;+∞][-∞;+∞]
        {
            FLOAT_T<FloatImplementation> result;
            result.add_assign(_val1);
            result.add_assign(_val2);
            result.div_assign(FLOAT_T<FloatImplementation>(2));
            return result;
        }
        FLOAT_T<FloatImplementation> int_down(FLOAT_T<FloatImplementation> _val)    // [-∞;+∞]
        {
            FLOAT_T<FloatImplementation> result(_val);
            _val.floor(result, CARL_RND::CARL_RNDU);
            return result;
        }
        FLOAT_T<FloatImplementation> int_up  (FLOAT_T<FloatImplementation> _val)    // [-∞;+∞]
        {
            FLOAT_T<FloatImplementation> result(_val);
            _val.ceil(result, CARL_RND::CARL_RNDU);
            return result;
        }
        // conversion functions
        template<typename U>
        FLOAT_T<FloatImplementation> conv_down(U _val)
        {
            return FLOAT_T<FloatImplementation>(_val, CARL_RND::CARL_RNDD);
        }
        
        template<typename U>
        FLOAT_T<FloatImplementation> conv_up(U _val)
        {
            return FLOAT_T<FloatImplementation>(_val, CARL_RND::CARL_RNDU);
        }
        // unprotected rounding class
//    typedef ... unprotected_rounding;
    };
}
