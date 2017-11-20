#pragma once

#include <vector>

namespace carl {
template<typename Number> class RealAlgebraicPoint;
}

#include "RealAlgebraicNumber.h"

namespace carl {

/**
 * Represent a multidimensional point whose components are algebraic reals.
 */
template<typename Number>
class RealAlgebraicPoint {
private:
  std::vector<RealAlgebraicNumber<Number>> numbers;

  // This class is just a thin wrapper around vector to have a clearer semantic
  // meaning.
public:
  /**
   * Create an empty Point of dimension 0.
   */
  RealAlgebraicPoint() noexcept:
    numbers()
  {}

  /**
   * Convert from a vector using its numbers in the same order as components.
   */
  explicit RealAlgebraicPoint(const std::vector<RealAlgebraicNumber<Number>>& v):
    numbers(v)
  {}

  /**
   * Convert from a vector using its numbers in the same order as components.
   */
  explicit RealAlgebraicPoint(std::vector<RealAlgebraicNumber<Number>>&& v):
    numbers(std::move(v))
  {}

  /**
   * Convert from a list using its numbers in the same order as components.
   */
  explicit RealAlgebraicPoint(const std::list<RealAlgebraicNumber<Number>>& v):
    numbers(v.begin(), v.end())
  {}

  /**
   * Convert from a initializer_list using its numbers in the same order as components.
   */
  RealAlgebraicPoint(const std::initializer_list<RealAlgebraicNumber<Number>>& v):
    numbers(v.begin(), v.end())
  {}

  /**
   * Give the dimension/number of components of this point.
   */
  std::size_t dim() const {
    return this->numbers.size();
  }

  /**
   * Get a lower dimensional copy that contains only the first k components.
   */
  RealAlgebraicPoint prefix(size_t k) const {
    assert(k <= numbers.size());
    std::vector<RealAlgebraicNumber<Number>> copy(k);
    std::copy_n(numbers.begin(),k,copy.begin());
    return RealAlgebraicPoint(copy);
  }

  /**
   * Create a new point with another given component added at the end of this
   * point, thereby increasing its dimension by 1. The original point remains
   * untouched.
   */
  RealAlgebraicPoint conjoin(const RealAlgebraicNumber<Number>& r) {
    RealAlgebraicPoint res = RealAlgebraicPoint(*this);
    res.numbers.push_back(r);
    return res;
  }

  /**
   * Retrieve the component of this point at the given index.
   */
  const RealAlgebraicNumber<Number>& operator[](std::size_t index) const {
    assert(index < this->numbers.size());
    return this->numbers[index];
  }

  /**
   * Retrieve the component of this point at the given index.
   */
  RealAlgebraicNumber<Number>& operator[](std::size_t index) {
    assert(index < this->numbers.size());
    return this->numbers[index];
  }

  template<typename Num>
  friend std::ostream& operator<<(std::ostream& os, const RealAlgebraicPoint<Num>& r);

  /**
   * Check if two RealAlgebraicPoints are equal.
   */
  bool operator==(RealAlgebraicPoint<Number>& r) {
    if (this->dim() != r.dim()) return false;
    std::not_equal_to<Number> neq;
    for (std::size_t i = 0; i < this->numbers.size(); i++) {
      if (neq(this->numbers[i], r.numbers[i])) return false;
    }
    return true;
  }
};

/**
 * Streaming operator for a RealAlgebraicPoint.
 */
template<typename Number>
std::ostream& operator<<(std::ostream& os, const RealAlgebraicPoint<Number>& r) {
  os << "(";
  for (std::size_t i = 0; i < r.dim(); i++) {
    if (i > 0) os << ", ";
    os << r.numbers[i];
  }
  os << ")";
  return os;
}

}
