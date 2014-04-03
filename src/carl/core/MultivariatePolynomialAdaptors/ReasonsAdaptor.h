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
	static const bool has_reasons = false;
	void setReason(unsigned index);
	int getReasons() const { return 0;}
	void setReasons(...) const {}
};

struct BVReasons
{
	static const bool has_reasons = false;
	
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