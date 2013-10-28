/* 
 * File:   BoundType.h
 * Author: stefan
 *
 * Created on October 28, 2013, 11:44 AM
 */

#pragma once

enum BoundType {
                /// the given bound is compared by a strict ordering relation
                STRICT_BOUND,
                /// the given bound is compared by a weak ordering relation
                WEAK_BOUND,
                /// the given bound is interpreted as minus or plus infinity depending on whether it is the left or the right bound
                INFINITY_BOUND
            };

