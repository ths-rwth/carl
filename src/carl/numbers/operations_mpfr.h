/* This file contains the definition of all arithmetic operations for the carl_mpfr float type.
 * File:   operations_mpfr.h
 * @author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 *
 * @since	2013-12-13
 * @version 2013-12-15
 */

#pragma once

#include <mpfr.h>
#include "floatTypes/mpfr_wrapper.h"

namespace carl
{
    /**
     * Informational functions
     * 
     * The following functions return informations about the given numbers.
     */
    /*
    inline const Integer getNum(const Number&);

    inline const Integer getDenom(const Number&);
    
    inline bool isInteger(const Number&);
    */
    
    /*
    template<typename C>
    inline bool isInteger(const GFNumber<C>&) {
            return true;
    }
    */
    /**
     * Conversion functions
     * 
     * The following function convert types to other types.
     */
    /*
    inline double toDouble(const Number&);

    template<typename Integer> 
    inline Integer toInt(const Number&);

    inline Number rationalize(double);
    */

    /**
     * Basic arithmetic operations
     * 
     * The following functions implement simple operations on the given numbers.
     */
    
    inline carl_mpfr abs(const carl_mpfr& _val)
    {
        carl_mpfr res;
        mpfr_abs(res.val, _val.val, MPFR_RNDN);
        return res;
    }

    inline int floor(const carl_mpfr& _val)
    {
        carl_mpfr result;
        mpfr_rint_floor(result.val, _val.val, MPFR_RNDN);
        int tmp = mpfr_integer_p(result.val);
        return tmp;
    }

    inline int ceil(const carl_mpfr& _val)
    {
        carl_mpfr result;
        mpfr_rint_ceil(result.val, _val.val, MPFR_RNDN);
        int _result = mpfr_integer_p(result.val);
        return _result;
    }

    inline carl_mpfr pow(const carl_mpfr& _val, unsigned _exp, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        carl_mpfr res;
        mpfr_pow_ui(res.val, _val.val, _exp, _rnd);
        return res;
    }

//    inline std::pair<carl_mpfr,carl_mpfr> sqrt(const carl_mpfr& _val)
//    {
//        carl_mpfr resA;
//        mpfr_init(resA);
//        carl_mpfr resB;
//        mpfr_init(resB);
//        mpfr_sqrt(resA, _val, MPFR_RNDD);
//        mpfr_sqrt(resB, _val, MPFR_RNDU);
//        return std::make_pair(resA, resB);
//    }

    inline carl_mpfr add( const carl_mpfr& _op1, const carl_mpfr& _op2, mpfr_rnd_t _rnd = MPFR_RNDN )
    {
        carl_mpfr result;
        mpfr_add(result.val, _op1.val, _op2.val, _rnd);
        return result;
    }

    carl_mpfr sub( const carl_mpfr& _op1, const carl_mpfr& _op2, mpfr_rnd_t _rnd = MPFR_RNDN )
    {
        carl_mpfr result;
        mpfr_sub(result.val, _op1.val , _op2.val, _rnd);
        return result;
    }

    carl_mpfr mul( const carl_mpfr& _op1, const carl_mpfr& _op2, mpfr_rnd_t _rnd = MPFR_RNDN )
    {
        carl_mpfr result;
        mpfr_mul(result.val, _op1.val, _op2.val, _rnd);
        return result;
    }

    carl_mpfr div( const carl_mpfr& _op1, const carl_mpfr& _op2, mpfr_rnd_t _rnd = MPFR_RNDN )
    {
        assert( mpfr_zero_p(_op2.val) != 0 );
        carl_mpfr result;
        mpfr_div(result.val, _op1.val, _op2.val, _rnd);
        return result;
    }
    
    /**
     * Transcendental functions
     */
    
    carl_mpfr& exp_assign(carl_mpfr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_exp(_val.val, _val.val , _rnd);
        return _val;
    }
    
    carl_mpfr& exp(const carl_mpfr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        carl_mpfr res;
        mpfr_exp(res.val, _val.val, _rnd);
        return res;
    }
    
    carl_mpfr& sin_assign(carl_mpfr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_sin(_val.val, _val.val, _rnd);
        return _val;
    }
    
    carl_mpfr& sin(const carl_mpfr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        carl_mpfr res;
        mpfr_sin(res.val, _val.val, _rnd);
        return res;
    }
    
    carl_mpfr& cos_assign(carl_mpfr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_cos(_val.val, _val.val, _rnd);
        return _val;
    }
    
    carl_mpfr& cos(const carl_mpfr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        carl_mpfr res;
        mpfr_cos(res.val, _val.val, _rnd);
        return res;
    }
    
    carl_mpfr& log_assign(carl_mpfr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_log(_val.val, _val.val, _rnd);
        return _val;
    }
    
    carl_mpfr& log(const carl_mpfr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        carl_mpfr res;
        mpfr_log(res.val, _val.val, _rnd);
        return res;
    }
    
    carl_mpfr& tan_assign(carl_mpfr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_tan(_val.val, _val.val, _rnd);
        return _val;
    }
    
    carl_mpfr& tan(const carl_mpfr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        carl_mpfr res;
        mpfr_tan(res.val, _val.val, _rnd);
        return res;
    }
    
    carl_mpfr& asin_assign(carl_mpfr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_asin(_val.val, _val.val, _rnd);
        return _val;
    }
    
    carl_mpfr& asin(const carl_mpfr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        carl_mpfr res;
        mpfr_asin(res.val, _val.val, _rnd);
        return res;
    }
    
    carl_mpfr& acos_assign(carl_mpfr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_acos(_val.val, _val.val, _rnd);
        return _val;
    }
    
    carl_mpfr& acos(const carl_mpfr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        carl_mpfr res;
        mpfr_acos(res.val, _val.val, _rnd);
        return res;
    }
    
    carl_mpfr& atan_assign(carl_mpfr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_atan(_val.val, _val.val, _rnd);
        return _val;
    }
    
    carl_mpfr& atan(const carl_mpfr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        carl_mpfr res;
        mpfr_atan(res.val, _val.val, _rnd);
        return res;
    }
    
    carl_mpfr& sinh_assign(carl_mpfr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_sinh(_val.val, _val.val, _rnd);
        return _val;
    }
    
    carl_mpfr& sinh(const carl_mpfr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        carl_mpfr res;
        mpfr_sinh(res.val, _val.val, _rnd);
        return res;
    }
    
    carl_mpfr& cosh_assign(carl_mpfr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_cosh(_val.val, _val.val, _rnd);
        return _val;
    }
    
    carl_mpfr& cosh(const carl_mpfr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        carl_mpfr res;
        mpfr_cosh(res.val, _val.val, _rnd);
        return res;
    }
    
    carl_mpfr& tanh_assign(carl_mpfr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_tanh(_val.val, _val.val, _rnd);
        return _val;
    }
    
    carl_mpfr& tanh(const carl_mpfr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        carl_mpfr res;
        mpfr_tanh(res.val, _val.val, _rnd);
        return res;
    }
    
    carl_mpfr& asinh_assign(carl_mpfr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_asinh(_val.val, _val.val, _rnd);
        return _val;
    }
    
    carl_mpfr& asinh(const carl_mpfr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        carl_mpfr res;
        mpfr_asinh(res.val, _val.val, _rnd);
        return res;
    }
    
    carl_mpfr& acosh_assign(carl_mpfr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_acosh(_val.val, _val.val, _rnd);
        return _val;
    }
    
    carl_mpfr& acosh(const carl_mpfr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        carl_mpfr res;
        mpfr_acosh(res.val, _val.val, _rnd);
        return res;
    }
    
    carl_mpfr& atanh_assign(carl_mpfr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_atanh(_val.val, _val.val, _rnd);
        return _val;
    }
    
    carl_mpfr& atanh(carl_mpfr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        carl_mpfr res;
        mpfr_atanh(res.val, _val.val, _rnd);
        return res;
    }
    
    
    /**
     * Basic arithmetic operators
     */
 /*
    inline carl_mpfr operator +(const carl_mpfr& _lhs, const carl_mpfr& _rhs)
    {
        carl_mpfr res;
        mpfr_init(res);
        mpfr_add(res, _lhs, _rhs, MPFR_RNDN);
        return res;
    }
    
    inline carl_mpfr operator -(const carl_mpfr& _lhs, const carl_mpfr& _rhs)
    {
        carl_mpfr res;
        mpfr_init(res);
        mpfr_sub(res, _lhs, _rhs, MPFR_RNDN);
        return res;
    }
    
    inline carl_mpfr operator *(const carl_mpfr& _lhs, const carl_mpfr& _rhs)
    {
        carl_mpfr res;
        mpfr_init(res);
        mpfr_mul(res, _lhs, _rhs, MPFR_RNDN);
        return res;
    }
    
    inline carl_mpfr operator /(const carl_mpfr& _lhs, const carl_mpfr& _rhs)
    {
        // TODO: mpfr_div results in infty when dividing by zero, although this should not be defined.
        carl_mpfr res;
        mpfr_init(res);
        mpfr_div(res, _lhs, _rhs, MPFR_RNDN);
        return res;
    }
    
    inline carl_mpfr& operator +=(const carl_mpfr& _rhs)
    {
        mpfr_add(*this, *this, _rhs, MPFR_RNDN);
        return *this;
    }
    
    inline carl_mpfr& operator -=(const carl_mpfr& _rhs)
    {
        mpfr_sub( *this, *this, _rhs, MPFR_RNDN);
        return *this;
    }
    
    inline carl_mpfr& operator *=(const carl_mpfr& _rhs)
    {
        mpfr_mul(*this, *this, _rhs, MPFR_RNDN);
        return *this;
    }
    
    inline carl_mpfr& operator /=(const carl_mpfr& _rhs)
    {
        // TODO: mpfr_div results in infty when dividing by zero, although this should not be defined.
        mpfr_div(*this, *this, _rhs, MPFR_RNDN);
        return *this;
    }
   */
    /*
     * Comparison operators
     */
/*
    inline bool operator== (const carl_mpfr& _lhs, const int _rhs)
    {
        return mpfr_cmp_si(_lhs, _rhs) == 0;
    }
    
    inline bool operator== (const int _lhs, const carl_mpfr& _rhs)
    {
        return _rhs == _lhs;
    }
    
    inline bool operator== (const carl_mpfr& _lhs, const double _rhs)
    {
        return mpfr_cmp_d(_lhs,_rhs) == 0;
    }
    
    inline bool operator== (const double _lhs, const carl_mpfr& _rhs)
    {
        return _rhs == _lhs;
    }
    
    inline bool operator== (const carl_mpfr& _lhs, const float _rhs)
    {
        return mpfr_cmp_d(_lhs, _rhs);
    }
    
    inline bool operator== (const float _lhs, const carl_mpfr& _rhs)
    {
        return _rhs == _lhs;
    }

  */  
}