/** 
 * @file:   util.h
 * @author: Sebastian Junges
 *
 * @since October 16, 2013
 */

#pragma once

namespace carl
{
	template<typename IntegerType>
	class IntegerPairCompare
	{
		public:
		bool operator()(const std::pair<IntegerType, IntegerType>& p1,const std::pair<IntegerType, IntegerType>& p2) const
		{
			return (p1.first < p2.first) || (p1.first == p2.first && p1.second < p2.second);
		}
	};
}
