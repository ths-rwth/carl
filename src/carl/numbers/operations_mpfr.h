/* This file contains the definition of all arithmetic operations for the mpfr_t float type.
 * File:   operations_mpfr.h
 * @author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 *
 * @since	2013-12-13
 * @version 2013-12-15
 */

#pragma once

#ifdef USE_MPFR_FLOAT
#include <mpfr.h>

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
    
    inline mpfr_t abs(const mpfr_t& _val)
    {
        mpfr_t res;
        mpfr_init(res);
        mpfr_abs(res, _val, MPFR_RNDN);
        return res;
    }

    inline int floor(const mpfr_t& _val)
    {
        mpfr_t result;
        mpfr_init(result);
        mpfr_rint_floor(result, _val, MPFR_RNDN);
        int tmp = mpfr_integer_p(result);
        mpfr_clear(result);
        return tmp;
    }

    inline int ceil(const mpfr_t& _val)
    {
        mpfr_t result;
        mpfr_init(result);
        mpfr_rint_ceil(result, _val, MPFR_RNDN);
        int _result = mpfr_integer_p(result);
        mpfr_clear(result);
        return _result;
    }

    inline mpfr_t pow(const mpfr_t& _val, unsigned _exp, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_t res;
        mpfr_init(res);
        mpfr_pow_ui(res, _val, _exp, _rnd);
        return res;
    }

    inline std::pair<mpfr_t,mpfr_t> sqrt(const mpfr_t& _val)
    {
        mpfr_t resA;
        mpfr_init(resA);
        mpfr_t resB;
        mpfr_init(resB);
        mpfr_sqrt(resA, _val, MPFR_RNDD);
        mpfr_sqrt(resB, _val, MPFR_RNDU);
        return std::make_pair(resA, resB);
    }

    mpfr_t add( const mpfr_t& _op1, const mpfr_t& _op2, mpfr_rnd_t _rnd = MPFR_RNDN ) const
    {
        mpft_t result;
        mpfr_init(result);
        mpfr_add(result, _op1, _op2, _rnd);
        return result;
    }

    mpfr_t sub( const mpfr_t& _op1, const mpfr_t& _op2, mpfr_rnd_t _rnd = MPFR_RNDN ) const
    {
        mpft_t result;
        mpfr_init(result);
        mpfr_sub(result, _op1 , _op2, _rnd);
        return result;
    }

    mpfr_t mul( const mpfr_t& _op1, const mpfr_t& _op2, mpfr_rnd_t _rnd = MPFR_RNDN ) const
    {
        mpft_t result;
        mpfr_init(result);
        mpfr_mul(result, _op1, _op2, _rnd);
        return result;
    }

    mpfr_t div( const mpfr_t& _op1, const mpfr_t& _op2, mpfr_rnd_t _rnd = MPFR_RNDN ) const
    {
        assert( mpfr_zero_p(_op2) != 0 );
        mpft_t result;
        mpfr_init(result);
        mpfr_div(result, _op1, _op2, _rnd);
        return result;
    }
    
    /**
     * Transcendental functions
     */
    
    mpfr_t& exp_assign(mpfr_t& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_exp(_val, _val , _rnd);
        return _val;
    }
    
    mpfr_t& exp(const mpfr_t& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_t res;
        mpfr_init(res);
        mpfr_exp(res, _val , _rnd);
        return res;
    }
    
    mpfr_t& sin_assign(mpfr_t& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_sin(_val, _val, _rnd);
        return _val;
    }
    
    mpfr_t& sin(const mpfr_t& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_t res;
        mpfr_init(res);
        mpfr_sin(res, _val, _rnd);
        return res;
    }
    
    mpfr_t& cos_assign(mpfr_t& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_cos(_val, _val, _rnd);
        return _val;
    }
    
    mpfr_t& cos(const mpfr_t& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_t res;
        mpfr_init(res);
        mpfr_cos(res, _val, _rnd);
        return res;
    }
    
    mpfr_t& log_assign(mpfr_t& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_log(_val, _val, _rnd);
        return _val;
    }
    
    mpfr_t& log(const mpfr_t& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_t res;
        mpfr_init(res);
        mpfr_log(res, _val, _rnd);
        return res;
    }
    
    mpfr_t& tan_assign(mpfr_t& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_tan(_val, _val, _rnd);
        return _val;
    }
    
    mpfr_t& tan(const mpfr_t& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_t res;
        mpfr_init(res);
        mpfr_tan(res, _val, _rnd);
        return res;
    }
    
    mpfr_t& asin_assign(mpfr_t& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_asin(_val, _val, _rnd);
        return _val;
    }
    
    mpfr_t& asin(const mpfr_t& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_t res;
        mpfr_init(res);
        mpfr_asin(res, _val, _rnd);
        return res;
    }
    
    mpfr_t& acos_assign(mpfr_t& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_acos(_val, _val, _rnd);
        return _val;
    }
    
    mpfr_t& acos(const mpfr_t& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_t res;
        mpfr_init(res);
        mpfr_acos(res, _val, _rnd);
        return res;
    }
    
    mpfr_t& atan_assign(mpfr_t& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_atan(_val, _val, _rnd);
        return _val;
    }
    
    mpfr_t& atan(const mpfr_t& _result, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_t res;
        mpfr_init(res);
        mpfr_atan(res, _val, _rnd);
        return res;
    }
    
    mpfr_t& sinh_assign(mpfr_t& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_sinh(_val, _val, _rnd);
        return _val;
    }
    
    mpfr_t& sinh(const mpfr_t& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_t res;
        mpfr_init(res);
        mpfr_sinh(res, _val, _rnd);
        return res;
    }
    
    mpfr_t& cosh_assign(mpfr_t& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_cosh(_val, _val, _rnd);
        return _val;
    }
    
    mpfr_t& cosh(const mpfr_t& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_t res;
        mpfr_init(res);
        mpfr_cosh(res, _val, _rnd);
        return res;
    }
    
    mpfr_t& tanh_assign(mpfr_t& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_tanh(_val, _val, _rnd);
        return _val;
    }
    
    mpfr_t& tanh(const mpfr_t& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_t res;
        mpfr_init(res);
        mpfr_tanh(res, _val, _rnd);
        return res;
    }
    
    mpfr_t& asinh_assign(mpfr_t& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_asinh(_val, _val, _rnd);
        return _val;
    }
    
    mpfr_t& asinh(const mpfr_t& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_t res;
        mpfr_init(res);
        mpfr_asinh(res, _val, _rnd);
        return res;
    }
    
    mpfr_t& acosh_assign(mpfr_t& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_acosh(_val, _val, _rnd);
        return _val;
    }
    
    mpfr_t& acosh(const mpfr_t& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_t res;
        mpfr_init(res);
        mpfr_acosh(res, _val, _rnd);
        return res;
    }
    
    mpfr_t& atanh_assign(mpfr_t& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_atanh(_val, _val, _rnd);
        return _val;
    }
    
    mpfr_t& atanh(mpfr_t& _val, mpfr_rnd_t _rnd = MPFR_RNDN)
    {
        mpfr_t res;
        mpfr_init(res);
        mpfr_atanh(res, _val, _rnd);
        return res;
    }
    
    
    /**
     * Basic arithmetic operators
     */
    
    inline mpfr_t operator +(const mpfr_t& _lhs, const mpfr_t& _rhs)
    {
        mpfr_t res;
        mpfr_init(res);
        mpfr_add(res, _lhs, _rhs, MPFR_RNDN);
        return res;
    }
    
    inline mpfr_t operator -(const mpfr_t& _lhs, const mpfr_t& _rhs)
    {
        mpfr_t res;
        mpfr_init(res);
        mpfr_sub(res, _lhs, _rhs, MPFR_RNDN);
        return res;
    }
    
    inline mpfr_t operator *(const mpfr_t& _lhs, const mpfr_t& _rhs)
    {
        mpfr_t res;
        mpfr_init(res);
        mpfr_mul(res, _lhs, _rhs, MPFR_RNDN);
        return res;
    }
    
    inline mpfr_t operator /(const mpfr_t& _lhs, const mpfr_t& _rhs)
    {
        // TODO: mpfr_div results in infty when dividing by zero, although this should not be defined.
        mpfr_t res;
        mpfr_init(res);
        mpfr_div(res, _lhs, _rhs, MPFR_RNDN);
        return res;
    }
    
    inline mpfr_t& operator +=(const mpfr_t& _rhs)
    {
        mpfr_add(*this, *this, _rhs, MPFR_RNDN);
        return *this;
    }
    
    inline mpfr_t& operator -=(const mpfr_t& _rhs)
    {
        mpfr_sub( *this, *this, _rhs, MPFR_RNDN);
        return *this;
    }
    
    inline mpfr_t& operator *=(const mpfr_t& _rhs)
    {
        mpfr_mul(*this, *this, _rhs, MPFR_RNDN);
        return *this;
    }
    
    inline mpfr_t& operator /=(const mpfr_t& _rhs)
    {
        // TODO: mpfr_div results in infty when dividing by zero, although this should not be defined.
        mpfr_div(*this, *this, _rhs, MPFR_RNDN);
        return *this;
    }
    
    /*
     * Comparison operators
     */
    inline bool operator== (const mpfr_t& _lhs, const int _rhs)
    {
        return mpfr_cmp_si(_lhs, _rhs) == 0;
    }
    
    inline bool operator== (const int _lhs, const mpfr_t& _rhs)
    {
        return _rhs == _lhs;
    }
    
    inline bool operator== (const mpfr_t& _lhs, const double _rhs)
    {
        return mpfr_cmp_d(_lhs,_rhs) == 0;
    }
    
    inline bool operator== (const double _lhs, const mpfr_t& _rhs)
    {
        return _rhs == _lhs;
    }
    
    inline bool operator== (const mpfr_t& _lhs, const float _rhs)
    {
        return mpfr_cmp_d(_lhs, _rhs);
    }
    
    inline bool operator== (const float _lhs, const mpfr_t& _rhs)
    {
        return _rhs == _lhs;
    }

    
}
#endif