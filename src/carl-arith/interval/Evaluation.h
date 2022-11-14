#pragma once

#include <boost/logic/tribool.hpp>
#include <carl-arith/core/Relation.h>
#include "Interval.h"

namespace carl {

template<typename Number>
inline boost::tribool evaluate(Interval<Number> interval, Relation relation) {
    if (interval.is_positive()) {
        return evaluate(Sign::POSITIVE, relation);
    } else if (interval.is_negative()) {
        return evaluate(Sign::NEGATIVE, relation);
    } else if (interval.is_zero()) {
        return evaluate(Sign::ZERO, relation);
    } else if (interval.is_semi_negative()) {
        if (relation == Relation::GREATER) {
            return false;
        } else if (relation == Relation::LEQ) {
            return true;
        }
    } else if (interval.is_semi_positive()) {
        if (relation == Relation::LESS) {
            return false;
        } else if (relation == Relation::GEQ) {
            return true;
        }
    }
    return boost::indeterminate;
}

}