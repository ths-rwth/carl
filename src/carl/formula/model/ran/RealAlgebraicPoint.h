/* 
 * @file RealAlgebraicPoint.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <vector>

namespace carl {
template<typename Number> class RealAlgebraicPoint;
}

#include "RealAlgebraicNumber.h"
//#include "RealAlgebraicNumberOperations.h"

namespace carl {

/**
 * A RealAlgebraicPoint represents a multidimensional point that consists of real algebraic numbers.
 */
template<typename Number>
class RealAlgebraicPoint {
private:
	/**
	 * Numbers of this RealAlgebraicPoint.
	 */
	std::vector<RealAlgebraicNumber<Number>> mNumbers;

public:
	/**
	 * Creates an empty Point of dimension 0.
	 */
	RealAlgebraicPoint() noexcept = default;

	/**
	 * Creates a real algebraic point with the specified components.
	 * @param v pointers to real algebraic numbers
	 */
	explicit RealAlgebraicPoint(const std::vector<RealAlgebraicNumber<Number>>& v):
		mNumbers(v)
	{}

	/**
	 * Creates a real algebraic point with the specified components.
	 * @param v pointers to real algebraic numbers
	 */
	explicit RealAlgebraicPoint(std::vector<RealAlgebraicNumber<Number>>&& v):
		mNumbers(std::move(v))
	{}

	/**
	 * Creates a real algebraic point with the specified components from a list.
	 * @param v pointers to real algebraic numbers
	 */
	explicit RealAlgebraicPoint(const std::list<RealAlgebraicNumber<Number>>& v):
		mNumbers(v.begin(), v.end())
	{}

	/**
	 * Creates a real algebraic point with the specified components from a list.
	 * @param v pointers to real algebraic numbers
	 */
	RealAlgebraicPoint(const std::initializer_list<RealAlgebraicNumber<Number>>& v):
		mNumbers(v.begin(), v.end())
	{}

	/** Gives the number of components of this point.
	 * @return the dimension of this point
	 */
	std::size_t dim() const {
		return mNumbers.size();
	}

	/**
	 * Conjoins a point with a real algebraic number and returns the conjoined point as new object.
	 * e.g.: a point of dimension n conjoined with
	 * a real algebraic number is a point of dimension
	 * n+1.
	 * @param r additional dimension given as real algebraic number
	 * @return real algebraic point with higher dimension
	 */
	RealAlgebraicPoint conjoin(const RealAlgebraicNumber<Number>& r) {
		RealAlgebraicPoint res = RealAlgebraicPoint(*this);
		res.mNumbers.push_back(r);
		return res;
	}
	
	/**
	 * Retrieves the number of this point at the given index.
	 * @param index Index.
	 * @return Value of this point at the given index.
	 */
	const RealAlgebraicNumber<Number>& operator[](std::size_t index) const {
		assert(index < mNumbers.size());
		return mNumbers[index];
	}
	
	/**
	 * Retrieves the number of this point at the given index.
	 * @param index Index.
	 * @return Value of this point at the given index.
	 */
	RealAlgebraicNumber<Number>& operator[](std::size_t index) {
		assert(index < mNumbers.size());
		return mNumbers[index];
	}
};

/**
 * Checks if two RealAlgebraicPoints are equal.
 * @param lhs RealAlgebraicPoint.
 * @param rhs RealAlgebraicPoint.
 * @return `lhs == rhs`.
 */
template<typename Number>
bool operator==(RealAlgebraicPoint<Number>& lhs, RealAlgebraicPoint<Number>& rhs) {
	if (lhs.dim() != rhs.dim()) return false;
	std::not_equal_to<Number> neq;
	for (std::size_t i = 0; i < lhs.dim(); ++i) {
		if (neq(lhs[i], rhs[i])) return false;
	}
	return true;
}

/**
 * Streaming operator for a RealAlgebraicPoint.
 * @param os Output stream.
 * @param r RealAlgebraicPoint.
 * @return os.
 */
template<typename Number>
std::ostream& operator<<(std::ostream& os, const RealAlgebraicPoint<Number>& r) {
	os << "(";
	for (std::size_t i = 0; i < r.dim(); ++i) {
		if (i > 0) os << ", ";
		os << r[i];
	}
	os << ")";
	return os;
}

}
