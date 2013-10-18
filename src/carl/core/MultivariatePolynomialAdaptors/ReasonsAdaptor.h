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
};

struct BVReasons
{
	static const bool has_reasons = false;
	
	void setReason(unsigned index);
private:
	BitVector mReasonset;
};
}