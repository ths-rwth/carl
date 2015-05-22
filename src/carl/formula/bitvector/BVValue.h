/**
 * @file BVValue.h
 * @author Andreas Krueger <andreas.krueger@rwth-aachen.de>
 */

#pragma once

#include <boost/dynamic_bitset.hpp>
#include <cln/cln.h>
#include <gmpxx.h>
#include <limits.h>

namespace carl
{
    class BVValue
    {
    private:
        boost::dynamic_bitset<> mValue;

    public:
        BVValue() : mValue()
        {
        }

        BVValue(std::size_t _width, unsigned long _value = 0) :
        mValue(_width, _value)
        {
        }

        BVValue(std::size_t _width, const cln::cl_I _value) :
        mValue(_width)
        {
            for(std::size_t i=0;i<_width;++i) {
                mValue[i] = cln::logbitp(i, _value);
            }
        }

        BVValue(std::size_t _width, const mpz_class _value) :
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

            // We export into an array of boost::dynamic_bitset<>::block_type.
            // Calculate the number of needed blocks upfront
            std::size_t bitsPerBlock = sizeof(boost::dynamic_bitset<>::block_type) * CHAR_BIT;
            std::size_t valueBits = mpz_sizeinbase(value, 2);
            std::size_t blockCount = (valueBits + bitsPerBlock - 1) / bitsPerBlock;

            // The actual conversion from mpz_t to dynamic_bitset blocks
            auto bits = new boost::dynamic_bitset<>::block_type[blockCount];
            mpz_export(&bits[0], &blockCount, -1, sizeof(boost::dynamic_bitset<>::block_type), -1, 0, value);

            // Import the blocks into mValue, and resize it afterwards to the desired size
            mValue.append(&bits[0], &bits[0] + blockCount);
            mValue.resize(_width);
            delete[] bits;

            // If the value was negative, finalize the construction of the two's complement
            // by inverting all bits
            if(valueNegative) {
                mValue.flip();
            }
        }

        BVValue(const BVValue& _other) :
        mValue(_other())
        {
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

        boost::dynamic_bitset<> operator()() const
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

        boost::dynamic_bitset<>::reference operator[](std::size_t _index)
        {
            assert(_index < width());
            return mValue[_index];
        }

        bool operator[](std::size_t _index) const
        {
            assert(_index < width());
            return mValue[_index];
        }
    };
}

namespace std
{
    /**
     * Implements std::hash for bit vector values.
     * TODO: Make more efficient (currently uses dynamic_bitset<> conversion to string).
     * See also: https://stackoverflow.com/q/3896357
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

