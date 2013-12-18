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
	const unsigned mP;
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
	GaloisField(unsigned p, unsigned k=1)
	: mP(p), mK(k), mPK(pow(IntegerType(p),k)), symmetricModuloOperationOffset( (mPK-1)/2 )
	{ 
	}
	
	/**
	 * Returns the p from Z_{p^k}
     * @return a prime
     */
	unsigned p() const
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
		return n > (mPK-1)/2 ?  mod((IntegerType)(n-(mPK+1)/2), mPK) - (mPK-1)/2 : mod(n, mPK);
	}
	
	friend bool operator==(const GaloisField& lhs, const GaloisField& rhs)
	{
		return lhs.mPK == rhs.mPK;
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
	std::map<std::pair<unsigned, unsigned>, const GaloisField<IntegerType>*, IntegerPairCompare<unsigned>> mGaloisFields;
	
	GaloisFieldManager()
	{
		
	}
	
	public:
	static GaloisFieldManager& getInstance();
	
	const GaloisField<IntegerType>* getField(unsigned p, unsigned k=1)
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
