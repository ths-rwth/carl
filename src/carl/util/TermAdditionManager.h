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

#ifdef USE_MONOMIAL_POOL

#define USE_INDIRECTION
#ifdef USE_INDIRECTION

#define USE_TUPLE
#ifdef USE_TUPLE

template<typename Polynomial>
class TermAdditionManager {
public:
	typedef unsigned short IDType;
	typedef typename Polynomial::CoeffType Coeff;
	typedef Term<Coeff> TermType;
	typedef std::shared_ptr<const TermType> TermPtr;
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
        if( terms.size() < expectedSize + 1) terms.resize(expectedSize + 1);
        #ifdef SWAP_TERMS
        memset(&terms[0], 0, sizeof(TermPtr)*terms.size());
        #endif
        size_t greatestIdPlusOne = MonomialPool::getInstance().nextID();
		if( std::get<0>(data).size() < greatestIdPlusOne ) std::get<0>(data).resize(greatestIdPlusOne);
		std::get<3>(data) = constant_zero<Coeff>::get();
		std::get<4>(data) = 1;
		std::get<2>(data) = true;
		std::size_t result = mNextId;
		mNextId = (mNextId + 1) % mData.size();
		return result;
	}

    template<bool SizeUnknown, bool NewMonomials = true>
	void addTerm(std::size_t id, const TermPtr& term) {
        Tuple& data = mData[id];
		assert(std::get<2>(data));
		TermIDs& termIDs = std::get<0>(data);
		Terms& terms = std::get<1>(data);
		if (term->monomial()) {
			std::size_t monId = term->monomial()->id();
			if (NewMonomials && monId >= termIDs.size()) termIDs.resize(monId + 1);
            IDType& locId = termIDs[monId];
			if (locId) {
                TermPtr& t = terms[locId];
				if (t) {
					Coeff coeff = t->coeff() + term->coeff();
					if (carl::isZero(coeff)) {
						t = nullptr;
					} else if (t.unique()) {
						TermType::setCoeff(t, std::move(coeff));
					} else {
						t = std::make_shared<const TermType>(std::move(coeff), term->monomial());
					}
				}
				else
                     t = term;
			} else {
				IDType& nextID = std::get<4>(data);
				if (SizeUnknown && nextID >= terms.size()) terms.resize(nextID + 1);
				assert(nextID < terms.size());
				locId = nextID;
				terms[nextID] = term;
				++nextID;
			}
		} else {
			std::get<3>(data) += term->coeff();
		}
	}
    
	void readTerms(std::size_t id, Terms& terms) {
        Tuple& data = mData[id];
		assert(std::get<2>(data));
		Terms& t = std::get<1>(data);
        TermIDs& termIDs = std::get<0>(data);
        #ifdef SWAP_TERMS
		if (!isZero(std::get<3>(data))) {
			t[0] = std::make_shared<const TermType>(std::move(std::get<3>(data)), nullptr);
		}
        auto i = t.begin();
        ++i;
		for (; i != t.end();) {
			if (*i == nullptr) {
				std::swap(*i, *t.rbegin());
				t.pop_back();
			} else
            {
                termIDs[(*i)->monomial()->id()] = 0;
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

#else

template<typename Polynomial>
class TermAdditionManager {
public:
	typedef unsigned short IDType;
	typedef typename Polynomial::CoeffType Coeff;
	typedef Term<Coeff> TermType;
	typedef std::shared_ptr<const TermType> TermPtr;
	typedef std::vector<IDType> TermIDs;
	typedef std::vector<TermPtr> Terms;
private:
	std::size_t mNextId;
	std::vector<TermIDs> mTermIDs;
	std::vector<Terms> mTerms;
	std::vector<bool> mUsed;
	std::vector<typename Polynomial::CoeffType> mConstant;
	mutable std::mutex mMutex;
public:
	TermAdditionManager(): mNextId(0), mTermIDs(1), mTerms(1), mUsed(1, false), mConstant(1)
	{
	}
	
	std::size_t getId(std::size_t expectedSize = 0) {
		std::lock_guard<std::mutex> lock(mMutex);
		while (mUsed.at(mNextId)) {
			mNextId++;
			if (mNextId == mTerms.size()) {
				mTermIDs.emplace_back();
				mTerms.emplace_back();
				mUsed.push_back(false);
				mConstant.emplace_back();
			}
		}
		assert(mTermIDs.at(mNextId).empty());
		assert(mTerms.at(mNextId).empty());
		assert(mUsed.at(mNextId) == false);
		mTermIDs[mNextId].clear();
		mTermIDs[mNextId].reserve(MonomialPool::getInstance().nextID());
		mTerms[mNextId].clear();
		mTerms[mNextId].reserve(expectedSize);
		mTerms[mNextId].emplace_back(nullptr);
		mUsed[mNextId] = true;
		mConstant[mNextId] = constant_zero<Coeff>::get();
		std::size_t result = mNextId;
		mNextId = (mNextId + 1) % mTerms.size();
		return result;
	}

	template<bool SizeUnknown>
	void addTerm(std::size_t id, const TermPtr& term) {
		assert(mUsed[id]);
		TermIDs& termIDs = mTermIDs[id];
		Terms& terms = mTerms[id];
		if (term->monomial()) {
			std::size_t monId = term->monomial()->id();
			if (SizeUnknown && monId >= termIDs.size()) termIDs.resize(monId + 1);

			if (termIDs[monId] == 0) {
				termIDs[monId] = (IDType)terms.size();
				terms.push_back(term);
			} else {
				monId = termIDs[monId];
				if (terms[monId] == nullptr) terms[monId] = term;
				else {
					Coeff coeff = terms[monId]->coeff() + term->coeff();
					if (carl::isZero(coeff)) {
						terms[monId] = nullptr;
					} else if (terms[monId].unique()) {
						TermType::setCoeff(terms[monId], std::move(coeff));
					} else {
						terms[monId] = std::make_shared<const TermType>(coeff, term->monomial());
					}
				}
			}
		} else {
			mConstant[id] += term->coeff();
		}
	}
	
	void readTerms(std::size_t id, Terms& terms) {
		assert(mUsed.at(id));
		Terms& t = mTerms[id];
		if (!isZero(mConstant[id]))	{
			t[0] = std::make_shared<const TermType>(mConstant[id], nullptr);
		}
		for (auto i = t.begin(); i != t.end();) {
			if (*i == nullptr) {
				std::swap(*i, *t.rbegin());
				t.pop_back();
			} else i++;
		}
		std::swap(t, terms);
		std::lock_guard<std::mutex> lock(mMutex);
		memset(&mTermIDs.at(id)[0], 0, sizeof(IDType)*mTermIDs.at(id).size());
		mTermIDs.at(id).clear();
		mTerms.at(id).clear();
		mUsed.at(id) = false;
	}
};

#endif

#else

template<typename Polynomial>
class TermAdditionManager {
public:
	typedef Term<typename Polynomial::CoeffType> TermType;
	typedef std::shared_ptr<const TermType> TermPtr;
	typedef std::vector<TermPtr> Terms;
private:
	std::size_t mNextId;
	std::vector<Terms> mTerms;
	std::vector<bool> mUsed;
	mutable std::mutex mMutex;
public:
	TermAdditionManager(): mNextId(0), mTerms(1), mUsed(1, false)
	{
	}
	
	std::size_t getId() {
		std::lock_guard<std::mutex> lock(mMutex);
		while (mUsed.at(mNextId)) {
			mNextId++;
			if (mNextId == mTerms.size()) {
				mTerms.emplace_back();
				mUsed.emplace_back(false);
			}
		}
		assert(mUsed.at(mNextId) == false);
		assert(mTerms.at(mNextId).empty());
		mUsed[mNextId] = true;
		mTerms[mNextId].clear();
		std::size_t result = mNextId;
		mNextId = (mNextId + 1) % mTerms.size();
		return result;
	}
	
	void addTerm(std::size_t id, const TermPtr& term) {
		assert(mUsed.at(id));
		Terms& terms = mTerms[id];
		std::size_t monId = 0;
		if (term->monomial()) monId = term->monomial()->id();
		if (monId >= terms.size()) terms.resize(monId + 1);
		if (terms.at(monId) == nullptr) {
			terms[monId] = term;
		} else {
			auto coeff = terms.at(monId)->coeff() + term->coeff();
			if (coeff == typename Polynomial::CoeffType(0)) {
				terms[monId] = nullptr;
			} else {
				terms[monId] = std::make_shared<const TermType>(coeff, term->monomial());
			}
		}
	}
	
	void readTerms(std::size_t id, Terms& terms) {
		assert(mUsed.at(id));
		terms.clear();
		for (const auto& t: mTerms.at(id)) {
			if (t != nullptr) terms.push_back(t);
		}
		mTerms.at(id).clear();
		mUsed.at(id) = false;
	}
};
#endif

#else

/**
 * Class to manage term addition.
 */
template<typename Polynomial>
class TermAdditionManager
{
    private:
        
        /// A hash map of shared pointer of monomials to shared pointer of terms. 
        typedef std::unordered_map<
            std::shared_ptr<const Monomial>,
            std::shared_ptr<const Term<typename Polynomial::CoeffType>>,
            std::hash<std::shared_ptr<const Monomial>>,
            //std::equal_to<std::shared_ptr<const Monomial>>> MapType;
			hashEqual> MapType;
    
        /// Id of the next free map.
        std::size_t mNextMapId;
        /** 
         * If the ith map is used by the polynomial p, this map stores a reference to p at the ith entry of this vector. 
         * This is only for asserting that no illegal access occurs.
         */
        std::vector<const Polynomial*> mUsers;
        /// Stores the maps for the detection of equal terms.
        std::vector<MapType> mTermMaps;
        /** 
         * Stores the constant term of the currently added terms for each map. It points to the end of its corresponding
         * map, if the constant term is currently 0.
         */
        std::vector<typename MapType::iterator> mConstantTerms;
        /// Mutex for allocation of the map ids.
        mutable std::mutex mMutex;
        
    public:
            
        /**
         * Constructs a term addition manager.
         */
        TermAdditionManager():
            mNextMapId( 0 ),
            mUsers( 1, nullptr ),
            mTermMaps( 1, MapType() ),
            mConstantTerms(1, mTermMaps.back().end())
        {
        }
        
        /**
         * Reserve a free equal-term-detection-map.
         * @param _user The polynomial, which wants to reserve a equal-term-detection-map.
         * @param _expectedSize An upper bound of the expected number of terms, which are going to be added.
         * @return The id of the reserved equal-term-detection-map.
         */
        size_t getTermMapId( const Polynomial& _user, size_t _expectedSize )
        {
            std::lock_guard<std::mutex> lock(mMutex);
			while (mUsers.at( mNextMapId ) != nullptr) {
				mNextMapId++;
				if (mNextMapId == mTermMaps.size()) {
					mUsers.push_back(nullptr);
					mTermMaps.emplace_back();
					mConstantTerms.push_back(mTermMaps.back().end());
				}
			}
            assert( mUsers.at( mNextMapId ) == nullptr );
            assert( mTermMaps.at( mNextMapId ).empty());
            assert( mConstantTerms.at( mNextMapId ) == mTermMaps.at( mNextMapId ).end() );
            mUsers[mNextMapId] = &_user;
            mTermMaps[mNextMapId].reserve( _expectedSize );
            size_t result = mNextMapId;
            mNextMapId = (mNextMapId + 1) % mTermMaps.size();
            return result;
        }
        
        /**
         * Add the given term to the equal-term-detection-map with the given id.
         * @param _user The polynomial which uses this map.
         * @param _id The id of the map to add the term to.
         * @param _term The term to add.
         */
        void addTerm( const Polynomial& _user, size_t _id, std::shared_ptr<const Term<typename Polynomial::CoeffType>> _term )
        {
            assert( mUsers.at( _id ) == &_user );
            MapType& mcMap = mTermMaps[_id];
			///@todo what is this supposed to do?
            assert( mcMap.size() < (mcMap.max_load_factor()*mcMap.bucket_count()) );
            auto res = mcMap.emplace( _term->monomial(), _term );
            if( res.second )
            {
                if( _term->monomial() == nullptr )
                {
                    assert( mConstantTerms.at( _id ) == mcMap.end() );
                    mConstantTerms[_id] = res.first;
                }
            }
            else
            {
                typename Polynomial::CoeffType tmp = res.first->second->coeff() + _term->coeff();
                if( tmp == typename Polynomial::CoeffType(0) )
                {
                    mcMap.erase( res.first );
					if (res.first == mConstantTerms[_id]) mConstantTerms[_id] = mcMap.end();
                }
                else
                {
                    res.first->second = std::make_shared<const Term<typename Polynomial::CoeffType>>( tmp, _term->monomial() );
                }
            }
        }
        
        /**
         * Copy the terms in the equal-term-detection-map to the given term vector, while putting the leading term at the end of this vector
         * and the constant part at the beginning.
         * @sideeffect The given vector to store the terms into will be cleared an resized before adding terms to it.
         * @sideeffect The given id is released and the corresponding equal-term-detection-map is cleared.
         * @param _user The polynomial which uses this map.
         * @param _id The id of the map to add the term to.
         * @param _terms The vector of terms containing all terms of equal-term-detection-map with the given id.
         */
        void readTerms( const Polynomial& _user, size_t _id, std::vector<std::shared_ptr<const Term<typename Polynomial::CoeffType>>>& _terms )
        {
            assert( mUsers.at( _id ) == &_user );
            MapType& mcMap = mTermMaps[_id];
            _terms.clear();
            _terms.reserve( mcMap.size() );
            if( mConstantTerms[_id] != mcMap.end() )
            {
                _terms.push_back( mConstantTerms[_id]->second );
                mcMap.erase( mConstantTerms[_id] );
                mConstantTerms[_id] = mcMap.end();
            }
            for( auto iter = mcMap.begin(); iter != mcMap.end(); ++iter )
            {
                _terms.push_back( iter->second );
            }
			//printHashStats(mcMap);
			mcMap.clear();
            mUsers[_id] = nullptr;
        }
        
        void removeDuplicates( const Polynomial& _poly )
        {
            size_t id = getTermMapId( _poly, _poly.nrTerms() );
            for( std::shared_ptr<const Term<typename Polynomial::CoeffType>> term : _poly.mTerms )
            {
                addTerm( _poly, id, term );
            }
            readTerms( _poly, id, _poly.mTerms );
        }

		void printHashStats(const MapType& m) const {
			std::cout << m.size() << " in " << m.bucket_count() << " buckets" << std::endl;
			for (std::size_t id = 0; id < m.bucket_count(); id++) {
				std::cout << "\t" << id << " -> " << m.bucket_size(id) << std::endl;
			}
		}
        
     private:
         
        /**
         * @return true, if any equal-term-detection-map is in use.
         */
        bool inUse() const
        {
            for( size_t i = 0; i < mTermMaps.size(); ++i )
            {
                if( mUsers.at( i ) != nullptr ) return true;
                if( !mTermMaps.at( i ).empty() ) return true;
                if( mConstantTerms.at( i ) != mTermMaps.at( i ).end() ) return true;
            }
            return false;
        }
};

#endif

}

