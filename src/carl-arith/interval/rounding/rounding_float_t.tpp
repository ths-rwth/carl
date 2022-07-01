/* 
 * This file contains the rounding policies needed from the boost interval class
 * for the FLOAT_T<FloatType> type used in carl.
 * 
 * @file   rounding.h
 * @author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 *
 * @since   2013-12-05
 * @version 2014-01-30
 */

#pragma once
#include "../../numbers/numbers.h"

namespace carl
{
    template<typename FloatType>
    struct rounding<FLOAT_T<FloatType> >
    {
        // default constructor, destructor
//        rounding();
//        ~rounding();
        // mathematical operations
        FLOAT_T<FloatType> add_down(FLOAT_T<FloatType> _lhs, FLOAT_T<FloatType> _rhs) // [-∞;+∞][-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _lhs.add(result, _rhs, CARL_RND::D );
            return result;
        }

        FLOAT_T<FloatType> add_up(FLOAT_T<FloatType> _lhs, FLOAT_T<FloatType> _rhs) // [-∞;+∞][-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _lhs.add(result, _rhs, CARL_RND::U );
            return result;
        }

        FLOAT_T<FloatType> sub_down(FLOAT_T<FloatType> _lhs, FLOAT_T<FloatType> _rhs) // [-∞;+∞][-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _lhs.sub(result, _rhs, CARL_RND::D );
            return result;
        }

        FLOAT_T<FloatType> sub_up  (FLOAT_T<FloatType> _lhs, FLOAT_T<FloatType> _rhs) // [-∞;+∞][-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _lhs.sub(result, _rhs, CARL_RND::U );
            return result;
        }

        FLOAT_T<FloatType> mul_down(FLOAT_T<FloatType> _lhs, FLOAT_T<FloatType> _rhs) // [-∞;+∞][-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _lhs.mul(result, _rhs, CARL_RND::D );
            return result;
        }

        FLOAT_T<FloatType> mul_up  (FLOAT_T<FloatType> _lhs, FLOAT_T<FloatType> _rhs) // [-∞;+∞][-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _lhs.mul(result, _rhs, CARL_RND::U );
            return result;
        }

        FLOAT_T<FloatType> div_down(FLOAT_T<FloatType> _lhs, FLOAT_T<FloatType> _rhs) // [-∞;+∞]([-∞;+∞]-{0})
        {
            FLOAT_T<FloatType> result;
            _lhs.div(result, _rhs, CARL_RND::D );
            return result;
        }

        FLOAT_T<FloatType> div_up  (FLOAT_T<FloatType> _lhs, FLOAT_T<FloatType> _rhs) // [-∞;+∞]([-∞;+∞]-{0})
        {
            FLOAT_T<FloatType> result;
            _lhs.div(result, _rhs, CARL_RND::U );
            return result;
        }

        FLOAT_T<FloatType> sqrt_down(FLOAT_T<FloatType> _val)   // ]0;+∞]
        {
            if( _val <= 0 )
                return FLOAT_T<FloatType>(0);
            
            FLOAT_T<FloatType> result;
            _val.sqrt(result, CARL_RND::D );
            return result;
        }
        FLOAT_T<FloatType> sqrt_up  (FLOAT_T<FloatType> _val)   // ]0;+∞]
        {
            if( _val <= 0 )
                return FLOAT_T<FloatType>(0);
            
            FLOAT_T<FloatType> result;
            _val.sqrt(result, CARL_RND::U );
            return result;
        }
        FLOAT_T<FloatType> exp_down(FLOAT_T<FloatType> _val)    // [-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _val.exp(result, CARL_RND::D );
            return result;
        }
        FLOAT_T<FloatType> exp_up  (FLOAT_T<FloatType> _val)    // [-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _val.exp(result, CARL_RND::U );
            return result;
        }
        FLOAT_T<FloatType> log_down(FLOAT_T<FloatType> _val)    // ]0;+∞]
        {
            FLOAT_T<FloatType> result;
            _val.log(result, CARL_RND::D );
            return result;
        }
        FLOAT_T<FloatType> log_up  (FLOAT_T<FloatType> _val)    // ]0;+∞]
        {
            FLOAT_T<FloatType> result;
            _val.log(result, CARL_RND::U );
            return result;
        }
        FLOAT_T<FloatType> cos_down(FLOAT_T<FloatType> _val)    // [0;2π]
        {
            FLOAT_T<FloatType> result;
            _val.cos(result, CARL_RND::D );
            return result;
        }
        FLOAT_T<FloatType> cos_up  (FLOAT_T<FloatType> _val)    // [0;2π]
        {
            FLOAT_T<FloatType> result;
            _val.cos(result, CARL_RND::U );
            return result;
        }
        FLOAT_T<FloatType> tan_down(FLOAT_T<FloatType> _val)    // ]-π/2;π/2[
        {
            FLOAT_T<FloatType> result;
            _val.tan(result, CARL_RND::D );
            return result;
        }
        FLOAT_T<FloatType> tan_up  (FLOAT_T<FloatType> _val)    // ]-π/2;π/2[
        {
            FLOAT_T<FloatType> result;
            _val.tan(result, CARL_RND::U );
            return result;
        }
        FLOAT_T<FloatType> asin_down(FLOAT_T<FloatType> _val)   // [-1;1]
        {
            FLOAT_T<FloatType> result;
            _val.asin(result, CARL_RND::D );
            return result;
        }
        FLOAT_T<FloatType> asin_up  (FLOAT_T<FloatType> _val)   // [-1;1]
        {
            FLOAT_T<FloatType> result;
            _val.asin(result, CARL_RND::U );
            return result;
        }
        FLOAT_T<FloatType> acos_down(FLOAT_T<FloatType> _val)   // [-1;1]
        {
            FLOAT_T<FloatType> result;
            _val.acos(result, CARL_RND::D );
            return result;
        }
        FLOAT_T<FloatType> acos_up  (FLOAT_T<FloatType> _val)   // [-1;1]
        {
            FLOAT_T<FloatType> result;
            _val.acos(result, CARL_RND::U );
            return result;
        }
        FLOAT_T<FloatType> atan_down(FLOAT_T<FloatType> _val)   // [-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _val.atan(result, CARL_RND::D );
            return result;
        }
        FLOAT_T<FloatType> atan_up  (FLOAT_T<FloatType> _val)   // [-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _val.atan(result, CARL_RND::U );
            return result;
        }
        FLOAT_T<FloatType> sinh_down(FLOAT_T<FloatType> _val)   // [-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _val.sinh(result, CARL_RND::D );
            return result;
        }
        FLOAT_T<FloatType> sinh_up  (FLOAT_T<FloatType> _val)   // [-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _val.sinh(result, CARL_RND::U );
            return result;
        }
        FLOAT_T<FloatType> cosh_down(FLOAT_T<FloatType> _val)   // [-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _val.cosh(result, CARL_RND::D );
            return result;
        }
        FLOAT_T<FloatType> cosh_up  (FLOAT_T<FloatType> _val)   // [-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _val.cosh(result, CARL_RND::U );
            return result;
        }
        FLOAT_T<FloatType> tanh_down(FLOAT_T<FloatType> _val)   // [-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _val.tanh(result, CARL_RND::D );
            return result;
        }
        FLOAT_T<FloatType> tanh_up  (FLOAT_T<FloatType> _val)   // [-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _val.tanh(result, CARL_RND::U );
            return result;
        }
        FLOAT_T<FloatType> asinh_down(FLOAT_T<FloatType> _val)  // [-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _val.asinh(result, CARL_RND::D );
            return result;
        }
        FLOAT_T<FloatType> asinh_up  (FLOAT_T<FloatType> _val)  // [-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _val.asinh(result, CARL_RND::U );
            return result;
        }
        FLOAT_T<FloatType> acosh_down(FLOAT_T<FloatType> _val)  // [1;+∞]
        {
            FLOAT_T<FloatType> result;
            _val.acosh(result, CARL_RND::D );
            return result;
        }
        FLOAT_T<FloatType> acosh_up  (FLOAT_T<FloatType> _val)  // [1;+∞]
        {
            FLOAT_T<FloatType> result;
            _val.acosh(result, CARL_RND::U );
            return result;
        }
        FLOAT_T<FloatType> atanh_down(FLOAT_T<FloatType> _val)  // [-1;1]
        {
            FLOAT_T<FloatType> result;
            _val.atanh(result, CARL_RND::D );
            return result;
        }
        FLOAT_T<FloatType> atanh_up  (FLOAT_T<FloatType> _val)  // [-1;1] 
        {
            FLOAT_T<FloatType> result;
            _val.atanh(result, CARL_RND::U );
            return result;
        }
        FLOAT_T<FloatType> median(FLOAT_T<FloatType> _val1, FLOAT_T<FloatType> _val2)   // [-∞;+∞][-∞;+∞]
        {
			// @todo: Median <. arithmetisches Mittel ???
            FLOAT_T<FloatType> result;
            result.add_assign(_val1);
            result.add_assign(_val2);
            result.div_assign(FLOAT_T<FloatType>(2));
            return result;
        }
        FLOAT_T<FloatType> int_down(FLOAT_T<FloatType> _val)    // [-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _val.floor(result, CARL_RND::D );
            return result;
        }
        FLOAT_T<FloatType> int_up  (FLOAT_T<FloatType> _val)    // [-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _val.ceil(result, CARL_RND::U );
            return result;
        }
        // conversion functions
        template<typename U>
        FLOAT_T<FloatType> conv_down(U _val)
        {
            return FLOAT_T<FloatType>(_val, CARL_RND::D );
        }
        
        template<typename U>
        FLOAT_T<FloatType> conv_up(U _val)
        {
            return FLOAT_T<FloatType>(_val, CARL_RND::U );
        }
        // unprotected rounding class
//    typedef ... unprotected_rounding;
    };
}
