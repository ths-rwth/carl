#pragma once

#include "boost_util.h"

#include <iostream>

#include <boost/dynamic_bitset.hpp>

namespace carl {
	
	/**
	 * This class is a simple wrapper around boost::dynamic_bitset.
	 * Its purpose is to allow for on-the-fly resizing of the bitset.
	 * Formally, a Bitset object represents an infinite bitset that starts with the bits stored in mData extended by mDefault.
	 * Whenever a bit is written that is not yet stored explicitly in mData or two Bitset objects with different mData sizes are involved, the size of mData is expanded transparently.
	 *
	 * Note that some operations only make sense for a certain value of `mDefault`.
	 * For example, `any()` or `none()` require `mDefault` to be `false`.
	 */
	class Bitset {
	public:
		friend struct std::hash<carl::Bitset>;

		/// Underlying storage type.
		using BaseType = boost::dynamic_bitset<>;
		/// Sentinel element for iteration.
		static auto constexpr npos = BaseType::npos;
		/// Number of bits in each storage block.
		static auto constexpr bits_per_block = BaseType::bits_per_block;
		
		/**
		 * Iterate for iterate over all bits of a Bitset that are set to true.
		 *
		 * If you want to iterate of all bits that are false use `operator~()`.
		 */
		struct iterator {
		private:
			/// The Bitset iterated over.
			const Bitset& mBitset;
			/// The current bit.
			std::size_t mBit;
			
			/// Check that two iterators speak about the same Bitset.
			bool compatible(const iterator& rhs) const {
				return &mBitset == &rhs.mBitset;
			}
		public:
			/// Construct a new iterator from a Bitset and a bit.
			iterator(const Bitset& b, std::size_t bit): mBitset(b), mBit(bit) {}
			
			/// Retrieve the index into the Bitset.
			operator std::size_t() const {
				return mBit;
			}
			/// Retrieve the index into the Bitset.
			std::size_t operator*() const {
				return mBit;
			}
			/// Step to the next bit that is set to true.
			iterator& operator++() {
				mBit = mBitset.find_next(mBit);
				return *this;
			}
			/// Step to the next bit that is set to true.
			iterator operator++(int) {
				iterator res(*this);
				++(*this);
				return res;
			}
			/// Compare two iterators. Asserts that they are compatible.
			bool operator==(const iterator& rhs) const {
				assert(compatible(rhs));
				return mBit == rhs.mBit;
			}
			/// Compare two iterators. Asserts that they are compatible.
			bool operator!=(const iterator& rhs) const {
				assert(compatible(rhs));
				return !(*this == rhs);
			}
			/// Compare two iterators. Asserts that they are compatible.
			bool operator<(const iterator& rhs) const {
				assert(compatible(rhs));
				return mBit < rhs.mBit;
			}
		};
		
	private:
		/// The actual data.
		mutable BaseType mData;
		/// The default value for bits beyond mData.
		bool mDefault;
		/// Resize mData to hold at least pos bits.
		void ensureSize(std::size_t pos) {
			if (pos >= mData.size()) {
				mData.resize(pos+1);
			}
		}
	public:
		/// Create an empty bitset.
		explicit Bitset(bool defaultValue = false): mDefault(defaultValue) {}
		/// Create a bitset from a BaseType object.
		Bitset(BaseType&& base, bool defaultValue): mData(std::move(base)), mDefault(defaultValue) {}
		/// Create a bitset from a list of bits indices that shall be set to true.
		Bitset(const std::initializer_list<std::size_t>& bits, bool defaultValue = false): mDefault(defaultValue) {
			for (auto b: bits) {
				set(b);
			}
		}
		
		/// Resize the Bitset to hold at least num_bits bits. New bits are set to the given value.
		auto resize(std::size_t num_bits, bool value) const {
			return mData.resize(num_bits, value);
		}
		/// Resize the Bitset to hold at least num_bits bits. New bits are set to mDefault.
		auto resize(std::size_t num_bits) const {
			return mData.resize(num_bits, mDefault);
		}
		
		/// Sets all bits to false that are true in rhs.
		Bitset& operator-=(const Bitset& rhs) {
			assert(mDefault == rhs.mDefault);
			alignSize(*this, rhs);
			mData -= rhs.mData;
			return *this;
		}
		/// Computes the bitwise and with rhs.
		Bitset& operator&=(const Bitset& rhs) {
			alignSize(*this, rhs);
			mData &= rhs.mData;
			return *this;
		}
		/// Computes the bitwise or with rhs.
		Bitset& operator|=(const Bitset& rhs) {
			alignSize(*this, rhs);
			mData |= rhs.mData;
			return *this;
		}
		
		/// Sets the given bit to a value, true by default.
		Bitset& set(std::size_t n, bool value = true) {
			ensureSize(n);
			mData.set(n, value);
			return *this;
		}
		/// Sets the a range of bits to a value, true by default.
		Bitset& set_interval(std::size_t start, std::size_t end, bool value = true) {
			ensureSize(end);
			for (; start <= end; start++) {
				mData.set(start, value);
			}
			return *this;
		}
		/// Resets a bit to false.
		Bitset& reset(std::size_t n) {
			ensureSize(n);
			mData.reset(n);
			return *this;
		}
		/// Retrieves the value of the given bit.
		bool test(std::size_t n) const {
			if (n >= mData.size()) {
				return mDefault;
			}
			return mData.test(n);
		}
		/// Checks if any bits are set to true. Asserts that mDefault is false.
		bool any() const {
			assert(!mDefault);
			return mData.any();
		}
		/// Checks if no bits are set to true. Asserts that mDefault is false.
		bool none() const {
			assert(!mDefault);
			return mData.none();
		}
		
		/// Counts the number of bits that are set to true. Asserts that mDefault is false.
		auto count() const noexcept {
			assert(!mDefault);
			return mData.count();
		}
		
		/// Retrieves the size of mData.
		auto size() const {
			return mData.size();
		}
		/// Retrieves the number of blocks used to store mData.
		auto num_blocks() const {
			return mData.num_blocks();
		}
		/// Checks wether the bits set is a subset of the bits set in rhs.
		auto is_subset_of(const Bitset& rhs) const {
			if (mDefault && !rhs.mDefault) {
				return false;
			}
			alignSize(*this, rhs);
			return mData.is_subset_of(rhs.mData);
		}
		/// Retrieves the index of the first bit that is set to true.
		std::size_t find_first() const {
			return mData.find_first();
		}
		/// Retrieves the index of the first bit set to true after the given position.
		std::size_t find_next(std::size_t pos) const {
			return mData.find_next(pos);
		}
		/// Returns an iterator to the first bit that is set to true.
		iterator begin() const {
			return iterator(*this, find_first());
		}
		/// Returns an past-the-end iterator.
		iterator end() const {
			return iterator(*this, npos);
		}
		
		/// Ensures that the explicitly stored bits of lhs and rhs have the same size.
		friend void alignSize(const Bitset& lhs, const Bitset& rhs) {
			if (lhs.size() < rhs.size()) {
				lhs.resize(rhs.size());
			} else if (lhs.size() > rhs.size()) {
				rhs.resize(lhs.size());
			}
		}
		
		/// Compares lhs and rhs.
		friend bool operator==(const Bitset& lhs, const Bitset& rhs) {
			alignSize(lhs, rhs);
			return (lhs.mData == rhs.mData) && (lhs.mDefault == rhs.mDefault);
		}
		/// Compares lhs and rhs according to some order.
		friend bool operator<(const Bitset& lhs, const Bitset& rhs) {
			if (lhs.size() < rhs.size()) {
				return true;
			}
			if (lhs.size() > rhs.size()) {
				return false;
			}
			return lhs.mData < rhs.mData;
		}
		
		/// Returns the bitwise negation of lhs.
		friend Bitset operator~(const Bitset& lhs) {
			return Bitset(~lhs.mData, !lhs.mDefault);
		}
		/// Returns the bitwise `and` of lhs and rhs.
		friend Bitset operator&(const Bitset& lhs, const Bitset& rhs) {
			alignSize(lhs, rhs);
			return Bitset(lhs.mData & rhs.mData, lhs.mDefault && rhs.mDefault);
		}
		/// Returns the bitwise `or` of lhs and rhs.
		friend Bitset operator|(const Bitset& lhs, const Bitset& rhs) {
			alignSize(lhs, rhs);
			return Bitset(lhs.mData | rhs.mData, lhs.mDefault || rhs.mDefault);
		}
		
		/// Outputs `b` to `os` using the format `<explicit bits>[<default>]`.
		friend std::ostream& operator<<(std::ostream& os, const Bitset& b) {
			return os << b.mData << '|' << b.mDefault;
		}
	};
}

namespace std {

template<>
struct hash<carl::Bitset> {
	std::size_t operator()(const carl::Bitset& bs) const {
		return std::hash<carl::Bitset::BaseType>()(bs.mData);
	}
};

}