#include "BVValue.h"

namespace carl {

BVValue::BVValue(std::size_t _width, const mpz_class& _value) {
	// Obtain an mpz_t copy of _value
	mpz_t value;
	mpz_init(value);
	mpz_set(value, _value.get_mpz_t());

	// Remember whether the given value is negative.
	// The conversion is based on the mpz_export function, which ignores
	// the sign. Hence, we have to construct the two's complement
	// ourselves in case of negative numbers.
	bool valueNegative = (mpz_sgn(value) == -1);

	if (valueNegative) {
		// If the value is negative, increase it by 1 before conversion.
		// This way we only have to flip all bits afterwards.
		mpz_add_ui(value, value, 1);
	}

	// We export into an array of Base::block_type.
	// Calculate the number of needed blocks upfront
	std::size_t bitsPerBlock = sizeof(Base::block_type) * CHAR_BIT;
	std::size_t valueBits = mpz_sizeinbase(value, 2);
	std::size_t blockCount = (valueBits + bitsPerBlock - 1) / bitsPerBlock;

	{
		// The actual conversion from mpz_t to dynamic_bitset blocks
		auto bits = std::make_unique<Base::block_type[]>(blockCount);
		mpz_export(&bits[0], &blockCount, -1, sizeof(Base::block_type), -1, 0, value);

		// Import the blocks into mValue, and resize it afterwards to the desired size
		mValue.append(&bits[0], &bits[0] + blockCount);
		mValue.resize(_width);
	}

	// If the value was negative, finalize the construction of the two's complement
	// by inverting all bits
	if (valueNegative) {
		mValue.flip();
	}
}

BVValue BVValue::operator+(const BVValue& _other) const {
	assert(_other.width() == width());

	bool carry = false;
	Base sum(width());

	for (std::size_t i = 0; i < width(); ++i) {
		sum[i] = ((*this)[i] != _other[i]) != carry;
		carry = ((*this)[i] && _other[i]) || (carry && ((*this)[i] || _other[i]));
	}

	return BVValue(std::move(sum));
}

BVValue BVValue::concat(const BVValue& _other) const {
	Base concatenation(mValue);
	concatenation.resize(width() + _other.width());
	concatenation <<= _other.width();
	Base otherResized = static_cast<Base>(_other);
	otherResized.resize(concatenation.size());
	concatenation |= otherResized;
	return BVValue(std::move(concatenation));
}

BVValue BVValue::operator*(const BVValue& _other) const {
	BVValue product(width());
	Base summand(mValue);

	for (std::size_t i = 0; i < width(); ++i) {
		if (_other[i]) {
			product = product + BVValue(Base(summand));
		}
		summand <<= 1;
	}

	return product;
}

BVValue BVValue::divideSigned(const BVValue& _other) const {
	assert(width() == _other.width());

	bool firstNegative = (*this)[width() - 1];
	bool secondNegative = _other[_other.width() - 1];

	if (!firstNegative && !secondNegative) {
		return (*this) / _other;
	} else if (firstNegative && !secondNegative) {
		return -(-(*this) / _other);
	} else if (!firstNegative && secondNegative) {
		return -((*this) / -_other);
	} else {
		return -(*this) / -_other;
	}
}

BVValue BVValue::remSigned(const BVValue& _other) const {
	assert(width() == _other.width());

	bool firstNegative = (*this)[width() - 1];
	bool secondNegative = _other[_other.width() - 1];

	if (!firstNegative && !secondNegative) {
		return (*this) % _other;
	} else if (firstNegative && !secondNegative) {
		return -(-(*this) % _other);
	} else if (!firstNegative && secondNegative) {
		return (*this) % -_other;
	} else {
		return -((*this) % -_other);
	}
}

BVValue BVValue::modSigned(const BVValue& _other) const {
	assert(width() == _other.width());

	bool firstNegative = (*this)[width() - 1];
	bool secondNegative = _other[_other.width() - 1];

	BVValue absFirst(firstNegative ? -(*this) : (*this));
	BVValue absSecond(secondNegative ? -_other : _other);

	BVValue u = absFirst % absSecond;

	if (u.isZero()) {
		return u;
	} else if (!firstNegative && !secondNegative) {
		return u;
	} else if (firstNegative && !secondNegative) {
		return -u + _other;
	} else if (!firstNegative && secondNegative) {
		return (*this) + _other;
	} else {
		return -u;
	}
}

BVValue BVValue::extract(std::size_t _highest, std::size_t _lowest) const {
	assert(_highest < width() && _highest >= _lowest);
	BVValue extraction(_highest - _lowest + 1);

	for (std::size_t i = 0; i < extraction.width(); ++i) {
		extraction[i] = (*this)[_lowest + i];
	}

	return extraction;
}

BVValue BVValue::shift(const BVValue& _other, bool _left, bool _arithmetic) const {
	std::size_t firstSize = width() - 1;
	std::size_t highestRelevantPos = 0;

	bool fillWithOnes = !_left && _arithmetic && (*this)[width() - 1];

	while ((firstSize >>= 1) != 0)
		++highestRelevantPos;

	for (std::size_t i = highestRelevantPos + 1; i < _other.width(); ++i) {
		if (_other[i]) {
			Base allZero(width());
			return BVValue(fillWithOnes ? ~allZero : allZero);
		}
	}

	Base shifted(fillWithOnes ? ~mValue : mValue);
	std::size_t shiftBy = 1;

	for (std::size_t i = 0; i <= highestRelevantPos && i < _other.width(); ++i) {
		if (_other[i]) {
			if (_left) {
				shifted <<= shiftBy;
			} else {
				shifted >>= shiftBy;
			}
		}
		shiftBy *= 2;
	}

	return BVValue(fillWithOnes ? ~shifted : shifted);
}

BVValue BVValue::divideUnsigned(const BVValue& _other, bool _returnRemainder) const {
		assert(width() == _other.width());
		assert(Base(_other) != Base(_other.width(), 0));

		Base quotient(width());
		std::size_t quotientIndex = 0;
		Base divisor = static_cast<Base>(_other);
		Base remainder(mValue);

		while (!divisor[divisor.size() - 1] && remainder > divisor) {
			++quotientIndex;
			divisor <<= 1;
		}

		while (true) {
			if (remainder >= divisor) {
				quotient[quotientIndex] = true;
				// substract divisor from remainder
				bool carry = false;
				for (std::size_t i = divisor.find_first(); i < remainder.size(); ++i) {
					bool newRemainderI = (remainder[i] != divisor[i]) != carry;
					carry = (remainder[i] && carry && divisor[i]) || (!remainder[i] && (carry || divisor[i]));
					remainder[i] = newRemainderI;
				}
			}
			if (quotientIndex == 0) {
				break;
			}
			divisor >>= 1;
			--quotientIndex;
		}

		return BVValue(_returnRemainder ? std::move(remainder) : std::move(quotient));
	}

}