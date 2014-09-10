/** 
 * @file:   DecimalStringToRational.h
 * @author: Sebastian Junges
 *
 * @since April 21, 2014
 */

#pragma once
#include <vector>
#include "boost/algorithm/string.hpp"
#include "adaption_gmpxx/operations.h"

namespace carl
{
template<class RationalType>
struct DecimalStringToRational
{
	RationalType operator()(const std::string& inputstring) const
	{
		std::vector<std::string> strs;
		boost::split(strs, inputstring, boost::is_any_of(". ,"));
		
		if(strs.size() > 2)
		{
			throw std::invalid_argument("More than one delimiter in the string.");
		}
		RationalType result;
		if(!strs.front().empty())
		{
			result += RationalType(strs.front());
		}
		if(strs.size() > 1)
		{
			result += (RationalType(strs.back())/carl::pow(typename IntegralType<RationalType>::type(10),strs.back().size()));
		}
		return result;
	}
};
}