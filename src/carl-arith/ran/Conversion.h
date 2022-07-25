#pragma once

#include <carl-common/config.h>

namespace carl {

template<typename T, typename = std::enable_if_t<is_ran_type<T>::value>>
T convert(const T& r) {
    return r;
}

#ifdef USE_LIBPOLY
template<typename R>
LPRealAlgebraicNumber convert(const carl::IntRepRealAlgebraicNumber<R>& r) {
    if(r.is_numeric()){
        return LPRealAlgebraicNumber(r.value()) ; 
    }
    return LPRealAlgebraicNumber(r.polynomial(), r.interval());
}
template<typename R>
IntRepRealAlgebraicNumber<R> convert(const LPRealAlgebraicNumber& r) {
    if(r.is_numeric()){
        return IntRepRealAlgebraicNumber<R>(r.value());
    } 
    return IntRepRealAlgebraicNumber<R>(r.polynomial(), r.interval());
}
#endif

}; // namespace carl