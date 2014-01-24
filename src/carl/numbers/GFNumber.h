/** 
 * @file   GFNumber.h
 * @author Sebastian Junges
 *
 * @since October 16, 2013
 */

#pragma once

#include "util.h"
#include "numbers.h"
#include "GaloisField.h"

namespace carl
{

/**
 * Galois Field numbers, i.e. numbers from fields with a finite characteristic.
 */
template<typename IntegerType>
class GFNumber
{
	IntegerType mN;
	const GaloisField<IntegerType>* mGf;
	
	public:
	GFNumber(IntegerType n, const GaloisField<IntegerType>* gf= nullptr)
	: mN(gf == nullptr ? n : gf->symmetricModulo(n)), mGf(gf)
	{
		
	}
	
	GFNumber(const GFNumber& n, const GaloisField<IntegerType>* gf) :
	mN(n.mN),
	mGf(gf)
	{
		
	}

	const GaloisField<IntegerType>* gf() const
	{
		return mGf;
	}
	
	GFNumber<IntegerType> toGF(const GaloisField<IntegerType>* newfield) const
	{
		return GFNumber(mN, newfield);
	}
	
	void normalize()
	{
		if(isZero() || isUnit()) return;
		assert(mGf != nullptr);
		mGf->modulo(mN);
	}
	
	bool isZero() const
	{
		return mN == 0;
	}
	bool isUnit() const
	{
		return mN == 1;
	}
	
	const IntegerType& representingInteger() const
	{
		return mN;
	}
	
	GFNumber inverse() const;
	
	template<typename IntegerT>
	friend bool operator==(const GFNumber<IntegerT>& lhs, const GFNumber<IntegerT>& rhs);
	/**
	 * lhs == rhs, if rhs \\in [lhs].
     * @return 
     */
	template<typename IntegerT>
	friend bool operator==(const GFNumber<IntegerT>& lhs, const IntegerT& rhs);
	/**
	 * lhs == rhs, if lhs \\in [rhs].
     * @return 
     */
	template<typename IntegerT>
	friend bool operator==(const IntegerT& lhs, const GFNumber<IntegerT>& rhs);
	template<typename IntegerT>
	friend bool operator==(const GFNumber<IntegerT>& lhs, const GFNumber<IntegerT>& rhs);
	/**
	 * lhs == rhs, if rhs \\in [lhs].
     * @return 
     */
	template<typename IntegerT>
	friend bool operator==(const GFNumber<IntegerT>& lhs, int rhs);
	/**
	 * lhs == rhs, if lhs \\in [rhs].
     * @return 
     */
	template<typename IntegerT>
	friend bool operator==(int lhs, const GFNumber<IntegerT>& rhs);
	template<typename IntegerT>
	friend bool operator!=(const GFNumber<IntegerT>& lhs, const GFNumber<IntegerT>& rhs);
	template<typename IntegerT>
	friend bool operator!=(const GFNumber<IntegerT>& lhs, const IntegerT& rhs);
	template<typename IntegerT>
	friend bool operator!=(const IntegerT& lhs, const GFNumber<IntegerT>& rhs);
	template<typename IntegerT>
	friend bool operator!=(const GFNumber<IntegerT>& lhs, int rhs);
	template<typename IntegerT>
	friend bool operator!=(int lhs, const GFNumber<IntegerT>& rhs);
	
	
	const GFNumber operator-() const;
	
	template<typename IntegerT>
	friend GFNumber<IntegerT> operator+(const GFNumber<IntegerT>& lhs, const GFNumber<IntegerT>& rhs);
	template<typename IntegerT>
	friend GFNumber<IntegerT> operator+(const GFNumber<IntegerT>& lhs, const IntegerT& rhs);
	template<typename IntegerT>
	friend GFNumber<IntegerT> operator+(const IntegerT& lhs, const GFNumber<IntegerT>& rhs);
	
	GFNumber& operator++();
	GFNumber& operator+=(const GFNumber& rhs);
	GFNumber& operator+=(const IntegerType& rhs);
	
	template<typename IntegerT>
	friend GFNumber<IntegerT> operator-(const GFNumber<IntegerT>& lhs, const GFNumber<IntegerT>& rhs);
	template<typename IntegerT>
	friend GFNumber<IntegerT> operator-(const GFNumber<IntegerT>& lhs, const IntegerT& rhs);
	template<typename IntegerT>
	friend GFNumber<IntegerT> operator-(const IntegerT& lhs, const GFNumber<IntegerT>& rhs);
	
	GFNumber& operator--();
	GFNumber& operator-=(const GFNumber& rhs);
	GFNumber& operator-=(const IntegerType& rhs);
	
	template<typename IntegerT>
	friend GFNumber<IntegerT> operator*(const GFNumber<IntegerT>& lhs, const GFNumber<IntegerT>& rhs);
	template<typename IntegerT>
	friend GFNumber<IntegerT> operator*(const GFNumber<IntegerT>& lhs, const IntegerT& rhs);
	template<typename IntegerT>
	friend GFNumber<IntegerT> operator*(const IntegerT& lhs, const GFNumber<IntegerT>& rhs);
	
	GFNumber& operator*=(const GFNumber& rhs);
	GFNumber& operator*=(const IntegerType& rhs);
	
	template<typename IntegerT>
	friend GFNumber<IntegerT> operator/(const GFNumber<IntegerT>& lhs, const GFNumber<IntegerT>& rhs);

	GFNumber& operator/=(const GFNumber& rhs);
	
	friend std::ostream& operator<<(std::ostream& os, const GFNumber& rhs)
	{
		os << "(" << rhs.mN << ") mod ";
		if(rhs.mGf != nullptr)
		{
			os << rhs.mGf->size();
		}
		else
		{
			os << "?";
		}
		return os;
	}
	
};
}
#include "GFNumber.tpp"
