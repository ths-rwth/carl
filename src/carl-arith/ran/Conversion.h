#pragma once

#include<carl-common/config.h>

namespace carl {

template<typename T, typename = std::enable_if_t<is_ran_type<T>::value>>
T convert(const T& r) {
    return r;
}

#ifdef USE_LIBPOLY
template<typename R>
LPRealAlgebraicNumber convert(const carl::IntRepRealAlgebraicNumber<R>& r) {
    return LPPolynomial::RootType(); // TODO add conversion here
}
template<typename R>
carl::IntRepRealAlgebraicNumber<R> convert(const LPRealAlgebraicNumber& r) {
    return carl::IntRepRealAlgebraicNumber<R>(); // TODO add conversion here
}
#endif

}