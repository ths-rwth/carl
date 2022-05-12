#pragma once

#include <carl/poly/umvpoly/functions/Complexity.h>

namespace carl {
    
/**
 * @return An approximation of the complexity of this constraint.
 */
template<typename Poly>
std::size_t complexity(const BasicConstraint<Poly>& c) {
	return 1 + complexity(c.lhs());
}

}
