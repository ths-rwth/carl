/** 
 * @file   GFNumber.h
 * @author Sebastian Junges
 *
 * @since October 16, 2013
 */

#pragma once

#include "GaloisField.h"
#include "numbers.h"

namespace carl
{

/**
 * Galois Field numbers, i.e. numbers from fields with a finite characteristic.
 */
template<typename IntegerType>
class GFNumber
{
	IntegerType mN = carl::constant_zero<IntegerType>::get();
	const GaloisField<IntegerType>* mGf = nullptr;
	
	public:

	GFNumber() = default;
	explicit GFNumber(IntegerType n, const GaloisField<IntegerType>* gf = nullptr):
		mN(gf == nullptr ? n : gf->symmetricModulo(n)),
		mGf(gf)
	{
	}
	
	GFNumber(const GFNumber& n, const GaloisField<IntegerType>* gf):
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
    
	bool isOne() const
	{
		return isUnit();
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

template<typename IntegerT>
static bool isZero(const GFNumber<IntegerT>& _in) {
	return _in.isZero();
}

template<typename IntegerT>
static bool isOne(const GFNumber<IntegerT>& _in) {
	return _in.isOne();
}

template<typename IntegerT>
GFNumber<IntegerT> quotient(const GFNumber<IntegerT>& lhs, const GFNumber<IntegerT>& rhs) {
	return lhs / rhs;
}

template<typename IntegerT>
GFNumber<IntegerT> abs(const GFNumber<IntegerT>& n) {
	return n;
}

/**
 * @todo Implement this
 * @param 
 * @return 
 */
template<typename IntegerT>
inline bool isInteger(const GFNumber<IntegerT>& /*unused*/) {
	return false;
}

/**
 * Creates the string representation to the given galois field number.
 * @param _number The galois field number to get its string representation for.
 * @return The string representation to the given galois field number.
 */
template<typename IntegerType>
std::string toString(const GFNumber<IntegerType>& _number, bool /*unused*/)
{
    std::stringstream s;
    s << _number;
    return s.str();
}

}
#include "GFNumber.tpp"
