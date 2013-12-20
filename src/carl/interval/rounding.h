/* 
 * This file contains the rounding policies needed from the boost interval class for the general
 * Number type used in carl.
 * 
 * @file   rounding.h
 * @author Stefan Schupp
 *
 * @since	2013-12-05
 * @version 2013-12-20
 */

#pragma once
#include "../numbers/roundingConversion.h"
#include "../numbers/operations_mpfr.h"
#include <mpfr.h>

namespace carl
{
    template<typename Number>
    struct rounding 
    {
        // default constructor, destructor
//        rounding();
//        ~rounding();
        // mathematical operations
        Number add_down(Number _lhs, Number _rhs) // [-∞;+∞][-∞;+∞]
        {
            Number result;
			result = add( _lhs, _rhs, convRnd<Number>(CARL_RND::CARL_RNDD) );
            return result;
        }

        Number add_up(Number _lhs, Number _rhs) // [-∞;+∞][-∞;+∞]
        {
            Number result;
            _lhs->add(*result, *_rhs, convRnd<Number>(CARL_RND::CARL_RNDU) );
            return result;
        }

        Number sub_down(Number _lhs, Number _rhs) // [-∞;+∞][-∞;+∞]
        {
            Number result;
            _lhs->sub(*result, *_rhs, convRnd<Number>(CARL_RND::CARL_RNDD) );
            return result;
        }

        Number sub_up  (Number _lhs, Number _rhs) // [-∞;+∞][-∞;+∞]
        {
            Number result;
            _lhs->sub(*result, *_rhs, convRnd<Number>(CARL_RND::CARL_RNDU) );
            return result;
        }

        Number mul_down(Number _lhs, Number _rhs) // [-∞;+∞][-∞;+∞]
        {
            Number result;
            _lhs->mul(*result, *_rhs, convRnd<Number>(CARL_RND::CARL_RNDD) );
            return result;
        }

        Number mul_up  (Number _lhs, Number _rhs) // [-∞;+∞][-∞;+∞]
        {
            Number result;
            _lhs->mul(*result, *_rhs, convRnd<Number>(CARL_RND::CARL_RNDU) );
            return result;
        }

        Number div_down(Number _lhs, Number _rhs) // [-∞;+∞]([-∞;+∞]-{0})
        {
            Number result;
            _lhs->div(*result, *_rhs, convRnd<Number>(CARL_RND::CARL_RNDD) );
            return result;
        }

        Number div_up  (Number _lhs, Number _rhs) // [-∞;+∞]([-∞;+∞]-{0})
        {
            Number result;
            _lhs->div(*result, *_rhs, convRnd<Number>(CARL_RND::CARL_RNDU) );
            return result;
        }

        Number sqrt_down(Number _val)   // ]0;+∞]
        {
            Number result;
            _val->sqrt(*result, convRnd<Number>(CARL_RND::CARL_RNDD) );
            return result;
        }
        Number sqrt_up  (Number _val)   // ]0;+∞]
        {
            Number result;
            _val->sqrt(*result, convRnd<Number>(CARL_RND::CARL_RNDU) );
            return result;
        }
        Number exp_down(Number _val)    // [-∞;+∞]
        {
            Number result;
            _val->exp(*result, convRnd<Number>(CARL_RND::CARL_RNDD) );
            return result;
        }
        Number exp_up  (Number _val)    // [-∞;+∞]
        {
            Number result;
            _val->exp(*result, convRnd<Number>(CARL_RND::CARL_RNDU) );
            return result;
        }
        Number log_down(Number _val)    // ]0;+∞]
        {
            Number result;
            _val->log(*result, convRnd<Number>(CARL_RND::CARL_RNDD) );
            return result;
        }
        Number log_up  (Number _val)    // ]0;+∞]
        {
            Number result;
            _val->log(*result, convRnd<Number>(CARL_RND::CARL_RNDU) );
            return result;
        }
        Number cos_down(Number _val)    // [0;2π]
        {
            Number result;
            _val->cos(*result, convRnd<Number>(CARL_RND::CARL_RNDD) );
            return result;
        }
        Number cos_up  (Number _val)    // [0;2π]
        {
            Number result;
            _val->cos(*result, convRnd<Number>(CARL_RND::CARL_RNDU) );
            return result;
        }
        Number tan_down(Number _val)    // ]-π/2;π/2[
        {
            Number result;
            _val->tan(*result, convRnd<Number>(CARL_RND::CARL_RNDD) );
            return result;
        }
        Number tan_up  (Number _val)    // ]-π/2;π/2[
        {
            Number result;
            _val->tan(*result, convRnd<Number>(CARL_RND::CARL_RNDU) );
            return result;
        }
        Number asin_down(Number _val)   // [-1;1]
        {
            Number result;
            _val->asin(*result, convRnd<Number>(CARL_RND::CARL_RNDD) );
            return result;
        }
        Number asin_up  (Number _val)   // [-1;1]
        {
            Number result;
            _val->asin(*result, convRnd<Number>(CARL_RND::CARL_RNDU) );
            return result;
        }
        Number acos_down(Number _val)   // [-1;1]
        {
            Number result;
            _val->acos(*result, convRnd<Number>(CARL_RND::CARL_RNDD) );
            return result;
        }
        Number acos_up  (Number _val)   // [-1;1]
        {
            Number result;
            _val->acos(*result, convRnd<Number>(CARL_RND::CARL_RNDU) );
            return result;
        }
        Number atan_down(Number _val)   // [-∞;+∞]
        {
            Number result;
            _val->atan(*result, convRnd<Number>(CARL_RND::CARL_RNDD) );
            return result;
        }
        Number atan_up  (Number _val)   // [-∞;+∞]
        {
            Number result;
            _val->atan(*result, convRnd<Number>(CARL_RND::CARL_RNDU) );
            return result;
        }
        Number sinh_down(Number _val)   // [-∞;+∞]
        {
            Number result;
            _val->sinh(*result, convRnd<Number>(CARL_RND::CARL_RNDD) );
            return result;
        }
        Number sinh_up  (Number _val)   // [-∞;+∞]
        {
            Number result;
            _val->sinh(*result, convRnd<Number>(CARL_RND::CARL_RNDU) );
            return result;
        }
        Number cosh_down(Number _val)   // [-∞;+∞]
        {
            Number result;
            _val->cosh(*result, convRnd<Number>(CARL_RND::CARL_RNDD) );
            return result;
        }
        Number cosh_up  (Number _val)   // [-∞;+∞]
        {
            Number result;
            _val->cosh(*result, convRnd<Number>(CARL_RND::CARL_RNDU) );
            return result;
        }
        Number tanh_down(Number _val)   // [-∞;+∞]
        {
            Number result;
            _val->tanh(*result, convRnd<Number>(CARL_RND::CARL_RNDD) );
            return result;
        }
        Number tanh_up  (Number _val)   // [-∞;+∞]
        {
            Number result;
            _val->tanh(*result, convRnd<Number>(CARL_RND::CARL_RNDU) );
            return result;
        }
        Number asinh_down(Number _val)  // [-∞;+∞]
        {
            Number result;
            _val->asinh(*result, convRnd<Number>(CARL_RND::CARL_RNDD) );
            return result;
        }
        Number asinh_up  (Number _val)  // [-∞;+∞]
        {
            Number result;
            _val->asinh(*result, convRnd<Number>(CARL_RND::CARL_RNDU) );
            return result;
        }
        Number acosh_down(Number _val)  // [1;+∞]
        {
            Number result;
            _val->acosh(*result, convRnd<Number>(CARL_RND::CARL_RNDD) );
            return result;
        }
        Number acosh_up  (Number _val)  // [1;+∞]
        {
            Number result;
            _val->acosh(*result, convRnd<Number>(CARL_RND::CARL_RNDU) );
            return result;
        }
        Number atanh_down(Number _val)  // [-1;1]
        {
            Number result;
            _val->atanh(*result, convRnd<Number>(CARL_RND::CARL_RNDD) );
            return result;
        }
        Number atanh_up  (Number _val)  // [-1;1] 
        {
            Number result;
            _val->atanh(*result, convRnd<Number>(CARL_RND::CARL_RNDU) );
            return result;
        }
        Number median(Number _val1, Number _val2)   // [-∞;+∞][-∞;+∞]
        {
            Number result;
            result->add_assign(*_val1);
            result->add_assign(*_val2);
            result->div_assign(Number(2));
            return result;
        }
        Number int_down(Number _val)    // [-∞;+∞]
        {
            Number result;
            _val->floor(*result, convRnd<Number>(CARL_RND::CARL_RNDD) );
            return result;
        }
        Number int_up  (Number _val)    // [-∞;+∞]
        {
            Number result;
            _val->ceil(*result, convRnd<Number>(CARL_RND::CARL_RNDU) );
            return result;
        }
        // conversion functions
        template<typename U>
        Number conv_down(U _val)
        {
            return Number(_val, convRnd<Number>(CARL_RND::CARL_RNDD) );
        }
        
        template<typename U>
        Number conv_up(U _val)
        {
            return Number(_val, convRnd<Number>(CARL_RND::CARL_RNDU) );
        }
        // unprotected rounding class
//    typedef ... unprotected_rounding;
    };
}
