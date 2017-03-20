/** 
 * @file:   ReasonsAdaptor.h
 * @author: Sebastian Junges
 *
 * @since September 25, 2013
 */

#pragma once
#include "../../util/BitVector.h"

namespace carl
{
struct NoReasons
{
	static constexpr bool has_reasons = false;
	void setReason(unsigned index);
	BitVector getReasons() const { return BitVector(0); }
	void setReasons(const BitVector&) const {}
	virtual ~NoReasons() = default;
};

struct BVReasons
{
	static constexpr bool has_reasons = true;
	
	void setReason(unsigned index);
	void extendReasons(const BitVector& extendWith)
	{
		mReasonset |= extendWith;
	}
	BitVector getReasons() const
	{
		return mReasonset;
	}
	
	void setReasons(const BitVector& reasons)
	{
		mReasonset = reasons;
	}
private:
	BitVector mReasonset;
};
}
