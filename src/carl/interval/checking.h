/* 
 * A struct, which contains the checking policies for FLOAT_T intervals when used as boost intervals.
 * 
 * @file   checking.h
 * @author Stefan Schupp
 * @author Benedikt Seidl
 *
 * @since	2013-12-05
 * @version 2014-01-27
 */

#pragma once
//#include "../numbers/FLOAT_T.h"
#include <assert.h>


namespace carl
{
    template<typename Number>
    struct checking
    {
        static Number pos_inf()
        {
            return Number(1);
        }
        static Number neg_inf()
        {
            return Number(-1);
        }
        static Number nan()
        {
            assert(false);
        }
        static bool is_nan(const Number&)
        {
            return false;
        }
        static Number empty_lower()
        {
            return Number(0);
        }
        static Number empty_upper()
        {
            return Number(0);
        }
        static bool is_empty(const Number& _left, const Number& _right)
        {
            return _left > _right;
        }
    };

	template<>
	struct checking<FLOAT_T<mpfr_t> >
	{
		static FLOAT_T<mpfr_t> pos_inf()
		{
			FLOAT_T<mpfr_t> number();
			mpfr_set_inf(number.getValue(), 1);
			return number;
		}
		static FLOAT_T<mpfr_t> neg_inf()
		{
			FLOAT_T<mpfr_t> number();
			mpfr_set_inf(number.getValue(), -1);
			return number;
		}
		static FLOAT_T<mpfr_t> nan()
		{
			FLOAT_T<mpfr_t> number();
			mpfr_set_nan(number.getValue());
			return number;
		}
		static bool is_nan(const FLOAT_T<mpfr_t>& number)
		{
			return mpfr_nan_p(number) != 0;
		}
        static Number empty_lower()
        {
            return FLOAT_T<mpfr_t>(0);
        }
        static Number empty_upper()
        {
            return FLOAT_T<mpfr_t>(0);
        }
        static bool is_empty(const FLOAT_T<mpfr_t>& _left, const FLOAT_T<mpfr_t>& _right)
        {
            return _left > _right;
        }
	}
}