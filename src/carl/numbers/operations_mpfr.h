/* This file contains the definition of all arithmetic operations for the mpfr_ptr float type.
 * File:   operations_mpfr.h
 * @author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 *
 * @since	2013-12-13
 * @version 2013-12-15
 */

#pragma once

#include <gmpxx.h>

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
    
    inline mpfr_ptr abs(const mpfr_ptr& _val)
    {
        mpfr_ptr res;
        mpfr_abs(res, _val, MPFR_RNDN);
        return res;
    }

    inline int floor(const mpfr_ptr& _val)
    {
        mpfr_ptr result;
        mpfr_rint_floor(result, _val, MPFR_RNDN);
        int tmp = mpfr_integer_p(result);
        return tmp;
    }

    inline int ceil(const mpfr_ptr& _val)
    {
        mpfr_ptr result;
        mpfr_rint_ceil(result, _val, MPFR_RNDN);
        int _result = mpfr_integer_p(result);
        return _result;
    }

    inline mpfr_ptr pow(const mpfr_ptr& _val, unsigned _exp, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_ptr res;
        mpfr_pow_ui(res, _val, _exp, _rnd);
        return res;
    }

//    inline std::pair<mpfr_ptr,mpfr_ptr> sqrt(const mpfr_ptr& _val)
//    {
//        mpfr_ptr resA;
//        mpfr_init(resA);
//        mpfr_ptr resB;
//        mpfr_init(resB);
//        mpfr_sqrt(resA, _val, MPFR_RNDD);
//        mpfr_sqrt(resB, _val, MPFR_RNDU);
//        return std::make_pair(resA, resB);
//    }

    inline mpfr_ptr add( const mpfr_ptr& _op1, const mpfr_ptr& _op2, mpfr_rnd_t _rnd = MPFR_RNDN )
    {
        mpfr_ptr result;
        mpfr_add(result, _op1, _op2, _rnd);
        return result;
    }

    mpfr_ptr sub( const mpfr_ptr& _op1, const mpfr_ptr& _op2, mpfr_rnd_t _rnd = MPFR_RNDN )
    {
        mpfr_ptr result;
        mpfr_sub(result, _op1 , _op2, _rnd);
        return result;
    }

    mpfr_ptr mul( const mpfr_ptr& _op1, const mpfr_ptr& _op2, mpfr_rnd_t _rnd = MPFR_RNDN )
    {
        mpfr_ptr result;
        mpfr_mul(result, _op1, _op2, _rnd);
        return result;
    }

    mpfr_ptr div( const mpfr_ptr& _op1, const mpfr_ptr& _op2, mpfr_rnd_t _rnd = MPFR_RNDN )
    {
        assert( mpfr_zero_p(_op2) != 0 );
        mpfr_ptr result;
        mpfr_div(result, _op1, _op2, _rnd);
        return result;
    }
    
    /**
     * Transcendental functions
     */
    
    mpfr_ptr& exp_assign(mpfr_ptr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_exp(_val, _val , _rnd);
        return _val;
    }
    
    mpfr_ptr& exp(const mpfr_ptr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_ptr res;
        mpfr_exp(res, _val, _rnd);
        return res;
    }
    
    mpfr_ptr& sin_assign(mpfr_ptr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_sin(_val, _val, _rnd);
        return _val;
    }
    
    mpfr_ptr& sin(const mpfr_ptr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_ptr res;
        mpfr_sin(res, _val, _rnd);
        return res;
    }
    
    mpfr_ptr& cos_assign(mpfr_ptr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_cos(_val, _val, _rnd);
        return _val;
    }
    
    mpfr_ptr& cos(const mpfr_ptr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_ptr res;
        mpfr_cos(res, _val, _rnd);
        return res;
    }
    
    mpfr_ptr& log_assign(mpfr_ptr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_log(_val, _val, _rnd);
        return _val;
    }
    
    mpfr_ptr& log(const mpfr_ptr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_ptr res;
        mpfr_log(res, _val, _rnd);
        return res;
    }
    
    mpfr_ptr& tan_assign(mpfr_ptr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_tan(_val, _val, _rnd);
        return _val;
    }
    
    mpfr_ptr& tan(const mpfr_ptr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_ptr res;
        mpfr_tan(res, _val, _rnd);
        return res;
    }
    
    mpfr_ptr& asin_assign(mpfr_ptr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_asin(_val, _val, _rnd);
        return _val;
    }
    
    mpfr_ptr& asin(const mpfr_ptr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_ptr res;
        mpfr_asin(res, _val, _rnd);
        return res;
    }
    
    mpfr_ptr& acos_assign(mpfr_ptr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_acos(_val, _val, _rnd);
        return _val;
    }
    
    mpfr_ptr& acos(const mpfr_ptr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_ptr res;
        mpfr_acos(res, _val, _rnd);
        return res;
    }
    
    mpfr_ptr& atan_assign(mpfr_ptr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_atan(_val, _val, _rnd);
        return _val;
    }
    
    mpfr_ptr& atan(const mpfr_ptr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_ptr res;
        mpfr_atan(res, _val, _rnd);
        return res;
    }
    
    mpfr_ptr& sinh_assign(mpfr_ptr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_sinh(_val, _val, _rnd);
        return _val;
    }
    
    mpfr_ptr& sinh(const mpfr_ptr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_ptr res;
        mpfr_sinh(res, _val, _rnd);
        return res;
    }
    
    mpfr_ptr& cosh_assign(mpfr_ptr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_cosh(_val, _val, _rnd);
        return _val;
    }
    
    mpfr_ptr& cosh(const mpfr_ptr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_ptr res;
        mpfr_cosh(res, _val, _rnd);
        return res;
    }
    
    mpfr_ptr& tanh_assign(mpfr_ptr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_tanh(_val, _val, _rnd);
        return _val;
    }
    
    mpfr_ptr& tanh(const mpfr_ptr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_ptr res;
        mpfr_tanh(res, _val, _rnd);
        return res;
    }
    
    mpfr_ptr& asinh_assign(mpfr_ptr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_asinh(_val, _val, _rnd);
        return _val;
    }
    
    mpfr_ptr& asinh(const mpfr_ptr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_ptr res;
        mpfr_asinh(res, _val, _rnd);
        return res;
    }
    
    mpfr_ptr& acosh_assign(mpfr_ptr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_acosh(_val, _val, _rnd);
        return _val;
    }
    
    mpfr_ptr& acosh(const mpfr_ptr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_ptr res;
        mpfr_acosh(res, _val, _rnd);
        return res;
    }
    
    mpfr_ptr& atanh_assign(mpfr_ptr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_atanh(_val, _val, _rnd);
        return _val;
    }
    
    mpfr_ptr& atanh(mpfr_ptr& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_ptr res;
        mpfr_atanh(res, _val, _rnd);
        return res;
    }
    
    
    /**
     * Basic arithmetic operators
     */
 /*
    inline mpfr_ptr operator +(const mpfr_ptr& _lhs, const mpfr_ptr& _rhs)
    {
        mpfr_ptr res;
        mpfr_init(res);
        mpfr_add(res, _lhs, _rhs, MPFR_RNDN);
        return res;
    }
    
    inline mpfr_ptr operator -(const mpfr_ptr& _lhs, const mpfr_ptr& _rhs)
    {
        mpfr_ptr res;
        mpfr_init(res);
        mpfr_sub(res, _lhs, _rhs, MPFR_RNDN);
        return res;
    }
    
    inline mpfr_ptr operator *(const mpfr_ptr& _lhs, const mpfr_ptr& _rhs)
    {
        mpfr_ptr res;
        mpfr_init(res);
        mpfr_mul(res, _lhs, _rhs, MPFR_RNDN);
        return res;
    }
    
    inline mpfr_ptr operator /(const mpfr_ptr& _lhs, const mpfr_ptr& _rhs)
    {
        // TODO: mpfr_div results in infty when dividing by zero, although this should not be defined.
        mpfr_ptr res;
        mpfr_init(res);
        mpfr_div(res, _lhs, _rhs, MPFR_RNDN);
        return res;
    }
    
    inline mpfr_ptr& operator +=(const mpfr_ptr& _rhs)
    {
        mpfr_add(*this, *this, _rhs, MPFR_RNDN);
        return *this;
    }
    
    inline mpfr_ptr& operator -=(const mpfr_ptr& _rhs)
    {
        mpfr_sub( *this, *this, _rhs, MPFR_RNDN);
        return *this;
    }
    
    inline mpfr_ptr& operator *=(const mpfr_ptr& _rhs)
    {
        mpfr_mul(*this, *this, _rhs, MPFR_RNDN);
        return *this;
    }
    
    inline mpfr_ptr& operator /=(const mpfr_ptr& _rhs)
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
    inline bool operator== (const mpfr_ptr& _lhs, const int _rhs)
    {
        return mpfr_cmp_si(_lhs, _rhs) == 0;
    }
    
    inline bool operator== (const int _lhs, const mpfr_ptr& _rhs)
    {
        return _rhs == _lhs;
    }
    
    inline bool operator== (const mpfr_ptr& _lhs, const double _rhs)
    {
        return mpfr_cmp_d(_lhs,_rhs) == 0;
    }
    
    inline bool operator== (const double _lhs, const mpfr_ptr& _rhs)
    {
        return _rhs == _lhs;
    }
    
    inline bool operator== (const mpfr_ptr& _lhs, const float _rhs)
    {
        return mpfr_cmp_d(_lhs, _rhs);
    }
    
    inline bool operator== (const float _lhs, const mpfr_ptr& _rhs)
    {
        return _rhs == _lhs;
    }

  */  
}