/* 
 * This file contains the rounding policies needed from the boost interval class
 * for exact values using operator overloading.
 * 
 * @file   rounding.h
 * @author Benedikt Seidl
 * @author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 *
 * @since   2014-01-28
 * @version 2014-01-30
 */

#pragma once

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
            return _lhs + _rhs;
        }

        Number add_up(Number _lhs, Number _rhs) // [-∞;+∞][-∞;+∞]
        {
            return _lhs + _rhs;
        }

        Number sub_down(Number _lhs, Number _rhs) // [-∞;+∞][-∞;+∞]
        {
            return _lhs - _rhs;
        }

        Number sub_up  (Number _lhs, Number _rhs) // [-∞;+∞][-∞;+∞]
        {
            return _lhs - _rhs;
        }

        Number mul_down(Number _lhs, Number _rhs) // [-∞;+∞][-∞;+∞]
        {
            return _lhs * _rhs;
        }

        Number mul_up  (Number _lhs, Number _rhs) // [-∞;+∞][-∞;+∞]
        {
            return _lhs * _rhs;
        }

        Number div_down(Number _lhs, Number _rhs) // [-∞;+∞]([-∞;+∞]-{0})
        {
            return _lhs / _rhs;
        }

        Number div_up  (Number _lhs, Number _rhs) // [-∞;+∞]([-∞;+∞]-{0})
        {
            return _lhs / _rhs;
        }

        Number sqrt_down(Number _val)   // ]0;+∞]
        {
            if(_val <= 0)
                return 0;
            return sqrt(_val);
        }
        Number sqrt_up  (Number _val)   // ]0;+∞]
        {
            if(_val <= 0)
                return 0;
            return sqrt(_val);
        }
        Number exp_down(Number _val)    // [-∞;+∞]
        {
            return exp(_val);
        }
        Number exp_up  (Number _val)    // [-∞;+∞]
        {
            return exp(_val);
        }
        Number log_down(Number _val)    // ]0;+∞]
        {
            return log(_val);
        }
        Number log_up  (Number _val)    // ]0;+∞]
        {
            return log(_val);
        }
        Number sin_up(Number _val)
        {
            return sin(_val);
        }
        Number sin_down(Number _val)
        {
            return sin(_val);
        }
        Number cos_down(Number _val)    // [0;2π]
        {
            return cos(_val);
        }
        Number cos_up  (Number _val)    // [0;2π]
        {
            return cos(_val);
        }
        Number tan_down(Number _val)    // ]-π/2;π/2[
        {
            return tan(_val);
        }
        Number tan_up  (Number _val)    // ]-π/2;π/2[
        {
            return tan(_val);
        }
        Number asin_down(Number _val)   // [-1;1]
        {
            return asin(_val);
        }
        Number asin_up  (Number _val)   // [-1;1]
        {
            return asin(_val);
        }
        Number acos_down(Number _val)   // [-1;1]
        {
            return acos(_val);
        }
        Number acos_up  (Number _val)   // [-1;1]
        {
            return acos(_val);
        }
        Number atan_down(Number _val)   // [-∞;+∞]
        {
            return atan(_val);
        }
        Number atan_up  (Number _val)   // [-∞;+∞]
        {
            return atan(_val);
        }
        Number sinh_down(Number _val)   // [-∞;+∞]
        {
            return sinh(_val);
        }
        Number sinh_up  (Number _val)   // [-∞;+∞]
        {
            return sinh(_val);
        }
        Number cosh_down(Number _val)   // [-∞;+∞]
        {
            return cosh(_val);
        }
        Number cosh_up  (Number _val)   // [-∞;+∞]
        {
            return cosh(_val);
        }
        Number tanh_down(Number _val)   // [-∞;+∞]
        {
            return tanh(_val);
        }
        Number tanh_up  (Number _val)   // [-∞;+∞]
        {
            return tanh(_val);
        }
        Number asinh_down(Number _val)  // [-∞;+∞]
        {
            return asinh(_val);
        }
        Number asinh_up  (Number _val)  // [-∞;+∞]
        {
            return asinh(_val);
        }
        Number acosh_down(Number _val)  // [1;+∞]
        {
            return acosh(_val);
        }
        Number acosh_up  (Number _val)  // [1;+∞]
        {
            return acosh(_val);
        }
        Number atanh_down(Number _val)  // [-1;1]
        {
            return atanh(_val);
        }
        Number atanh_up  (Number _val)  // [-1;1] 
        {
            return atanh(_val);
        }
        Number median(Number _val1, Number _val2)   // [-∞;+∞][-∞;+∞]
        {
            return (_val1 + _val2) / 2;
        }
        Number int_down(Number _val)    // [-∞;+∞]
        {
            return floor(_val);
        }
        Number int_up  (Number _val)    // [-∞;+∞]
        {
            return ceil(_val);
        }
        // conversion functions
        template<typename U>
        Number conv_down(U _val)
        {
            return Number(_val);
        }
        
        template<typename U>
        Number conv_up(U _val)
        {
            return Number(_val);
        }
        // unprotected rounding class
//    typedef ... unprotected_rounding;
    };
}

#include "rounding_float_t.tpp"