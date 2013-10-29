/* 
 * File:   BoundType.h
 * Author: stefan
 *
 * Created on October 28, 2013, 11:44 AM
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
            
}

