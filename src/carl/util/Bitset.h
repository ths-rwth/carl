#pragma once

#include <iostream>

#include <boost/dynamic_bitset.hpp>

namespace carl {
	
	/**
	 * This class is a simple wrapper around boost::dynamic_bitset.
	 * Its purpose is to allow for on-the-fly resizing of the bitset.
	 * Formally, a Bitset object represents an infinite bitset that starts with the bits stored in mData extended by mDefault.
	 * Whenever a bit is written that is not yet stored explicitly in mData or two Bitset objects with different mData sizes are involved, the size of mData is expanded transparently.
	 */
	class Bitset {
	public:
		using BaseType = boost::dynamic_bitset<>;
		static auto constexpr npos = BaseType::npos;
		static auto constexpr bits_per_block = BaseType::bits_per_block;
		
		struct iterator {
		private:
			const Bitset& mBitset;
			std::size_t mBit;
			
			bool compatible(const iterator& rhs) const {
				return &mBitset == &rhs.mBitset;
			}
		public:
			iterator(const Bitset& b, std::size_t bit): mBitset(b), mBit(bit) {};
			
			operator std::size_t() const {
				return mBit;
			}
			std::size_t operator*() const {
				return mBit;
			}
			iterator& operator++() {
				mBit = mBitset.find_next(mBit);
				return *this;
			}
			iterator operator++(int) {
				iterator res(*this);
				++(*this);
				return res;
			}
			bool operator==(const iterator& rhs) const {
				assert(compatible(rhs));
				return mBit == rhs.mBit;
			}
			bool operator!=(const iterator& rhs) const {
				assert(compatible(rhs));
				return !(*this == rhs);
			}
			bool operator<(const iterator& rhs) const {
				assert(compatible(rhs));
				return mBit < rhs.mBit;
			}
		};
		
	private:
		mutable BaseType mData;
		bool mDefault;
		void ensureSize(std::size_t pos) {
			if (pos >= mData.size()) {
				mData.resize(pos+1);
			}
		}
	public:
		Bitset(bool defaultValue = false): mDefault(defaultValue) {}
		Bitset(BaseType&& base, bool defaultValue): mData(std::move(base)), mDefault(defaultValue) {}
		Bitset(const std::initializer_list<std::size_t>& bits, bool defaultValue = false): mDefault(defaultValue) {
			for (auto b: bits) set(b);
		}
		
		auto resize(std::size_t num_bits, bool value) const {
			return mData.resize(num_bits, value);
		}
		auto resize(std::size_t num_bits) const {
			return mData.resize(num_bits, mDefault);
		}
		
		Bitset& operator-=(const Bitset& rhs) {
			assert(mDefault == rhs.mDefault);
			alignSize(*this, rhs);
			mData -= rhs.mData;
			return *this;
		}
		Bitset& operator&=(const Bitset& rhs) {
			assert(mDefault == rhs.mDefault);
			alignSize(*this, rhs);
			mData &= rhs.mData;
			return *this;
		}
		Bitset& operator|=(const Bitset& rhs) {
			assert(mDefault == rhs.mDefault);
			alignSize(*this, rhs);
			mData |= rhs.mData;
			return *this;
		}
		
		Bitset& set(std::size_t n, bool value = true) {
			ensureSize(n);
			mData.set(n, value);
			return *this;
		}
		Bitset& set_interval(std::size_t start, std::size_t end, bool value = true) {
			ensureSize(end);
			for (; start <= end; start++) mData.set(start, value);
			return *this;
		}
		Bitset& reset(std::size_t n) {
			ensureSize(n);
			mData.reset(n);
			return *this;
		}
		bool test(std::size_t n) const {
			if (n >= mData.size()) return mDefault;
			return mData.test(n);
		}
		bool any() const {
			assert(!mDefault);
			return mData.any();
		}
		bool none() const {
			assert(!mDefault);
			return mData.none();
		}
		
		auto count() const noexcept {
			return mData.count();
		}
		
		auto size() const {
			return mData.size();
		}
		auto num_blocks() const {
			return mData.num_blocks();
		}
		auto is_subset_of(const Bitset& rhs) const {
			if (mDefault && !rhs.mDefault) return false;
			alignSize(*this, rhs);
			return mData.is_subset_of(rhs.mData);
		}
		std::size_t find_first() const {
			return mData.find_first();
		}
		std::size_t find_next(std::size_t pos) const {
			return mData.find_next(pos);
		}
		iterator begin() const {
			return iterator(*this, find_first());
		}
		iterator end() const {
			return iterator(*this, npos);
		}
		
		friend void alignSize(const Bitset& lhs, const Bitset& rhs) {
			if (lhs.size() < rhs.size()) lhs.resize(rhs.size());
			else if (lhs.size() > rhs.size()) rhs.resize(lhs.size());
		}
		
		friend bool operator==(const Bitset& lhs, const Bitset& rhs) {
			assert(lhs.mDefault == rhs.mDefault);
			alignSize(lhs, rhs);
			return lhs.mData == rhs.mData;
		}
		friend bool operator<(const Bitset& lhs, const Bitset& rhs) {
			if (lhs.size() < rhs.size()) return true;
			if (lhs.size() > rhs.size()) return false;
			return lhs.mData < rhs.mData;
		}
		
		friend Bitset operator~(const Bitset& lhs) {
			return Bitset(~lhs.mData, lhs.mDefault);
		}
		friend Bitset operator&(const Bitset& lhs, const Bitset& rhs) {
			assert(lhs.mDefault == rhs.mDefault);
			alignSize(lhs, rhs);
			return Bitset(lhs.mData & rhs.mData, lhs.mDefault);
		}
		friend Bitset operator|(const Bitset& lhs, const Bitset& rhs) {
			assert(lhs.mDefault == rhs.mDefault);
			alignSize(lhs, rhs);
			return Bitset(lhs.mData | rhs.mData, lhs.mDefault);
		}
		
		friend std::ostream& operator<<(std::ostream& os, const Bitset& b) {
			return os << b.mData << "@" << b.mDefault;
		}
	};
}
