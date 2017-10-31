#pragma once

#include <vector>

namespace carl {
template<typename Number> class RealAlgebraicPoint;
}

#include "RealAlgebraicNumber.h"
//#include "RealAlgebraicNumberOperations.h"

namespace carl {

/**
 * Represent a multidimensional point consisting of one real algebraic number for
 * each coordinate axis.
 */
template<typename Number>
class RealAlgebraicPoint {
private:
	/**
	 * Numbers of this RealAlgebraicPoint.
	 */
	std::vector<RealAlgebraicNumber<Number>> numbers;

public:
	/**
	 * Create an empty Point of dimension 0.
	 */
	RealAlgebraicPoint() noexcept:
		numbers()
	{}

	/**
	 * Create a real algebraic point with the specified components.
	 * @param v pointers to real algebraic numbers
	 */
	explicit RealAlgebraicPoint(const std::vector<RealAlgebraicNumber<Number>>& v):
		numbers(v)
	{}

	/**
	 * Create a real algebraic point with the specified components.
	 * @param v pointers to real algebraic numbers
	 */
	explicit RealAlgebraicPoint(std::vector<RealAlgebraicNumber<Number>>&& v):
		numbers(std::move(v))
	{}

	/**
	 * Create a real algebraic point with the specified components from a list.
	 * @param v pointers to real algebraic numbers
	 */
	explicit RealAlgebraicPoint(const std::list<RealAlgebraicNumber<Number>>& v):
		numbers(v.begin(), v.end())
	{}

	/**
	 * Create a real algebraic point with the specified components from a list.
	 * @param v pointers to real algebraic numbers
	 */
	RealAlgebraicPoint(const std::initializer_list<RealAlgebraicNumber<Number>>& v):
		numbers(v.begin(), v.end())
	{}

	/** Give the number of components of this point.
	 * @return the dimension of this point
	 */
	std::size_t dim() const {
		return this->numbers.size();
	}

	/**
	 * Conjoin a point with a real algebraic number and return the conjoined point as new object.
	 * e.g.: a point of dimension n conjoined with
	 * a real algebraic number is a point of dimension
	 * n+1.
	 * @param r additional dimension given as real algebraic number
	 * @return real algebraic point with higher dimension
	 */
	RealAlgebraicPoint conjoin(const RealAlgebraicNumber<Number>& r) {
		RealAlgebraicPoint res = RealAlgebraicPoint(*this);
		res.numbers.push_back(r);
		return res;
	}
	
	/**
	 * Retrieve the number of this point at the given index.
	 * @param index Index.
	 * @return Value of this point at the given index.
	 */
	const RealAlgebraicNumber<Number>& operator[](std::size_t index) const {
		assert(index < this->numbers.size());
		return this->numbers[index];
	}
	
	/**
	 * Retrieve the number of this point at the given index.
	 * @param index Index.
	 * @return Value of this point at the given index.
	 */
	RealAlgebraicNumber<Number>& operator[](std::size_t index) {
		assert(index < this->numbers.size());
		return this->numbers[index];
	}
	
	template<typename Num>
	friend std::ostream& operator<<(std::ostream& os, const RealAlgebraicPoint<Num>& r);

	/**
	 * Check if two RealAlgebraicPoints are equal.
     * @param r RealAlgebraicPoint.
     * @return `this == r`.
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
 * @param os Output stream.
 * @param r RealAlgebraicPoint.
 * @return os.
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
