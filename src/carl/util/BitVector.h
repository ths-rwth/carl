/** 
 * @file   BitVector.h
 *         Created on June 25, 2013, 6:23 PM
 * @author: Sebastian Junges
 *
 * 
 */
#pragma once

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <vector>

namespace carl
{

    constexpr unsigned sizeOfUnsigned = sizeof(unsigned);
	
    class BitVector {
	public:
		BitVector(): mBits() {}
		
		explicit BitVector(unsigned pos): mBits() {
			setBit(pos);
		}
		
		void clear() 
		{
			mBits = {};
		}
		
		size_t size() const {
			return mBits.size() * sizeOfUnsigned * 8;
		}
		
		void reserve(size_t capacity) {
			mBits.resize(capacity);
		}
		
        bool empty() const
        {
			for (const auto& b: mBits) {
				if (b != 0) return false;
			}
            return true;
        }
        
        size_t findFirstSetBit() const
        {
            size_t pos = 0;
			for (const auto& b: mBits) {
				if(b != 0) {
                    unsigned elem = b;
                    while( (elem & unsigned(1)) == 0 )
                    {
                        elem >>= 1;
                        ++pos;
                    }
                    return pos;
                }
                pos += sizeOfUnsigned * 8;
            }
            return size();
        }
                
		void setBit(unsigned pos, bool val = true) {
			static_assert(sizeof(unsigned) == 4, "Currently bitvectors are only supported on these platforms.");
			unsigned vecElem = pos >> 5;
			unsigned mask = unsigned(1);
			mask <<= (pos & unsigned(31));
			if(vecElem >=  mBits.size()) {
				mBits.resize(vecElem + 1, 0);
			} 
			if(!val) {
				mask = ~mask;
				mBits[vecElem] &= mask;
			} else {
				mBits[vecElem] |= mask;
			}
			assert(getBit(pos) == val);
		}
		
		bool getBit(unsigned pos) const {
			static_assert(sizeof(unsigned) == 4, "Currently bitvectors are only supported on these platforms.");
			unsigned vecElem =  pos >> 5;
			if(vecElem < mBits.size()) {
				unsigned bitNr = pos & unsigned(31);
				return ((mBits[vecElem] >> bitNr) & unsigned(1)) != 0;
			} 
			else 
			{
				return false;
			}
		}
        
        bool subsetOf(const BitVector& superset);
		
		friend bool operator== (const BitVector& lhs, const BitVector& rhs);
        
        BitVector& calculateUnion(const BitVector& rhs)
		{
			return *this |= rhs;
		}
		
		friend BitVector operator| (const BitVector& lhs, const BitVector& rhs);
		
		
		BitVector& operator|= (const BitVector& rhs) {
			auto lhsIt = mBits.begin();
			auto rhsIt = rhs.mBits.begin();
			
			auto lhsEnd = mBits.end();
			auto rhsEnd = rhs.mBits.end();
			
			while(true) {
				if(lhsIt == lhsEnd) {
					mBits.insert(lhsIt,rhsIt,rhsEnd);
					break;
				}
				
				if(rhsIt == rhsEnd) {
					break;
				}
				
				*lhsIt |= *rhsIt;
				++rhsIt;
				++lhsIt;
			}
				
			return *this;
		}
		
		
		class forward_iterator {
		public:
			forward_iterator(const std::vector<unsigned>::const_iterator it, const std::vector<unsigned>::const_iterator vectorEnd) :
			posInVec(0), vecIter(it), vecEnd(vectorEnd), curVecElem(vecIter == vecEnd ? 0 : *vecIter)
			{
			}
		protected:
			unsigned posInVec;
			std::vector<unsigned>::const_iterator vecIter;
			const std::vector<unsigned>::const_iterator vecEnd;
			unsigned curVecElem;
			
		public:
			bool get() {
				return bool(curVecElem & unsigned(1));
			}
			
			void next() {
				if(++posInVec == 32) { 
					posInVec = 0;
					curVecElem = (++vecIter) == vecEnd ? 0 : *vecIter ;
				} else {
					curVecElem >>= 1;
				}
			}
			
			friend bool operator==(const forward_iterator& fi1, const forward_iterator& fi2);
			
			forward_iterator operator++(int i) {
				if(i == 0) {
					next();
				}
				return *this;
			}
			
			bool isEnd() {
				return vecIter == vecEnd;
			}
		};
		using const_iterator = forward_iterator;
		
		forward_iterator begin() const {
			return forward_iterator(mBits.begin(), mBits.end());
		}
		
		forward_iterator end() const {
			return forward_iterator(mBits.end(), mBits.end());
		}
		
		void print(std::ostream& os = std::cout) const {
			//std::cout << "Size of vector entries: " << sizeOfUnsigned << " .. " << std::endl;
			for(const_iterator it = begin(); !it.isEnd(); it++) {
				os << it.get();
			}
		}
		
		protected:
			std::vector<unsigned> mBits;
	};
	
	inline std::ostream& operator<<(std::ostream& os, const BitVector& bv)
	{
		bv.print(os);
		return os;
	}	

}
