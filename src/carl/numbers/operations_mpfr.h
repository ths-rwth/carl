/* This file contains the definition of all arithmetic operations for the mpfr_t float type.
 * File:   operations_mpfr.h
 * @author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 *
 * @since 2013-12-13
 * @version 2013-12-13
 */

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

    template<typename C>
    inline bool isInteger(const GFNumber<C>&) {
            return true;
    }

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
     * Basic Operators
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
        assert( mpfr_zero_p(_op2.mValue) != 0 );
        mpft_t result;
        mpfr_init(result);
        mpfr_div(result, _op1, _op2, _rnd);
        return result;
    }
}