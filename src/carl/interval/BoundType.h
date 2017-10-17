/* 
 * File which contains the enum of the bound type for intervals as well as related functions.
 * @file   BoundType.h
 * @author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 *
 * @since	2013-10-28
 * @version 2014-01-07
 */

#pragma once

namespace carl
{
    enum class BoundType {
                /// the given bound is compared by a strict ordering relation
                STRICT=0,
                /// the given bound is compared by a weak ordering relation
                WEAK=1,
                /// the given bound is interpreted as minus or plus infinity depending on whether it is the left or the right bound
                INFTY=2
            };
    
	inline static BoundType getWeakestBoundType( BoundType type1, BoundType type2 )
	{
		if (type1 == BoundType::INFTY || type2 == BoundType::INFTY) return BoundType::INFTY;
		if (type1 == BoundType::WEAK || type2 == BoundType::WEAK) return BoundType::WEAK;
		return BoundType::STRICT;
	}
	inline static BoundType getStrictestBoundType( BoundType type1, BoundType type2 )
	{
		if (type1 == BoundType::INFTY || type2 == BoundType::INFTY) return BoundType::INFTY;
		if (type1 == BoundType::STRICT || type2 == BoundType::STRICT) return BoundType::STRICT;
		return BoundType::WEAK;
	}
        
        inline static BoundType getOtherBoundType( BoundType type ) 
        {
            return (type == BoundType::INFTY)
                    ? BoundType::INFTY
                    : (type == BoundType::WEAK) ? BoundType::STRICT : BoundType::WEAK;
                
        }
}

