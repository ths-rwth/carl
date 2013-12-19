/* This file contains the definition of all arithmetic operations for the mpf_class float type.
 * File:   operations_mpfr.h
 * @author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 *
 * @since	2013-12-13
 * @version 2013-12-15
 */

#pragma once

#include <gmpxx.h>
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
    
    inline mpf_class abs(const mpf_class& _val)
    {
        mpf_class res;
        mpfr_abs(res, _val, MPFR_RNDN);
        return res;
    }

    inline int floor(const mpf_class& _val)
    {
        mpf_class result;
        mpfr_rint_floor(result, _val, MPFR_RNDN);
        int tmp = mpfr_integer_p(result);
        return tmp;
    }

    inline int ceil(const mpf_class& _val)
    {
        mpf_class result;
        mpfr_rint_ceil(result, _val, MPFR_RNDN);
        int _result = mpfr_integer_p(result);
        return _result;
    }

    inline mpf_class pow(const mpf_class& _val, unsigned _exp, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpf_class res;
        mpfr_pow_ui(res, _val, _exp, _rnd);
        return res;
    }

//    inline std::pair<mpf_class,mpf_class> sqrt(const mpf_class& _val)
//    {
//        mpf_class resA;
//        mpfr_init(resA);
//        mpf_class resB;
//        mpfr_init(resB);
//        mpfr_sqrt(resA, _val, MPFR_RNDD);
//        mpfr_sqrt(resB, _val, MPFR_RNDU);
//        return std::make_pair(resA, resB);
//    }

    inline mpf_class add( const mpf_class& _op1, const mpf_class& _op2, mpfr_rnd_t _rnd = MPFR_RNDN )
    {
        mpf_class result;
        mpfr_add(result, _op1, _op2, _rnd);
        return result;
    }

    mpf_class sub( const mpf_class& _op1, const mpf_class& _op2, mpfr_rnd_t _rnd = MPFR_RNDN )
    {
        mpf_class result;
        mpfr_sub(result, _op1 , _op2, _rnd);
        return result;
    }

    mpf_class mul( const mpf_class& _op1, const mpf_class& _op2, mpfr_rnd_t _rnd = MPFR_RNDN )
    {
        mpf_class result;
        mpfr_mul(result, _op1, _op2, _rnd);
        return result;
    }

    mpf_class div( const mpf_class& _op1, const mpf_class& _op2, mpfr_rnd_t _rnd = MPFR_RNDN )
    {
        assert( mpfr_zero_p(_op2) != 0 );
        mpf_class result;
        mpfr_div(result, _op1, _op2, _rnd);
        return result;
    }
    
    /**
     * Transcendental functions
     */
    
    mpf_class& exp_assign(mpf_class& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_exp(_val, _val , _rnd);
        return _val;
    }
    
    mpf_class& exp(const mpf_class& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpf_class res;
        mpfr_exp(res, _val, _rnd);
        return res;
    }
    
    mpf_class& sin_assign(mpf_class& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_sin(_val, _val, _rnd);
        return _val;
    }
    
    mpf_class& sin(const mpf_class& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpf_class res;
        mpfr_sin(res, _val, _rnd);
        return res;
    }
    
    mpf_class& cos_assign(mpf_class& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_cos(_val, _val, _rnd);
        return _val;
    }
    
    mpf_class& cos(const mpf_class& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpf_class res;
        mpfr_cos(res, _val, _rnd);
        return res;
    }
    
    mpf_class& log_assign(mpf_class& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_log(_val, _val, _rnd);
        return _val;
    }
    
    mpf_class& log(const mpf_class& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpf_class res;
        mpfr_log(res, _val, _rnd);
        return res;
    }
    
    mpf_class& tan_assign(mpf_class& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_tan(_val, _val, _rnd);
        return _val;
    }
    
    mpf_class& tan(const mpf_class& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpf_class res;
        mpfr_tan(res, _val, _rnd);
        return res;
    }
    
    mpf_class& asin_assign(mpf_class& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_asin(_val, _val, _rnd);
        return _val;
    }
    
    mpf_class& asin(const mpf_class& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpf_class res;
        mpfr_asin(res, _val, _rnd);
        return res;
    }
    
    mpf_class& acos_assign(mpf_class& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_acos(_val, _val, _rnd);
        return _val;
    }
    
    mpf_class& acos(const mpf_class& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpf_class res;
        mpfr_acos(res, _val, _rnd);
        return res;
    }
    
    mpf_class& atan_assign(mpf_class& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_atan(_val, _val, _rnd);
        return _val;
    }
    
    mpf_class& atan(const mpf_class& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpf_class res;
        mpfr_atan(res, _val, _rnd);
        return res;
    }
    
    mpf_class& sinh_assign(mpf_class& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_sinh(_val, _val, _rnd);
        return _val;
    }
    
    mpf_class& sinh(const mpf_class& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpf_class res;
        mpfr_sinh(res, _val, _rnd);
        return res;
    }
    
    mpf_class& cosh_assign(mpf_class& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_cosh(_val, _val, _rnd);
        return _val;
    }
    
    mpf_class& cosh(const mpf_class& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpf_class res;
        mpfr_cosh(res, _val, _rnd);
        return res;
    }
    
    mpf_class& tanh_assign(mpf_class& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_tanh(_val, _val, _rnd);
        return _val;
    }
    
    mpf_class& tanh(const mpf_class& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpf_class res;
        mpfr_tanh(res, _val, _rnd);
        return res;
    }
    
    mpf_class& asinh_assign(mpf_class& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_asinh(_val, _val, _rnd);
        return _val;
    }
    
    mpf_class& asinh(const mpf_class& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpf_class res;
        mpfr_asinh(res, _val, _rnd);
        return res;
    }
    
    mpf_class& acosh_assign(mpf_class& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_acosh(_val, _val, _rnd);
        return _val;
    }
    
    mpf_class& acosh(const mpf_class& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpf_class res;
        mpfr_acosh(res, _val, _rnd);
        return res;
    }
    
    mpf_class& atanh_assign(mpf_class& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_atanh(_val, _val, _rnd);
        return _val;
    }
    
    mpf_class& atanh(mpf_class& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpf_class res;
        mpfr_atanh(res, _val, _rnd);
        return res;
    }
    
    
    /**
     * Basic arithmetic operators
     */
 /*
    inline mpf_class operator +(const mpf_class& _lhs, const mpf_class& _rhs)
    {
        mpf_class res;
        mpfr_init(res);
        mpfr_add(res, _lhs, _rhs, MPFR_RNDN);
        return res;
    }
    
    inline mpf_class operator -(const mpf_class& _lhs, const mpf_class& _rhs)
    {
        mpf_class res;
        mpfr_init(res);
        mpfr_sub(res, _lhs, _rhs, MPFR_RNDN);
        return res;
    }
    
    inline mpf_class operator *(const mpf_class& _lhs, const mpf_class& _rhs)
    {
        mpf_class res;
        mpfr_init(res);
        mpfr_mul(res, _lhs, _rhs, MPFR_RNDN);
        return res;
    }
    
    inline mpf_class operator /(const mpf_class& _lhs, const mpf_class& _rhs)
    {
        // TODO: mpfr_div results in infty when dividing by zero, although this should not be defined.
        mpf_class res;
        mpfr_init(res);
        mpfr_div(res, _lhs, _rhs, MPFR_RNDN);
        return res;
    }
    
    inline mpf_class& operator +=(const mpf_class& _rhs)
    {
        mpfr_add(*this, *this, _rhs, MPFR_RNDN);
        return *this;
    }
    
    inline mpf_class& operator -=(const mpf_class& _rhs)
    {
        mpfr_sub( *this, *this, _rhs, MPFR_RNDN);
        return *this;
    }
    
    inline mpf_class& operator *=(const mpf_class& _rhs)
    {
        mpfr_mul(*this, *this, _rhs, MPFR_RNDN);
        return *this;
    }
    
    inline mpf_class& operator /=(const mpf_class& _rhs)
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
    inline bool operator== (const mpf_class& _lhs, const int _rhs)
    {
        return mpfr_cmp_si(_lhs, _rhs) == 0;
    }
    
    inline bool operator== (const int _lhs, const mpf_class& _rhs)
    {
        return _rhs == _lhs;
    }
    
    inline bool operator== (const mpf_class& _lhs, const double _rhs)
    {
        return mpfr_cmp_d(_lhs,_rhs) == 0;
    }
    
    inline bool operator== (const double _lhs, const mpf_class& _rhs)
    {
        return _rhs == _lhs;
    }
    
    inline bool operator== (const mpf_class& _lhs, const float _rhs)
    {
        return mpfr_cmp_d(_lhs, _rhs);
    }
    
    inline bool operator== (const float _lhs, const mpf_class& _rhs)
    {
        return _rhs == _lhs;
    }

  */  
}