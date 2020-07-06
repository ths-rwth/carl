#pragma once

#include <boost/logic/tribool.hpp>
#include <carl/core/Relation.h>
#include "Interval.h"

namespace carl {

template<typename Number>
inline boost::tribool evaluate(Interval<Number> interval, Relation relation) {
    if (interval.isPositive()) {
        return evaluate(Sign::POSITIVE, relation);
    } else if (interval.isNegative()) {
        return evaluate(Sign::NEGATIVE, relation);
    } else if (interval.isZero()) {
        return evaluate(Sign::ZERO, relation);
    } else if (interval.isSemiNegative()) {
        if (relation == Relation::GREATER) {
            return false;
        } else if (relation == Relation::LEQ) {
            return true;
        }
    } else if (interval.isSemiPositive()) {
        if (relation == Relation::LESS) {
            return false;
        } else if (relation == Relation::GEQ) {
            return true;
        }
    }
    return boost::indeterminate;
}

}