/** 
 * @file:   GaloisField.h
 * @author: Sebastian Junges
 *
 * @since October 17, 2013
 */

#pragma once

#include <memory>
#include <mutex>
#include <map>

namespace carl
{
/**
 * A finite field.
 */
template<typename IntegerType>
class GaloisField
{
	const IntegerType mP;
	const unsigned mK;
	const IntegerType mPK;
	const IntegerType symmetricModuloOperationOffset;
	
	public:
	/**
	 * Creating the field Z_{p^k}
     * @param p A prime number
     * @param k A exponent
	 * @see GaloisFieldManager where the overhead of creating several GFs is prevented by storing them.
     */
	GaloisField(IntegerType p, unsigned k=1)
	: mP(p), mK(k), mPK(pow(p,k)), symmetricModuloOperationOffset( (mPK-1)/2 )
	{ 
	}
	
	/**
	 * Returns the p from Z_{p^k}
     * @return a prime
     */
	const IntegerType& p() const
	{
		return mP;
	}
	
	/**
	 * Returns the k from Z_{p^k}
     * @return A positive integer
     */
	unsigned k() const
	{
		return mK;
	}
	
	const IntegerType& size() const
	{
		return mPK;
	}
	
	IntegerType modulo(const IntegerType& n) const
	{
		return symmetricModulo(n);
	}
	
	IntegerType symmetricModulo(const IntegerType& n) const
	{
		return n > (mPK-1)/2 ?  mod(n-(mPK+1)/2, mPK) - (mPK-1)/2 : mod(n, mPK);
	}
	
	friend std::ostream& operator<<(std::ostream& os, const GaloisField& rhs)
	{
		return os << "GF(" << rhs.mP << "^" << rhs.mK << ")";
	}
};

template<typename IntegerType>
class GaloisFieldManager
{
	static std::unique_ptr<GaloisFieldManager> mInstance;
    /**
	 * Notice that for fully c++11 compliant compilers, this is redundant.
	 */
	static std::once_flag mOnceFlag;
	std::map<std::pair<IntegerType, IntegerType>, const GaloisField<IntegerType>*, IntegerPairCompare<IntegerType>> mGaloisFields;
	
	GaloisFieldManager()
	{
		
	}
	
	public:
	static GaloisFieldManager& getInstance();
	
	const GaloisField<IntegerType>* getField(const IntegerType& p, unsigned k)
	{
		auto it = mGaloisFields.find(std::make_pair(p,k));
		if(it == mGaloisFields.end())
		{	
			return mGaloisFields.insert(std::make_pair(std::make_pair(p,k),new GaloisField<IntegerType>(p,k))).first->second;
		}
		else
		{
			return it->second;
		}
	}
		
	
};


template<typename IntegerType>
std::unique_ptr<GaloisFieldManager<IntegerType>> GaloisFieldManager<IntegerType>::mInstance;
template<typename IntegerType>
std::once_flag GaloisFieldManager<IntegerType>::mOnceFlag;


template<typename IntegerType>
GaloisFieldManager<IntegerType>& GaloisFieldManager<IntegerType>::getInstance()
{
	std::call_once(mOnceFlag,
		[]  {
			mInstance.reset(new GaloisFieldManager());
	});
	return *mInstance.get();
}

}
