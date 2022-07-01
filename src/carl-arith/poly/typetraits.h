#pragma once

namespace carl {

template<typename T> struct is_polynomial_type: std::false_type {};
template<typename T> struct needs_cache_type : std::false_type {};
template<typename T> struct needs_context_type : std::false_type {};
template<typename T> struct is_factorized_type : std::true_type {};

}