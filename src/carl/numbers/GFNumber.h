/** 
 * @file:   GF.h
 * @author: Sebastian Junges
 *
 * @since October 16, 2013
 */

#pragma once

#include "util.h"
#include "numbers.h"
#include "GaloisField.h"

namespace carl
{

template<typename IntegerType>
class GFNumber;


/**
 * Galois Field numbers, i.e. numbers from fields with a finite characteristic.
 */
template<typename IntegerType>
class GFNumber
{
	IntegerType mN;
	const GaloisField<IntegerType>* mGf;
	
	public:
	GFNumber(IntegerType n, const GaloisField<IntegerType>* gf)
	: mN(gf->symmetricModulo(n)), mGf(gf)
	{
		
	}
	
	void normalize()
	{
		mGf->modulo(mN);
	}
	
	bool isZero()
	{
		return mN == 0;
	}
	bool isUnit()
	{
		return mN == 1;
	}
	
	IntegerType getRepresentingInteger() const
	{
		return mN;
	}
	
	GFNumber inverse() const;
	
	template<typename IntegerT>
	friend bool operator==(const GFNumber<IntegerT>& lhs, const GFNumber<IntegerT>& rhs);
	template<typename IntegerT>
	friend bool operator!=(const GFNumber<IntegerT>& lhs, const GFNumber<IntegerT>& rhs);
	
	template<typename IntegerT>
	friend GFNumber<IntegerT> operator+(const GFNumber<IntegerT>& lhs, const GFNumber<IntegerT>& rhs);
	template<typename IntegerT>
	friend GFNumber<IntegerT> operator+(const GFNumber<IntegerT>& lhs, const IntegerT& rhs);
	template<typename IntegerT>
	friend GFNumber<IntegerT> operator+(const IntegerT& lhs, const GFNumber<IntegerT>& rhs);
	
	template<typename IntegerT>
	friend GFNumber<IntegerT> operator-(const GFNumber<IntegerT>& lhs, const GFNumber<IntegerT>& rhs);
	template<typename IntegerT>
	friend GFNumber<IntegerT> operator-(const GFNumber<IntegerT>& lhs, const IntegerT& rhs);
	template<typename IntegerT>
	friend GFNumber<IntegerT> operator-(const IntegerT& lhs, const GFNumber<IntegerT>& rhs);
	
	template<typename IntegerT>
	friend GFNumber<IntegerT> operator*(const GFNumber<IntegerT>& lhs, const GFNumber<IntegerT>& rhs);
	template<typename IntegerT>
	friend GFNumber<IntegerT> operator*(const GFNumber<IntegerT>& lhs, const IntegerT& rhs);
	template<typename IntegerT>
	friend GFNumber<IntegerT> operator*(const IntegerT& lhs, const GFNumber<IntegerT>& rhs);
	
	template<typename IntegerT>
	friend GFNumber<IntegerT> operator/(const GFNumber<IntegerT>& lhs, const GFNumber<IntegerT>& rhs);
	
	friend std::ostream& operator<<(std::ostream& os, const GFNumber& rhs)
	{
		return os << "(" << rhs.mN << ") mod " << rhs.mGf->size();
	}
	
};
}
#include "GFNumber.tpp"
