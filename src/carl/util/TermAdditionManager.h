/* 
 * File:   TermAdditionManager.h
 * Author: Florian Corzilius
 *
 * Created on October 30, 2014, 7:20 AM
 */

#pragma once 
#include "../core/Term.h"
#include <vector>
#include <unordered_map>
#include <mutex>

namespace carl
{

template<typename Polynomial>
class TermAdditionManager
{
    private:
        
        typedef std::unordered_map<std::shared_ptr<const Monomial>,std::shared_ptr<const Term<typename Polynomial::CoeffType>>, std::equal_to<std::shared_ptr<const Monomial>>> MapType;
    
        ///
        size_t mNextMapId;
        ///
        std::vector<const Polynomial*> mUsers;
        ///
        std::vector<MapType> mTermMaps;
        ///
        std::vector<typename MapType::iterator> mLeadingTerms;
        ///
        std::vector<typename MapType::iterator> mConstantTerms;
        ///
        std::mutex mMutex;
        
    public:
            
        TermAdditionManager( size_t _numberOfMaps = 1 ):
            mNextMapId( 0 ),
            mUsers( _numberOfMaps, nullptr ),
            mTermMaps( _numberOfMaps, MapType() ),
            mLeadingTerms( _numberOfMaps, mTermMaps.end() ),
            mConstantTerms( _numberOfMaps, mTermMaps.end() )
        {}
            
        void resize( size_t _newSize )
        {
            assert( !inUse() );
            mUsers( _newSize, nullptr );
            mTermMaps( _newSize, MapType() );
            mLeadingTerms( _newSize, mTermMaps.end() );
            mConstantTerms( _newSize, mTermMaps.end() );
        }
        
        size_t getTermMapId( const Polynomial& _user, size_t _expectedSize )
        {
            std::lock_guard<std::mutex> lock(this->freshVarMutex);
            assert( mUsers.at( mNextMapId ) == nullptr );
            assert( mTermMaps.at( mNextMapId ).empty());
            assert( mLeadingTerms.at( mNextMapId ) == mTermMaps.end() );
            assert( mConstantTerms.at( mNextMapId ) == mTermMaps.end() );
            mUsers[mNextMapId] = &_user;
            mTermMaps[mNextMapId].reserve( _expectedSize );
            size_t result = mNextMapId;
            mNextMapId = (mNextMapId + 1) % mTermMaps.size();
            return result;
        }
        
        void addTerm( const Polynomial& _user, size_t _id, std::shared_ptr<const Term<typename Polynomial::CoeffType>> _term )
        {
            assert( mUsers.at( _id ) == &_user );
            assert( mTermMaps[_id].size() < mTermMaps[_id].capacity() );
            auto res = mTermMaps[_id].emplace( _term->monomial(), _term );
            if( res.second )
            {
                if( _term->monomial() == nullptr )
                {
                    assert( mConstantTerms.at( _id ) == mTermMaps.end() );
                    mConstantTerms[_id] = res.first;
                }
                else if( mLeadingTerms[_id] == mTermMaps.end() || *(mLeadingTerms[_id]->first) < *_term->monomial() )
                {
                    mLeadingTerms[_id] = res.first;
                }
            }
            else
            {
                typename Polynomial::CoeffType tmp = res.first->second->coeff() + _term->coeff();
                if( tmp == 0 )
                {
                    mTermMaps[_id].erase( res.first );
                }
                else
                {
                    res.first->second = std::make_shared<const Term<typename Polynomial::CoeffType>>( tmp, _term->monomial() );
                }
            }
        }
        
        void readTerms( const Polynomial& _user, size_t _id, std::vector<std::shared_ptr<const Term<typename Polynomial::CoeffType>>>& _terms )
        {
            assert( mUsers.at( _id ) == &_user );
            MapType& mcMap = mTermMaps[_id];
            _terms.clear();
            _terms.reserve( mcMap.size() );
            if( mLeadingTerms[_id] != mTermMaps.end() )
            {
                _terms.push_back( mLeadingTerms[_id]->second );
                mTermMaps.erase( mLeadingTerms[_id] );
                mLeadingTerms[_id] = mTermMaps.end();
            }
            std::shared_ptr<const Term<typename Polynomial::CoeffType>> constantPart = nullptr;
            if( mConstantTerms[_id] != mTermMaps.end() )
            {
                constantPart = mConstantTerms[_id]->second;
                mTermMaps.erase( mConstantTerms[_id] );
                mConstantTerms[_id] = mTermMaps.end();
            }
            for( auto iter = mTermMaps.begin(); iter != mTermMaps.end(); ++iter )
            {
                _terms.push_back( iter->second );
            }
            mTermMaps.clear();
            if( constantPart != nullptr )
            {
                _terms.push_back( constantPart );
            }
            mUsers[_id] = nullptr;
        }
        
     private:
         
        bool inUse() const
        {
            for( int i = 0; i < mTermMaps.size(); ++i )
            {
                if( mUsers.at( i ) != nullptr ) return true;
                if( !mTermMaps.at( i ).empty() ) return true;
                if( mLeadingTerms.at( i ) != mTermMaps.end() ) return true;
                if( mConstantTerms.at( i ) != mTermMaps.end() ) return true;
            }
            return false;
        }
};

}

