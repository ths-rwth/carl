/* 
 * File:   TermAdditionManager.h
 * Author: Florian Corzilius
 *
 * Created on October 30, 2014, 7:20 AM
 */

#pragma once 

#include <mutex>
#include <unordered_map>
#include <tuple>
#include <vector>

#include "../core/Term.h"
#include "../core/pointerOperations.h"
#include "../io/streamingOperators.h"

namespace carl
{

template<typename Polynomial>
class TermAdditionManager {
public:
	typedef unsigned short IDType;
	typedef typename Polynomial::CoeffType Coeff;
	typedef Term<Coeff> TermType;
	typedef TermType TermPtr;
	typedef std::vector<IDType> TermIDs;
	typedef std::vector<TermPtr> Terms;
	typedef std::tuple<TermIDs,Terms,bool,Coeff,IDType> Tuple;
private:
	std::size_t mNextId;
	std::vector<Tuple> mData;
	mutable std::mutex mMutex;
public:
	TermAdditionManager(): mNextId(0)
	{
		mData.emplace_back();
		std::get<4>(mData.back()) = 1;
	}
	
    #define SWAP_TERMS
	
	std::size_t getId(std::size_t expectedSize = 0) {
		//std::lock_guard<std::mutex> lock(mMutex);
		while (std::get<2>(mData[mNextId])) {
			mNextId++;
			if (mNextId == mData.size()) {
				mData.emplace_back();
				std::get<4>(mData.back()) = 1;
			}
		}
        Tuple& data = mData[mNextId];
		//std::lock_guard<std::mutex> lock(mMutex);
        Terms& terms = std::get<1>(data);
		terms.clear();
        terms.resize(expectedSize + 1);
        #ifdef SWAP_TERMS
        //memset(&terms[0], 0, sizeof(TermPtr)*terms.size());
        #endif
        size_t greatestIdPlusOne = MonomialPool::getInstance().nextID();
		if( std::get<0>(data).size() < greatestIdPlusOne ) std::get<0>(data).resize(greatestIdPlusOne);
		//memset(&std::get<0>(data)[0], 0, sizeof(IDType)*std::get<0>(data).size());
		std::get<3>(data) = constant_zero<Coeff>::get();
		std::get<4>(data) = 1;
		std::get<2>(data) = true;
		std::size_t result = mNextId;
		mNextId = (mNextId + 1) % mData.size();
		return result;
	}

    template<bool SizeUnknown, bool NewMonomials = true>
	void addTerm(std::size_t id, const TermPtr& term) {
		assert(!term.isZero());
        Tuple& data = mData[id];
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
				if (!carl::isZero(t.coeff())) {
					Coeff coeff = t.coeff() + term.coeff();
					if (carl::isZero(coeff)) {
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
    
	void readTerms(std::size_t id, Terms& terms) {
        Tuple& data = mData[id];
		assert(std::get<2>(data));
		Terms& t = std::get<1>(data);
        TermIDs& termIDs = std::get<0>(data);
        #ifdef SWAP_TERMS
		if (!isZero(std::get<3>(data))) {
			t[0] = std::move(TermType(std::move(std::get<3>(data)), nullptr));
		}
		for (auto i = t.begin(); i != t.end();) {
			if (i->isZero()) {
				std::swap(*i, *t.rbegin());
				t.pop_back();
			} else {
				if ((*i).monomial()) termIDs[(*i).monomial()->id()] = 0;
                ++i;
            }
		}
		std::swap(t, terms);
        #else
        terms.clear();
        if (!isZero(std::get<3>(data))) {
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
        #endif
        
		std::get<2>(data) = false;
	}
};

}

