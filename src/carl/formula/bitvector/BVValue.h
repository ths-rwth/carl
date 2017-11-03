/**
 * @file BVValue.h
 * @author Andreas Krueger <andreas.krueger@rwth-aachen.de>
 */

#pragma once

#include "../../numbers/numbers.h"

#include <boost/dynamic_bitset.hpp>
#include <limits>
#include <memory>

namespace carl
{
    class BVValue
    {
	public:
		using Base = boost::dynamic_bitset<uint>;
    private:
        Base mValue;

		template<std::size_t len>
		explicit BVValue(const std::array<uint,len>& a): mValue(a.begin(), a.end()) {}
        explicit BVValue(Base&& value): mValue(std::move(value)) {}

    public:
        BVValue() : mValue()
        {
        }

        explicit BVValue(std::size_t _width, uint _value = 0) :
            BVValue(std::array<uint,1>({{_value}}))
        {
			mValue.resize(_width);
        }
#ifdef USE_CLN_NUMBERS
        explicit BVValue(std::size_t _width, const cln::cl_I& _value) :
        	mValue(_width)
        {
            for(std::size_t i=0;i<_width;++i) {
                mValue[i] = cln::logbitp(i, _value);
            }
        }
#endif
        BVValue(std::size_t _width, const mpz_class& _value) :
        	mValue()
        {
            // Obtain an mpz_t copy of _value
            mpz_t value;
            mpz_init(value);
            mpz_set(value, _value.get_mpz_t());

            // Remember whether the given value is negative.
            // The conversion is based on the mpz_export function, which ignores
            // the sign. Hence, we have to construct the two's complement
            // ourselves in case of negative numbers.
            bool valueNegative = (mpz_sgn(value) == -1);

            if(valueNegative) {
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
            if(valueNegative) {
                mValue.flip();
            }
        }

        template <typename BlockInputIterator>
        explicit BVValue(BlockInputIterator _first, BlockInputIterator _last) :
        	mValue(_first, _last)
        {
        }

        template<typename Char, typename Traits, typename Alloc>
        explicit BVValue(const std::basic_string<Char,Traits,Alloc>& _s,
                         typename std::basic_string<Char, Traits, Alloc>::size_type _pos = 0,
                         typename std::basic_string<Char, Traits, Alloc>::size_type _n = std::basic_string<Char,Traits,Alloc>::npos) :
        	mValue(_s, _pos, _n)
        {
        }

        operator const Base&() const
        {
            return mValue;
        }

        std::size_t width() const
        {
            return mValue.size();
        }

        std::string toString() const
        {
            std::string output;
            boost::to_string(mValue, output);
            return "#b" + output;
        }

        bool isZero() const
        {
            return mValue.none();
        }

        BVValue operator-() const
        {
            return ~(*this) + BVValue(width(), 1);
        }

        BVValue operator~() const
        {
            return BVValue(~mValue);
        }

        BVValue rotateLeft(std::size_t _n) const
        {
            Base lowerPart(mValue);
            Base upperPart(mValue);

            lowerPart <<= _n % width();
            upperPart >>= width() - (_n % width());

            return BVValue(lowerPart ^ upperPart);
        }

        BVValue rotateRight(std::size_t _n) const
        {
            return rotateLeft(width() - (_n % width()));
        }

        BVValue repeat(std::size_t _n) const
        {
            assert(_n > 0);
            BVValue repeated(_n * width());

            for(std::size_t i=0;i<repeated.width();++i) {
                repeated[i] = (*this)[i % width()];
            }
            return repeated;
        }

        BVValue extendUnsignedBy(std::size_t _n) const
        {
            Base copy(mValue);
            copy.resize(width() + _n);
            return BVValue(std::move(copy));
        }

        BVValue extendSignedBy(std::size_t _n) const
        {
            Base copy(mValue);
            copy.resize(width() + _n, (*this)[width()-1]);
            return BVValue(std::move(copy));
        }

        friend std::ostream& operator<<(std::ostream& _out, const BVValue& _value)
        {
            return(_out << _value.toString());
        }

        bool operator==(const BVValue& _other) const
        {
            return mValue == _other.mValue;
        }

        bool operator<(const BVValue& _other) const
        {
            return mValue < _other.mValue;
        }

        Base::reference operator[](std::size_t _index)
        {
            assert(_index < width());
            return mValue[_index];
        }

        bool operator[](std::size_t _index) const
        {
            assert(_index < width());
            return mValue[_index];
        }

        BVValue operator+(const BVValue& _other) const
        {
            assert(_other.width() == width());

            bool carry = false;
            Base sum(width());

            for(std::size_t i=0;i<width();++i) {
                sum[i] = ((*this)[i] != _other[i]) != carry;
                carry = ((*this)[i] && _other[i]) || (carry && ((*this)[i] || _other[i]));
            }

            return BVValue(std::move(sum));
        }

        BVValue operator-(const BVValue& _other) const
        {
            assert(_other.width() == width());
            return (*this) + (-_other);
        }

        BVValue operator&(const BVValue& _other) const
        {
            assert(_other.width() == width());
            return BVValue(mValue & Base(_other));
        }

        BVValue operator|(const BVValue& _other) const
        {
            assert(_other.width() == width());
            return BVValue(mValue | Base(_other));
        }

        BVValue operator^(const BVValue& _other) const
        {
            assert(_other.width() == width());
            return BVValue(mValue ^ Base(_other));
        }

        BVValue concat(const BVValue& _other) const
        {
            Base concatenation(mValue);
            concatenation.resize(width() + _other.width());
            concatenation <<= _other.width();
            Base otherResized = _other;
            otherResized.resize(concatenation.size());
            concatenation |= otherResized;
            return BVValue(std::move(concatenation));
        }

        BVValue operator*(const BVValue& _other) const
        {
            BVValue product(width());
            Base summand(mValue);

            for(std::size_t i=0;i<width();++i) {
                if(_other[i]) {
                    product = product + BVValue(Base(summand));
                }
                summand <<= 1;
            }

            return product;
        }

        BVValue operator%(const BVValue& _other) const
        {
            return divideUnsigned(_other, true);
        }

        BVValue operator/(const BVValue& _other) const
        {
            return divideUnsigned(_other);
        }

        BVValue divideSigned(const BVValue& _other) const
        {
            assert(width() == _other.width());

            bool firstNegative = (*this)[width()-1];
            bool secondNegative = _other[_other.width()-1];

            if(! firstNegative && ! secondNegative) {
                return (*this) / _other;
            } else if(firstNegative && ! secondNegative) {
                return -(-(*this) / _other);
            } else if(! firstNegative && secondNegative) {
                return -((*this) / -_other);
            } else {
                return -(*this) / -_other;
            }
        }

        BVValue remSigned(const BVValue& _other) const
        {
            assert(width() == _other.width());

            bool firstNegative = (*this)[width()-1];
            bool secondNegative = _other[_other.width()-1];

            if(! firstNegative && ! secondNegative) {
                return (*this) % _other;
            } else if(firstNegative && ! secondNegative) {
                return -(-(*this) % _other);
            } else if(! firstNegative && secondNegative) {
                return (*this) % -_other;
            } else {
                return -((*this) % -_other);
            }
        }

        BVValue modSigned(const BVValue& _other) const
        {
            assert(width() == _other.width());

            bool firstNegative = (*this)[width()-1];
            bool secondNegative = _other[_other.width()-1];

            BVValue absFirst(firstNegative ? -(*this) : (*this));
            BVValue absSecond(secondNegative ? -_other : _other);

            BVValue u = absFirst % absSecond;

            if(u.isZero()) {
                return u;
            } else if(! firstNegative && ! secondNegative) {
                return u;
            } else if(firstNegative && ! secondNegative) {
                return -u + _other;
            } else if(! firstNegative && secondNegative) {
                return (*this) + _other;
            } else {
                return -u;
            }
        }

        BVValue operator<<(const BVValue& _other) const
        {
            return shift(_other, true);
        }

        BVValue operator>>(const BVValue& _other) const
        {
            return shift(_other, false, false);
        }

        BVValue rightShiftArithmetic(const BVValue& _other) const
        {
            return shift(_other, false, true);
        }

        BVValue extract(std::size_t _highest, std::size_t _lowest) const
        {
            assert(_highest < width() && _highest >= _lowest);
            BVValue extraction(_highest - _lowest + 1);

            for(std::size_t i=0;i<extraction.width();++i) {
                extraction[i] = (*this)[_lowest + i];
            }

            return extraction;
        }

    private:
        BVValue shift(const BVValue& _other, bool _left, bool _arithmetic = false) const
        {
            std::size_t firstSize = width() - 1;
            std::size_t highestRelevantPos = 0;

            bool fillWithOnes = !_left && _arithmetic && (*this)[width()-1];

            while((firstSize >>= 1) != 0)
                ++highestRelevantPos;

            for(std::size_t i=highestRelevantPos+1;i<_other.width();++i) {
                if(_other[i]) {
                    Base allZero(width());
                    return BVValue(fillWithOnes ? ~allZero : allZero);
                }
            }

            Base shifted(fillWithOnes ? ~mValue : mValue);
            std::size_t shiftBy = 1;

            for(std::size_t i=0;i<=highestRelevantPos && i < _other.width();++i) {
                if(_other[i]) {
                    if(_left) {
                        shifted <<= shiftBy;
                    } else {
                        shifted >>= shiftBy;
                    }
                }
                shiftBy *= 2;
            }

            return BVValue(fillWithOnes ? ~shifted : shifted);
        }

        BVValue divideUnsigned(const BVValue& _other, bool _returnRemainder = false) const
        {
            assert(width() == _other.width());
            assert(Base(_other) != Base(_other.width(), 0));

            Base quotient(width());
            std::size_t quotientIndex = 0;
            Base divisor = _other;
            Base remainder(mValue);

            while(! divisor[divisor.size()-1] && remainder > divisor) {
                ++quotientIndex;
                divisor <<= 1;
            }

            while(true) {
                if(remainder >= divisor) {
                    quotient[quotientIndex] = true;
                    // substract divisor from remainder
                    bool carry = false;
                    for(std::size_t i=divisor.find_first();i<remainder.size();++i) {
                        bool newRemainderI = (remainder[i] != divisor[i]) != carry;
                        carry = (remainder[i] && carry && divisor[i]) || (! remainder[i] && (carry || divisor[i]));
                        remainder[i] = newRemainderI;
                    }
                }
                if(quotientIndex == 0) {
                    break;
                }
                divisor >>= 1;
                --quotientIndex;
            }

            return BVValue(_returnRemainder ? std::move(remainder) : std::move(quotient));
        }
    };
}

namespace std
{
    /**
     * Implements std::hash for bit vector values.
     * TODO: Make more efficient (currently uses dynamic_bitset<> conversion to string).
     * See also: https://stackoverflow.com/q/3896357, https://svn.boost.org/trac/boost/ticket/2841
     */
    template <>
    struct hash<carl::BVValue>
    {
        public:

        /**
         * @param _value The bit vector value to get the hash for.
         * @return The hash of the given bit vector value.
         */
        size_t operator()(const carl::BVValue& _value) const
        {
            return std::hash<std::string>()(_value.toString());
        }
    };
}
