/**
 * @file BVValue.h
 * @author Andreas Krueger <andreas.krueger@rwth-aachen.de>
 */

#pragma once

#include "../../numbers/numbers.h"
#include "../../util/boost_util.h"

#include <array>
#include <boost/dynamic_bitset.hpp>
#include <limits>
#include <memory>

namespace carl {
class BVValue {
public:
	using Base = boost::dynamic_bitset<uint>;

private:
	Base mValue;

	template<std::size_t len>
	explicit BVValue(const std::array<uint, len>& a)
		: mValue(a.begin(), a.end()) {}

public:
	BVValue() = default;

	explicit BVValue(Base&& value)
		: mValue(std::move(value)) {}

	explicit BVValue(std::size_t _width, uint _value = 0)
		: BVValue(std::array<uint, 1>({{_value}})) {
		mValue.resize(_width);
	}
#ifdef USE_CLN_NUMBERS
	explicit BVValue(std::size_t _width, const cln::cl_I& _value)
		: mValue(_width) {
		for (std::size_t i = 0; i < _width; ++i) {
			mValue[i] = cln::logbitp(i, _value);
		}
	}
#endif
	BVValue(std::size_t _width, const mpz_class& _value);

	template<typename BlockInputIterator>
	explicit BVValue(BlockInputIterator _first, BlockInputIterator _last)
		: mValue(_first, _last) {
	}

	template<typename Char, typename Traits, typename Alloc>
	explicit BVValue(const std::basic_string<Char, Traits, Alloc>& _s,
					 typename std::basic_string<Char, Traits, Alloc>::size_type _pos = 0,
					 typename std::basic_string<Char, Traits, Alloc>::size_type _n = std::basic_string<Char, Traits, Alloc>::npos)
		: mValue(_s, _pos, _n) {
	}

	explicit operator const Base&() const {
		return mValue;
	}

	const Base& base() const {
		return mValue;
	}

	std::size_t width() const {
		return mValue.size();
	}

	std::string toString() const {
		std::string output;
		boost::to_string(mValue, output);
		return "#b" + output;
	}

	bool isZero() const {
		return mValue.none();
	}

	BVValue rotateLeft(std::size_t _n) const {
		Base lowerPart(mValue);
		Base upperPart(mValue);

		lowerPart <<= _n % width();
		upperPart >>= width() - (_n % width());

		return BVValue(lowerPart ^ upperPart);
	}

	BVValue rotateRight(std::size_t _n) const {
		return rotateLeft(width() - (_n % width()));
	}

	BVValue repeat(std::size_t _n) const {
		assert(_n > 0);
		BVValue repeated(_n * width());
		// Todo: Implement with shifts instead of copying every single bit.
		for (std::size_t i = 0; i < repeated.width(); ++i) {
			repeated[i] = (*this)[i % width()];
		}
		return repeated;
	}

	BVValue extendUnsignedBy(std::size_t _n) const {
		Base copy(mValue);
		copy.resize(width() + _n);
		return BVValue(std::move(copy));
	}

	BVValue extendSignedBy(std::size_t _n) const {
		Base copy(mValue);
		copy.resize(width() + _n, (*this)[width() - 1]);
		return BVValue(std::move(copy));
	}

	Base::reference operator[](std::size_t _index) {
		assert(_index < width());
		return mValue[_index];
	}

	bool operator[](std::size_t _index) const {
		assert(_index < width());
		return mValue[_index];
	}

	BVValue operator+(const BVValue& _other) const;

	BVValue concat(const BVValue& _other) const;

	BVValue operator*(const BVValue& _other) const;

	BVValue divideSigned(const BVValue& _other) const;

	BVValue remSigned(const BVValue& _other) const;

	BVValue modSigned(const BVValue& _other) const;

	BVValue rightShiftArithmetic(const BVValue& _other) const {
		return shift(_other, false, true);
	}

	BVValue extract(std::size_t _highest, std::size_t _lowest) const;

	BVValue shift(const BVValue& _other, bool _left, bool _arithmetic = false) const;

	BVValue divideUnsigned(const BVValue& _other, bool _returnRemainder = false) const;
};

inline bool operator==(const BVValue& lhs, const BVValue& rhs) {
	return lhs.base() == rhs.base();
}

inline bool operator<(const BVValue& lhs, const BVValue& rhs) {
	return lhs.base() < rhs.base();
}

inline BVValue operator~(const BVValue& val) {
	return BVValue(~val.base());
}

inline BVValue operator-(const BVValue& val) {
	return ~val + BVValue(val.width(), 1);
}

inline BVValue operator-(const BVValue& lhs, const BVValue& rhs) {
	assert(lhs.width() == rhs.width());
	return lhs + (-rhs);
}

inline BVValue operator%(const BVValue& lhs, const BVValue& rhs) {
	return lhs.divideUnsigned(rhs, true);
}

inline BVValue operator/(const BVValue& lhs, const BVValue& rhs) {
	return lhs.divideUnsigned(rhs);
}

inline BVValue operator&(const BVValue& lhs, const BVValue& rhs) {
	assert(lhs.width() == rhs.width());
	return BVValue(lhs.base() & rhs.base());
}

inline BVValue operator|(const BVValue& lhs, const BVValue& rhs) {
	assert(lhs.width() == rhs.width());
	return BVValue(lhs.base() | rhs.base());
}

inline BVValue operator^(const BVValue& lhs, const BVValue& rhs) {
	assert(lhs.width() == rhs.width());
	return BVValue(lhs.base() ^ rhs.base());
}

inline BVValue operator<<(const BVValue& lhs, const BVValue& rhs) {
	return lhs.shift(rhs, true);
}

inline BVValue operator>>(const BVValue& lhs, const BVValue& rhs) {
	return lhs.shift(rhs, false, false);
}

inline std::ostream& operator<<(std::ostream& os, const BVValue& val) {
	return os << "#b" << static_cast<BVValue::Base>(val);
}

} // namespace carl

namespace std {
/**
 * Implements std::hash for bit vector values.
 */
template<>
struct hash<carl::BVValue> {
	/**
	 * @param _value The bit vector value to get the hash for.
	 * @return The hash of the given bit vector value.
	 */
	std::size_t operator()(const carl::BVValue& _value) const {
		return std::hash<carl::BVValue::Base>()(static_cast<carl::BVValue::Base>(_value));
	}
};
} // namespace std
