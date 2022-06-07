#pragma once

namespace carl {

/**
* States whether a given type is an `Interval`.
* By default, a type is not.
*/
template <class Number>
struct is_interval_type : std::false_type {};

}