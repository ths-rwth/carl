/* 
 * This file contains the rounding policies needed from the boost interval class
 * for the FLOAT_T<FloatType> type used in carl.
 * 
 * @file   rounding.h
 * @author Stefan Schupp
 *
 * @since   2013-12-05
 * @version 2013-12-20
 */

#pragma once
#include "../../numbers/roundingConversion.h"
#include "../../numbers/FLOAT_T.h"
//#include "../numbers/operations_mpfr.h"
//#include <mpfr.h>

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
            _lhs->add(*result, *_rhs, convRnd<FLOAT_T<FloatType> >(CARL_RND::D) );
            return result;
        }

        FLOAT_T<FloatType> add_up(FLOAT_T<FloatType> _lhs, FLOAT_T<FloatType> _rhs) // [-∞;+∞][-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _lhs->add(*result, *_rhs, convRnd<FLOAT_T<FloatType> >(CARL_RND::U) );
            return result;
        }

        FLOAT_T<FloatType> sub_down(FLOAT_T<FloatType> _lhs, FLOAT_T<FloatType> _rhs) // [-∞;+∞][-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _lhs->sub(*result, *_rhs, convRnd<FLOAT_T<FloatType> >(CARL_RND::D) );
            return result;
        }

        FLOAT_T<FloatType> sub_up  (FLOAT_T<FloatType> _lhs, FLOAT_T<FloatType> _rhs) // [-∞;+∞][-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _lhs->sub(*result, *_rhs, convRnd<FLOAT_T<FloatType> >(CARL_RND::U) );
            return result;
        }

        FLOAT_T<FloatType> mul_down(FLOAT_T<FloatType> _lhs, FLOAT_T<FloatType> _rhs) // [-∞;+∞][-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _lhs->mul(*result, *_rhs, convRnd<FLOAT_T<FloatType> >(CARL_RND::D) );
            return result;
        }

        FLOAT_T<FloatType> mul_up  (FLOAT_T<FloatType> _lhs, FLOAT_T<FloatType> _rhs) // [-∞;+∞][-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _lhs->mul(*result, *_rhs, convRnd<FLOAT_T<FloatType> >(CARL_RND::U) );
            return result;
        }

        FLOAT_T<FloatType> div_down(FLOAT_T<FloatType> _lhs, FLOAT_T<FloatType> _rhs) // [-∞;+∞]([-∞;+∞]-{0})
        {
            FLOAT_T<FloatType> result;
            _lhs->div(*result, *_rhs, convRnd<FLOAT_T<FloatType> >(CARL_RND::D) );
            return result;
        }

        FLOAT_T<FloatType> div_up  (FLOAT_T<FloatType> _lhs, FLOAT_T<FloatType> _rhs) // [-∞;+∞]([-∞;+∞]-{0})
        {
            FLOAT_T<FloatType> result;
            _lhs->div(*result, *_rhs, convRnd<FLOAT_T<FloatType> >(CARL_RND::U) );
            return result;
        }

        FLOAT_T<FloatType> sqrt_down(FLOAT_T<FloatType> _val)   // ]0;+∞]
        {
            FLOAT_T<FloatType> result;
            _val->sqrt(*result, convRnd<FLOAT_T<FloatType> >(CARL_RND::D) );
            return result;
        }
        FLOAT_T<FloatType> sqrt_up  (FLOAT_T<FloatType> _val)   // ]0;+∞]
        {
            FLOAT_T<FloatType> result;
            _val->sqrt(*result, convRnd<FLOAT_T<FloatType> >(CARL_RND::U) );
            return result;
        }
        FLOAT_T<FloatType> exp_down(FLOAT_T<FloatType> _val)    // [-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _val->exp(*result, convRnd<FLOAT_T<FloatType> >(CARL_RND::D) );
            return result;
        }
        FLOAT_T<FloatType> exp_up  (FLOAT_T<FloatType> _val)    // [-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _val->exp(*result, convRnd<FLOAT_T<FloatType> >(CARL_RND::U) );
            return result;
        }
        FLOAT_T<FloatType> log_down(FLOAT_T<FloatType> _val)    // ]0;+∞]
        {
            FLOAT_T<FloatType> result;
            _val->log(*result, convRnd<FLOAT_T<FloatType> >(CARL_RND::D) );
            return result;
        }
        FLOAT_T<FloatType> log_up  (FLOAT_T<FloatType> _val)    // ]0;+∞]
        {
            FLOAT_T<FloatType> result;
            _val->log(*result, convRnd<FLOAT_T<FloatType> >(CARL_RND::U) );
            return result;
        }
        FLOAT_T<FloatType> cos_down(FLOAT_T<FloatType> _val)    // [0;2π]
        {
            FLOAT_T<FloatType> result;
            _val->cos(*result, convRnd<FLOAT_T<FloatType> >(CARL_RND::D) );
            return result;
        }
        FLOAT_T<FloatType> cos_up  (FLOAT_T<FloatType> _val)    // [0;2π]
        {
            FLOAT_T<FloatType> result;
            _val->cos(*result, convRnd<FLOAT_T<FloatType> >(CARL_RND::U) );
            return result;
        }
        FLOAT_T<FloatType> tan_down(FLOAT_T<FloatType> _val)    // ]-π/2;π/2[
        {
            FLOAT_T<FloatType> result;
            _val->tan(*result, convRnd<FLOAT_T<FloatType> >(CARL_RND::D) );
            return result;
        }
        FLOAT_T<FloatType> tan_up  (FLOAT_T<FloatType> _val)    // ]-π/2;π/2[
        {
            FLOAT_T<FloatType> result;
            _val->tan(*result, convRnd<FLOAT_T<FloatType> >(CARL_RND::U) );
            return result;
        }
        FLOAT_T<FloatType> asin_down(FLOAT_T<FloatType> _val)   // [-1;1]
        {
            FLOAT_T<FloatType> result;
            _val->asin(*result, convRnd<FLOAT_T<FloatType> >(CARL_RND::D) );
            return result;
        }
        FLOAT_T<FloatType> asin_up  (FLOAT_T<FloatType> _val)   // [-1;1]
        {
            FLOAT_T<FloatType> result;
            _val->asin(*result, convRnd<FLOAT_T<FloatType> >(CARL_RND::U) );
            return result;
        }
        FLOAT_T<FloatType> acos_down(FLOAT_T<FloatType> _val)   // [-1;1]
        {
            FLOAT_T<FloatType> result;
            _val->acos(*result, convRnd<FLOAT_T<FloatType> >(CARL_RND::D) );
            return result;
        }
        FLOAT_T<FloatType> acos_up  (FLOAT_T<FloatType> _val)   // [-1;1]
        {
            FLOAT_T<FloatType> result;
            _val->acos(*result, convRnd<FLOAT_T<FloatType> >(CARL_RND::U) );
            return result;
        }
        FLOAT_T<FloatType> atan_down(FLOAT_T<FloatType> _val)   // [-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _val->atan(*result, convRnd<FLOAT_T<FloatType> >(CARL_RND::D) );
            return result;
        }
        FLOAT_T<FloatType> atan_up  (FLOAT_T<FloatType> _val)   // [-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _val->atan(*result, convRnd<FLOAT_T<FloatType> >(CARL_RND::U) );
            return result;
        }
        FLOAT_T<FloatType> sinh_down(FLOAT_T<FloatType> _val)   // [-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _val->sinh(*result, convRnd<FLOAT_T<FloatType> >(CARL_RND::D) );
            return result;
        }
        FLOAT_T<FloatType> sinh_up  (FLOAT_T<FloatType> _val)   // [-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _val->sinh(*result, convRnd<FLOAT_T<FloatType> >(CARL_RND::U) );
            return result;
        }
        FLOAT_T<FloatType> cosh_down(FLOAT_T<FloatType> _val)   // [-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _val->cosh(*result, convRnd<FLOAT_T<FloatType> >(CARL_RND::D) );
            return result;
        }
        FLOAT_T<FloatType> cosh_up  (FLOAT_T<FloatType> _val)   // [-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _val->cosh(*result, convRnd<FLOAT_T<FloatType> >(CARL_RND::U) );
            return result;
        }
        FLOAT_T<FloatType> tanh_down(FLOAT_T<FloatType> _val)   // [-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _val->tanh(*result, convRnd<FLOAT_T<FloatType> >(CARL_RND::D) );
            return result;
        }
        FLOAT_T<FloatType> tanh_up  (FLOAT_T<FloatType> _val)   // [-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _val->tanh(*result, convRnd<FLOAT_T<FloatType> >(CARL_RND::U) );
            return result;
        }
        FLOAT_T<FloatType> asinh_down(FLOAT_T<FloatType> _val)  // [-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _val->asinh(*result, convRnd<FLOAT_T<FloatType> >(CARL_RND::D) );
            return result;
        }
        FLOAT_T<FloatType> asinh_up  (FLOAT_T<FloatType> _val)  // [-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _val->asinh(*result, convRnd<FLOAT_T<FloatType> >(CARL_RND::U) );
            return result;
        }
        FLOAT_T<FloatType> acosh_down(FLOAT_T<FloatType> _val)  // [1;+∞]
        {
            FLOAT_T<FloatType> result;
            _val->acosh(*result, convRnd<FLOAT_T<FloatType> >(CARL_RND::D) );
            return result;
        }
        FLOAT_T<FloatType> acosh_up  (FLOAT_T<FloatType> _val)  // [1;+∞]
        {
            FLOAT_T<FloatType> result;
            _val->acosh(*result, convRnd<FLOAT_T<FloatType> >(CARL_RND::U) );
            return result;
        }
        FLOAT_T<FloatType> atanh_down(FLOAT_T<FloatType> _val)  // [-1;1]
        {
            FLOAT_T<FloatType> result;
            _val->atanh(*result, convRnd<FLOAT_T<FloatType> >(CARL_RND::D) );
            return result;
        }
        FLOAT_T<FloatType> atanh_up  (FLOAT_T<FloatType> _val)  // [-1;1] 
        {
            FLOAT_T<FloatType> result;
            _val->atanh(*result, convRnd<FLOAT_T<FloatType> >(CARL_RND::U) );
            return result;
        }
        FLOAT_T<FloatType> median(FLOAT_T<FloatType> _val1, FLOAT_T<FloatType> _val2)   // [-∞;+∞][-∞;+∞]
        {
			// @todo: Median <-> arithmetisches Mittel ???
            FLOAT_T<FloatType> result;
            result->add_assign(*_val1);
            result->add_assign(*_val2);
            result->div_assign(FLOAT_T<FloatType>(2));
            return result;
        }
        FLOAT_T<FloatType> int_down(FLOAT_T<FloatType> _val)    // [-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _val->floor(*result, convRnd<FLOAT_T<FloatType> >(CARL_RND::D) );
            return result;
        }
        FLOAT_T<FloatType> int_up  (FLOAT_T<FloatType> _val)    // [-∞;+∞]
        {
            FLOAT_T<FloatType> result;
            _val->ceil(*result, convRnd<FLOAT_T<FloatType> >(CARL_RND::U) );
            return result;
        }
        // conversion functions
        template<typename U>
        FLOAT_T<FloatType> conv_down(U _val)
        {
            return FLOAT_T<FloatType>(_val, convRnd<FLOAT_T<FloatType> >(CARL_RND::D) );
        }
        
        template<typename U>
        FLOAT_T<FloatType> conv_up(U _val)
        {
            return FLOAT_T<FloatType>(_val, convRnd<FLOAT_T<FloatType> >(CARL_RND::U) );
        }
        // unprotected rounding class
//    typedef ... unprotected_rounding;
    };
}