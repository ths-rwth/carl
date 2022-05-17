/* 
 * File:   TermAdditionManager.h
 * Author: Florian Corzilius
 *
 * Created on October 30, 2014, 7:20 AM
 */

#pragma once 

#include <list>
#include <mutex>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <carl-common/config.h>
#include "Term.h"
#include <carl-common/util/streamingOperators.h>
#include <carl-common/util/pointerOperations.h>

namespace carl
{

template<typename Polynomial, typename Ordering>
class TermAdditionManager {
public:
	using IDType = unsigned;
	using Coeff = typename Polynomial::CoeffType;
	using TermType = Term<Coeff>;
	using TermPtr = TermType;
	using TermIDs = std::vector<IDType>;
	using Terms = std::vector<TermPtr>;
	/* 0: Maps global IDs to local IDs.
	 * 1: Actual terms by local IDs.
	 * 2: Flag if this entry is currently used.
	 * 3: Constant part.
	 * 4: Next free local ID.
	 */
	using Tuple = std::tuple<TermIDs,Terms,bool,Coeff,IDType>;
	using TAMId = typename std::list<Tuple>::iterator;
private:
	std::list<Tuple> mData;
	TAMId mNextId;
	mutable std::mutex mMutex;
    
    #ifdef THREAD_SAFE
    #define TAM_LOCK_GUARD std::lock_guard<std::mutex> lock( mMutex );
    #define TAM_LOCK mMutex.lock();
    #define TAM_UNLOCK mMutex.unlock();
    #else
    #define TAM_LOCK_GUARD
    #define TAM_LOCK
    #define TAM_UNLOCK
    #endif
	
	TAMId createNewEntry() {
		TAMId res = mData.emplace(mData.end());
		std::get<4>(*res) = 1;
		return res;
	}
	
	bool compare(TAMId id, IDType t1, IDType t2) const {
		Tuple& data = *id;
		assert(std::get<2>(data));
		Terms& t = std::get<1>(data);
		return Ordering::less(t[t1], t[t2]);
	}
public:
	TermAdditionManager() {
        MonomialPool::getInstance();
		mNextId = createNewEntry();
	}
	
    #define SWAP_TERMS
	
	TAMId getId(std::size_t expectedSize = 0) {
		TAM_LOCK_GUARD
		assert(mNextId != mData.end());
		while (std::get<2>(*mNextId)) {
			mNextId++;
			if (mNextId == mData.end()) {
				mNextId = mData.emplace(mData.end());
				std::get<4>(*mNextId) = 1;
			}
		}
        Tuple& data = *mNextId;
        Terms& terms = std::get<1>(data);
		terms.clear();
        terms.resize(expectedSize + 1);
        #ifdef SWAP_TERMS
        //memset(&terms[0], 0, sizeof(TermPtr)*terms.size());
        #endif
        std::size_t greatestIdPlusOne = MonomialPool::getInstance().largestID() + 1;
		if( std::get<0>(data).size() < greatestIdPlusOne ) std::get<0>(data).resize(greatestIdPlusOne);
		//memset(&std::get<0>(data)[0], 0, sizeof(IDType)*std::get<0>(data).size());
		std::get<3>(data) = constant_zero<Coeff>::get();
		std::get<4>(data) = 1;
		std::get<2>(data) = true;
		TAMId result = mNextId;
		mNextId++;
		if (mNextId == mData.end()) mNextId = mData.begin();
		return result;
	}

    template<bool SizeUnknown, bool NewMonomials = true>
	void addTerm(TAMId id, const TermPtr& term) {
		assert(!is_zero(term));
        Tuple& data = *id;
		assert(std::get<2>(data));
		TermIDs& termIDs = std::get<0>(data);
		Terms& terms = std::get<1>(data);
		if (term.monomial()) {
			std::size_t monId = term.monomial()->id();
			if (NewMonomials && monId >= termIDs.size()) termIDs.resize(monId + 1);
            IDType locId = termIDs[monId];
			if (locId != 0) {
				if (SizeUnknown && locId >= terms.size()) terms.resize(locId + 1);
				assert(locId < terms.size());
                TermPtr& t = terms[locId];
				if (!carl::is_zero(t.coeff())) {
					Coeff coeff = t.coeff() + term.coeff();
					if (carl::is_zero(coeff)) {
						termIDs[monId] = 0;
						t = std::move(TermType());
					} else {
						t.coeff() = std::move(coeff);
					}
				} else 
                    t = term;
			} else {
				IDType& nextID = std::get<4>(data);
				if (SizeUnknown && nextID >= terms.size()) terms.resize(nextID + 1);
				assert(nextID < terms.size());
				assert(nextID < std::numeric_limits<IDType>::max());
				termIDs[monId] = nextID;
				terms[nextID] = term;
				++nextID;
			}
		} else {
			std::get<3>(data) += term.coeff();
		}
	}

	TermType getMaxTerm(TAMId id) const {
		Tuple& data = *id;
		Terms& terms = std::get<1>(data);
		std::size_t max = 0;
		assert(terms.size() > 0);
		for (std::size_t i = 1; i < terms.size(); i++) {
			if (Ordering::less(terms[max], terms[i])) max = i;
		}
		assert(!terms[max].isConstant() || is_zero(terms[max]));
		if (is_zero(terms[max])) return TermType(std::get<3>(data));
		else return terms[max];
	}
    
	void readTerms(TAMId id, Terms& terms) {
        Tuple& data = *id;
		assert(std::get<2>(data));
		Terms& t = std::get<1>(data);
        TermIDs& termIDs = std::get<0>(data);
        #ifdef SWAP_TERMS
		if (!is_zero(std::get<3>(data))) {
			t[0] = std::move(TermType(std::move(std::get<3>(data)), nullptr));
		}
        for (auto i = t.begin(); i != t.end();) {
			if (is_zero(*i)) {
				//Avoid invalidating pointer for last element
				if (i == --t.end()) {
					t.pop_back();
					break;
				} else {
					std::swap(*i, *t.rbegin());
					t.pop_back();
				}
			} else {
				if ((*i).monomial()) termIDs[(*i).monomial()->id()] = 0;
                ++i;
            }
		}
		std::swap(t, terms);
		t.clear();
        #else
        terms.clear();
        if (!is_zero(std::get<3>(data))) {
			terms.push_back( std::make_shared<const TermType>(std::move(std::get<3>(data)), nullptr) );
		}
        auto i = t.begin();
        ++i;
		for (; i != t.end(); ++i)
        {
			if (*i)
            {
                termIDs[(*i)->monomial()->id()] = 0;
                terms.push_back( *i );
                *i = nullptr;
            }
		}
		t.clear();
        #endif
		TAM_LOCK_GUARD
		std::get<2>(data) = false;
	}

	void dropTerms(TAMId id) {
		Tuple& data = *id;
		assert(std::get<2>(data));
		Terms& t = std::get<1>(data);
        TermIDs& termIDs = std::get<0>(data);
		for (auto i = t.begin(); i != t.end(); i++) {
			if ((*i).monomial()) termIDs[(*i).monomial()->id()] = 0;
		}
		TAM_LOCK_GUARD
		std::get<2>(data) = false;
	}
};

}
