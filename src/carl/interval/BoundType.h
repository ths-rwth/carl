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
                STRICT,
                /// the given bound is compared by a weak ordering relation
                WEAK,
                /// the given bound is interpreted as minus or plus infinity depending on whether it is the left or the right bound
                INFTY
            };
    
	inline static BoundType getWeakestBoundType( BoundType type1, BoundType type2 )
	{
		return (type1 == BoundType::INFTY || type2 == BoundType::INFTY)
		? BoundType::INFTY : (type1 == BoundType::WEAK || type2 == BoundType::WEAK) ? BoundType::WEAK : BoundType::STRICT;
	}
        
        inline static BoundType getOtherBoundType( BoundType type ) 
        {
            return (type == BoundType::INFTY)
                    ? BoundType::INFTY
                    : (type == BoundType::WEAK) ? BoundType::STRICT : BoundType::WEAK;
                
        }

}

