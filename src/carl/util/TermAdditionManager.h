/* 
 * File:   TermAdditionManager.h
 * Author: Florian Corzilius
 *
 * Created on October 30, 2014, 7:20 AM
 */

#pragma once 
#include "../core/Term.h"
#include "../core/pointerOperations.h"
#include <vector>
#include <unordered_map>
#include <mutex>

namespace carl
{

#ifdef USE_MONOMIAL_POOL

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

