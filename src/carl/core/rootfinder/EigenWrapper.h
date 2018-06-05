#pragma once

#include <vector>

namespace carl {
namespace rootfinder {
namespace eigen {

/**
 * Compute the eigenvalues of 
 */
std::vector<double> root_approximation(const std::vector<double>& coeffs);

}
}
}
