/**
 * @file BVValue.h
 * @author Andreas Krueger <andreas.krueger@rwth-aachen.de>
 */

#pragma once

#include "../../numbers/numbers.h"
#include "../../util/Bitset.h"

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
	explicit BVValue(Base&& value)
		: mValue(std::move(value)) {}

public:
	BVValue() = default;

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

	BVValue operator-() const {
		return ~(*this) + BVValue(width(), 1);
	}

	BVValue operator~() const {
		return BVValue(~mValue);
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

	friend std::ostream& operator<<(std::ostream& _out, const BVValue& _value) {
		return (_out << _value.toString());
	}

	bool operator==(const BVValue& _other) const {
		return mValue == _other.mValue;
	}

	bool operator<(const BVValue& _other) const {
		return mValue < _other.mValue;
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

	BVValue operator-(const BVValue& _other) const {
		assert(_other.width() == width());
		return (*this) + (-_other);
	}

	BVValue operator&(const BVValue& _other) const {
		assert(_other.width() == width());
		return BVValue(mValue & Base(_other));
	}

	BVValue operator|(const BVValue& _other) const {
		assert(_other.width() == width());
		return BVValue(mValue | Base(_other));
	}

	BVValue operator^(const BVValue& _other) const {
		assert(_other.width() == width());
		return BVValue(mValue ^ Base(_other));
	}

	BVValue concat(const BVValue& _other) const;

	BVValue operator*(const BVValue& _other) const;

	BVValue operator%(const BVValue& _other) const {
		return divideUnsigned(_other, true);
	}

	BVValue operator/(const BVValue& _other) const {
		return divideUnsigned(_other);
	}

	BVValue divideSigned(const BVValue& _other) const;

	BVValue remSigned(const BVValue& _other) const;

	BVValue modSigned(const BVValue& _other) const;

	BVValue operator<<(const BVValue& _other) const {
		return shift(_other, true);
	}

	BVValue operator>>(const BVValue& _other) const {
		return shift(_other, false, false);
	}

	BVValue rightShiftArithmetic(const BVValue& _other) const {
		return shift(_other, false, true);
	}

	BVValue extract(std::size_t _highest, std::size_t _lowest) const;

private:
	BVValue shift(const BVValue& _other, bool _left, bool _arithmetic = false) const;

	BVValue divideUnsigned(const BVValue& _other, bool _returnRemainder = false) const;
};
} // namespace carl

namespace std {
/**
 * Implements std::hash for bit vector values.
 * TODO: Make more efficient (currently uses dynamic_bitset<> conversion to string).
 * See also: https://stackoverflow.com/q/3896357, https://svn.boost.org/trac/boost/ticket/2841
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
