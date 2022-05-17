/** 
 * @file:   GFNumber.tpp
 * @author: Sebastian Junges
 *
 * @since October 17, 2013
 */

#pragma once

#include "GFNumber.h"
#include "adaption_native/EEA.h"

namespace carl
{

template<typename IntegerT>
GFNumber<IntegerT> GFNumber<IntegerT>::inverse() const
{
	assert(mGf != nullptr);
	return GFNumber<IntegerT>(EEA<IntegerT>::calculate(mGf->size(), mN).second, mGf);
	
}



template<typename IntegerT>
bool operator==(const GFNumber<IntegerT>& lhs, const GFNumber<IntegerT>& rhs)
{	
	if (lhs.is_zero() && rhs.is_zero()) return true;
	if (lhs.is_unit() && rhs.is_unit()) return true;
	if (lhs.mN == rhs.mN) return true;
	if (!lhs.mGf || !rhs.mGf) return false;
	assert(lhs.mGf != nullptr);
	assert(rhs.mGf != nullptr);
	return  (lhs.mGf == rhs.mGf || *(lhs.mGf) == *(rhs.mGf)) && (lhs.mGf->modulo(lhs.mN - rhs.mN) == 0);
}
template<typename IntegerT>
bool operator==(const GFNumber<IntegerT>& lhs, const IntegerT& rhs)
{
	assert(lhs.mGf != nullptr);
	return lhs.mGf->modulo(lhs.mN) == rhs;
}
template<typename IntegerT>
bool operator==(const IntegerT& lhs, const GFNumber<IntegerT>& rhs)
{
	return rhs == lhs;
}
template<typename IntegerT>
bool operator==(const GFNumber<IntegerT>& lhs, int rhs)
{
	if(lhs.is_zero() && rhs == 0) return true;
	if(lhs.is_unit() && rhs == 1) return true;
	if(lhs.is_zero() && rhs != 0) return false;
	if(lhs.is_unit() && rhs != 1) return false;
	assert(lhs.mGf != nullptr);
	return lhs.mGf->modulo(lhs.mN) == rhs;
}
template<typename IntegerT>
bool operator==(int lhs, const GFNumber<IntegerT>& rhs)
{
	return rhs == lhs;
}


template<typename IntegerT>
bool operator!=(const GFNumber<IntegerT>& lhs, const GFNumber<IntegerT>& rhs)
{
	return !(lhs == rhs);
}
template<typename IntegerT>
bool operator!=(const GFNumber<IntegerT>& lhs, const IntegerT& rhs)
{
		return !(lhs == rhs);
}
template<typename IntegerT>
bool operator!=(const IntegerT& lhs, const GFNumber<IntegerT>& rhs)
{
	return !(lhs == rhs);
}

template<typename IntegerT>
bool operator!=(const GFNumber<IntegerT>& lhs, int rhs)
{
		return !(lhs == rhs);
}
template<typename IntegerT>
bool operator!=(int lhs, const GFNumber<IntegerT>& rhs)
{
	return !(lhs == rhs);
}

template<typename IntegerT>
const GFNumber<IntegerT> GFNumber<IntegerT>::operator-() const
{
	return GFNumber<IntegerT>(-mN, mGf);
}

template<typename IntegerT>
GFNumber<IntegerT> operator+(const GFNumber<IntegerT>& lhs, const GFNumber<IntegerT>& rhs)
{
	assert(lhs.mGf == nullptr || rhs.mGf == nullptr || *(lhs.mGf) == *(rhs.mGf));
	return GFNumber<IntegerT>(lhs.mN + rhs.mN, lhs.mGf == nullptr ? rhs.mGf : lhs.mGf);
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
GFNumber<IntegerT>& GFNumber<IntegerT>::operator ++()
{
	mN++;
	return *this;
}

template<typename IntegerType>
GFNumber<IntegerType>& GFNumber<IntegerType>::operator +=(const GFNumber& rhs)
{
	if(mGf == nullptr)
	{
		mGf = rhs.mGf;
	}
	mN += rhs.mN;
	return *this;
}

template<typename IntegerType>
GFNumber<IntegerType>& GFNumber<IntegerType>::operator +=(const IntegerType& rhs)
{
	mN += rhs;
	return *this;
}


template<typename IntegerT>
GFNumber<IntegerT> operator-(const GFNumber<IntegerT>& lhs, const GFNumber<IntegerT>& rhs)
{
	assert(lhs.mGf == nullptr || rhs.mGf == nullptr || *(lhs.mGf) == *(rhs.mGf));
	return GFNumber<IntegerT>(lhs.mN - rhs.mN, lhs.mGf == nullptr ? rhs.mGf : lhs.mGf);
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
GFNumber<IntegerT>& GFNumber<IntegerT>::operator --()
{
	mN--;
	return *this;
}

template<typename IntegerType>
GFNumber<IntegerType>& GFNumber<IntegerType>::operator -=(const GFNumber& rhs)
{
	if (rhs.is_zero()) return *this;
	if (mGf == nullptr) {
		mGf = rhs.mGf;
	}
	mN -= rhs.mN;
	return *this;
}

template<typename IntegerType>
GFNumber<IntegerType>& GFNumber<IntegerType>::operator -=(const IntegerType& rhs)
{
	mN -= rhs;
	return *this;
}

template<typename IntegerT>
GFNumber<IntegerT> operator*(const GFNumber<IntegerT>& lhs, const GFNumber<IntegerT>& rhs)
{
	assert(lhs.mGf == nullptr || rhs.mGf == nullptr || *(lhs.mGf) == *(rhs.mGf));
	return GFNumber<IntegerT>(lhs.mN * rhs.mN, lhs.mGf == nullptr ? rhs.mGf : lhs.mGf);
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
GFNumber<IntegerT>& GFNumber<IntegerT>::operator *=(const GFNumber& rhs)
{
	assert(rhs.mGf == nullptr || *mGf == *(rhs.mGf));
	mN *= rhs.mN;
	return *this;
}

template<typename IntegerType>
GFNumber<IntegerType>& GFNumber<IntegerType>::operator *=(const IntegerType& rhs)
{
	mN *= rhs;
	return *this;
}


template<typename IntegerT>
GFNumber<IntegerT> operator/(const GFNumber<IntegerT>& lhs, const GFNumber<IntegerT>& rhs)
{
	assert(!rhs.is_zero());
	assert(rhs.mGf != nullptr);
	return GFNumber<IntegerT>(lhs.mN * rhs.inverse().mN, rhs.mGf);
}


template<typename IntegerT>
GFNumber<IntegerT>& GFNumber<IntegerT>::operator /=(const GFNumber<IntegerT>& rhs)
{
	assert(!rhs.is_zero());
	mN * rhs.inverse().mN;
	return *this;
}

}
