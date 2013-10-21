/** 
 * @file:   GFNumber.tpp
 * @author: Sebastian Junges
 *
 * @since October 17, 2013
 */

#pragma once

#include "GFNumber.h"
#include "EEA.h"

namespace carl
{

template<typename IntegerT>
GFNumber<IntegerT> GFNumber<IntegerT>::inverse() const
{
	return GFNumber<IntegerT>(EEA<IntegerT>::calculate(mGf->size(), mN).second, mGf);
}

template<typename IntegerT>
bool operator==(const GFNumber<IntegerT>& lhs, const GFNumber<IntegerT>& rhs)
{
	// same field AND same number after normalization.
	return  (lhs.mGf == rhs.mGf || *(lhs.mGf) == *(rhs.mGf)) && (lhs.mGf->modulo(lhs.mN - rhs.mN) == 0);
}
template<typename IntegerT>
bool operator!=(const GFNumber<IntegerT>& lhs, const GFNumber<IntegerT>& rhs)
{
	return !(lhs == rhs);
}
	

template<typename IntegerT>
GFNumber<IntegerT> operator+(const GFNumber<IntegerT>& lhs, const GFNumber<IntegerT>& rhs)
{
	assert(*(lhs.mGf) == *(rhs.mGf));
	return GFNumber<IntegerT>(lhs.mN + rhs.mN, lhs.mGf);
}
template<typename IntegerT>
GFNumber<IntegerT> operator+(const GFNumber<IntegerT>& lhs, const IntegerT& rhs)
{
	return GFNumber<IntegerT>(lhs.mN + rhs, lhs.mGf);
}
template<typename IntegerT>
GFNumber<IntegerT> operator+(const IntegerT& lhs, const GFNumber<IntegerT>& rhs)
{
	return rhs + lhs;
}

template<typename IntegerT>
GFNumber<IntegerT> operator-(const GFNumber<IntegerT>& lhs, const GFNumber<IntegerT>& rhs)
{
	assert(*(lhs.mGf) == *(rhs.mGf));
	return GFNumber<IntegerT>(lhs.mN - rhs.mN, lhs.mGf);
}
template<typename IntegerT>
GFNumber<IntegerT> operator-(const GFNumber<IntegerT>& lhs, const IntegerT& rhs)
{
	return GFNumber<IntegerT>(lhs.mN - rhs, lhs.mGf);
}
template<typename IntegerT>
GFNumber<IntegerT> operator-(const IntegerT& lhs, const GFNumber<IntegerT>& rhs)
{
	return GFNumber<IntegerT>(lhs - rhs.mN, rhs.mGf);
}

template<typename IntegerT>
GFNumber<IntegerT> operator*(const GFNumber<IntegerT>& lhs, const GFNumber<IntegerT>& rhs)
{
	assert(*(lhs.mGf) == *(rhs.mGf));
	return GFNumber<IntegerT>(lhs.mN * rhs.mN, lhs.mGf);
}
template<typename IntegerT>
GFNumber<IntegerT> operator*(const GFNumber<IntegerT>& lhs, const IntegerT& rhs)
{
	return GFNumber<IntegerT>(lhs.mN * rhs, lhs.mGf);
}
template<typename IntegerT>
GFNumber<IntegerT> operator*(const IntegerT& lhs, const GFNumber<IntegerT>& rhs)
{
	return rhs * lhs;
}

template<typename IntegerT>
GFNumber<IntegerT> operator/(const GFNumber<IntegerT>& lhs, const GFNumber<IntegerT>& rhs)
{
	assert(*(lhs.mGf) == *(rhs.mGf));	
	return GFNumber<IntegerT>(lhs.mN * rhs.inverse().mN, lhs.mGf);
}

}