#pragma once

#include <carl-common/config.h>

namespace carl {

template<typename T, std::enable_if_t<is_ran_type<T>::value, int> = 0>
inline T convert(const T& r) {
    return r;
}

namespace convert_ran {
template<typename T, typename S>
struct ConvertHelper {};

#ifdef USE_LIBPOLY
template<typename R>
struct ConvertHelper<LPRealAlgebraicNumber, IntRepRealAlgebraicNumber<R>> {
    static LPRealAlgebraicNumber convert(const IntRepRealAlgebraicNumber<R>& r) {
        if(r.is_numeric()){
            return LPRealAlgebraicNumber(r.value()) ; 
        }
        return LPRealAlgebraicNumber(r.polynomial(), r.interval());
    }
};

template<typename R>
struct ConvertHelper<IntRepRealAlgebraicNumber<R>, LPRealAlgebraicNumber> {
    static IntRepRealAlgebraicNumber<R> convert(const LPRealAlgebraicNumber& r) {
        if(r.is_numeric()){
            return IntRepRealAlgebraicNumber<R>(r.value());
        } 
        return IntRepRealAlgebraicNumber<R>(r.polynomial(), r.interval());
    }
};
#endif
}

template<typename T, typename S, std::enable_if_t<is_ran_type<T>::value && is_ran_type<S>::value && !std::is_same<T,S>::value, int> = 0>
inline T convert(const S& r) {
    return convert_ran::ConvertHelper<T,S>::convert(r);
}

}; // namespace carl