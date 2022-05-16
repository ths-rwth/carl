/** 
 * @file:   GaloisField.h
 * @author: Sebastian Junges
 *
 * @since October 17, 2013
 */

#pragma once

#include <carl/numbers/numbers.h>
#include <carl-common/memory/Singleton.h>

#include <map>
#include <memory>
#include <mutex>
#include <utility>

namespace carl
{

template<typename IntegerType>
struct IntegerPairCompare {
	bool operator()(const std::pair<IntegerType, IntegerType>& p1,const std::pair<IntegerType, IntegerType>& p2) const {
		return (p1.first < p2.first) || (p1.first == p2.first && p1.second < p2.second);
	}
};

/**
 * A finite field.
 */
template<typename IntegerType>
class GaloisField {
public:
	using BaseIntType = unsigned;
private:
	const BaseIntType mP;
	const BaseIntType mK;
	const IntegerType mPK; // = mP ^ mK
	const IntegerType mMaxValue; // = (mPK-1) / 2
	const IntegerType mModulus; // = (mPK+1) / 2 = mMaxValue + 1
	
	public:
	/**
	 * Creating the field Z_{p^k}
     * @param p A prime number
     * @param k A exponent
	 * @see GaloisFieldManager where the overhead of creating several GFs is prevented by storing them.
     */
	explicit GaloisField(BaseIntType p, BaseIntType k = 1):
		mP(p), mK(k),
		mPK(pow(p,k)),
		mMaxValue((mPK-1)/2),
		mModulus(mMaxValue+1)
	{ 
	}
	
	/**
	 * Returns the p from Z_{p^k}
     * @return a prime
     */
	BaseIntType p() const noexcept {
		return mP;
	}
	
	/**
	 * Returns the k from Z_{p^k}
     * @return A positive integer
     */
	BaseIntType k() const noexcept {
		return mK;
	}
	
	const IntegerType& size() const noexcept {
		return mPK;
	}
	
	IntegerType modulo(const IntegerType& n) const {
		return symmetric_modulo(n);
	}
	
	IntegerType symmetric_modulo(const IntegerType& n) const	{
		if (n > mMaxValue) {
			return carl::mod(IntegerType(n-mModulus), mPK) - mMaxValue;
		} else {
			return carl::mod(n, mPK);
		}
	}
	
	friend bool operator==(const GaloisField& lhs, const GaloisField& rhs) {
		return lhs.mPK == rhs.mPK;
	}
	
	friend std::ostream& operator<<(std::ostream& os, const GaloisField& rhs) {
		return os << "GF(" << rhs.mP << "^" << rhs.mK << ")";
	}
};

template<typename IntegerType>
class GaloisFieldManager: public Singleton<GaloisFieldManager<IntegerType>> {
public:
	using BaseIntType = typename GaloisField<IntegerType>::BaseIntType;
private:
	std::map<std::pair<BaseIntType,BaseIntType>, std::unique_ptr<GaloisField<IntegerType>>, IntegerPairCompare<unsigned>> mGaloisFields;
public:
	
	const GaloisField<IntegerType>* field(BaseIntType p, BaseIntType k = 1)
	{
		auto it = mGaloisFields.find(std::make_pair(p,k));
		if (it == mGaloisFields.end()) {	
			auto newit = mGaloisFields.emplace(
				std::make_pair(p,k),
				std::make_unique<GaloisField<IntegerType>>(p, k)
			);
			assert(newit.second);
			return newit.first->second.get();
		} else {
			return it->second.get();
		}
	}
};

}
