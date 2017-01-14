/**
 * @file UFManager.h
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-30
 * @version 2014-10-30
 */

#pragma once


#include "../../util/Common.h"
#include "../../util/hash.h"
#include "../../util/Singleton.h"
#include "../Sort.h"
#include "UninterpretedFunction.h"

#include <cassert>
#include <iostream>
#include <set>
#include <unordered_map>
#include <utility>
#include <vector>

namespace carl
{

// Forward declaration.
class UFManager;

/**
 * The actual content of an uninterpreted function instance.
 */
class UFContent
{
    friend class UFManager;
    
    private:
        /// The uninterpreted function's name.
        std::string mName;
        /// The uninterpreted function's domain.
        std::vector<Sort> mDomain;
        /// The uninterpreted function's codomain.
        Sort mCodomain;

        UFContent() = delete;
        UFContent(const UFContent&) = delete;
		UFContent(UFContent&&) = delete;

        /**
         * Constructs the content of an uninterpreted function.
         * @param _name The name of the uninterpreted function to construct.
         * @param _domain The domain of the uninterpreted function to construct.
         * @param _codomain The codomain of the uninterpreted function to construct.
         */
        explicit UFContent(std::string&& _name, std::vector<Sort>&& _domain, Sort _codomain):
            mName(std::move(_name)),
            mDomain(std::move(_domain)),
            mCodomain(_codomain)
        {}
    public:
        
        /**
         * @return The name of the uninterpreted function.
         */
        const std::string& name() const
        {
            return mName;
        }
        
        /**
         * @return The domain of the uninterpreted function.
         */
        const std::vector<Sort>& domain() const
        {
            return mDomain;
        }
        
        /**
         * @return The codomain of the uninterpreted function.
         */
        const Sort& codomain() const
        {
            return mCodomain;
        }
        
        /**
         * @param _ufc The uninterpreted function's content to compare with.
         * @return true, if this uninterpreted function's content is less than the given one.
         */
        bool operator==( const UFContent& _ufc ) const
        {
            return mDomain == _ufc.domain() && mName == _ufc.name();
        }

        /**
         * @param _ufc The uninterpreted function's content to compare with.
         * @return true, if this uninterpreted function's content is less than the given one.
         */
        bool operator<( const UFContent& _ufc ) const
        {   
            if( mName < _ufc.name() )
                return true;
            if( mName > _ufc.name() )
                return false;
            if( mDomain.size() < _ufc.domain().size() )
                return true;
            if( mDomain.size() > _ufc.domain().size() )
                return false;
            auto domA = mDomain.begin();
            auto domB = _ufc.domain().begin();
            while( domA != mDomain.end() )
            {
                assert( domB != _ufc.domain().end() );
                if( *domA < *domB )
                    return true;
                if( *domB < *domA )
                    return false;
                ++domA;
                ++domB;
            }
            return false;
        }
};

} // end namespace carl

namespace std
{
/**
 * Implements std::hash for uninterpreted function's contents.
 */
template<>
struct hash<carl::UFContent>
{
public:
    /**
     * @param _ufun The uninterpreted function to get the hash for.
     * @return The hash of the given uninterpreted function.
     */
    std::size_t operator()( const carl::UFContent& _ufun ) const 
    {
        hash<carl::Sort> h;
        std::size_t result = std::hash<std::string>()(_ufun.name());
        for (auto& dom: _ufun.domain()) {
			carl::hash_add(result, h(dom));
        }
        return result;
    }
};
} // end namespace std

namespace carl
{
    
/**
 * Implements a manager for uninterpreted functions, containing their actual contents and allocating their ids.
 */
class UFManager : public Singleton<UFManager>
{
    
        friend Singleton<UFManager>;
        
    private:
        // Members.

        /// Stores all instantiated uninterpreted function's contents and maps them to their unique id.
        FastPointerMap<UFContent, UninterpretedFunction::IDType> mUFIdMap;
        /// Maps the unique ids to the instantiated uninterpreted function's content.
        std::vector<const UFContent*> mUFs;

        /**
         * Constructs an uninterpreted functions manager.
         */
        UFManager():
            mUFIdMap(),
            mUFs()
        {
            mUFs.emplace_back( nullptr ); // default value
        }
        ~UFManager() override {
            mUFIdMap.clear();
            for (auto& ptr: mUFs) delete ptr;
            mUFs.clear();
        }
        
        /**
         * Tries to add the given uninterpreted function's content to the so far stored uninterpreted function's 
         * contents. If it has already been stored, if will be deleted and the id of the already existing uninterpreted 
         * function's content will be used to create the returned uninterpreted function.
         * @param _sc The uninterpreted function's content to store.
         * @return The uninterpreted function corresponding to the given content.
         */
        UninterpretedFunction newUF( const UFContent* _sc );

    public:
        
        const std::vector<const UFContent*>& ufContents() const {
            return mUFs;
        }
        const FastPointerMap<UFContent, UninterpretedFunction::IDType>& ufIDMap() const {
            return mUFIdMap;
        }
        
        /**
         * @param _uf An uninterpreted function.
         * @return The name of the uninterpreted function of the given uninterpreted function.
         */
        const std::string& getName( const UninterpretedFunction& _uf ) const
        {
            assert( _uf.id() != 0 );
            assert( _uf.id() < mUFs.size() );
            return mUFs[_uf.id()]->name();
        }
        
        /**
         * @param _uf An uninterpreted function.
         * @return The domain of the uninterpreted function of the given uninterpreted function.
         */
        const std::vector<Sort>& getDomain( const UninterpretedFunction& _uf ) const
        {
            assert( _uf.id() != 0 );
            assert( _uf.id() < mUFs.size() );
            return mUFs[_uf.id()]->domain();
        }
        
        /**
         * @param _uf An uninterpreted function.
         * @return The codomain of the uninterpreted function of the given uninterpreted function.
         */
        const Sort& getCodomain( const UninterpretedFunction& _uf ) const
        {
            assert( _uf.id() != 0 );
            assert( _uf.id() < mUFs.size() );
            return mUFs[_uf.id()]->codomain();
        }
        
        /**
         * Prints the given uninterpreted function on the given output stream.
         * @param _out The output stream to print the given uninterpreted function on.
         * @param  _uf The uninterpreted function to print.
         * @return The output stream after printing the given uninterpreted function on it.
         */
        std::ostream& print( std::ostream& _out, const UninterpretedFunction& _uf ) const;
        
        /**
         * Gets the uninterpreted function with the given name, domain, arguments and codomain.
         * @param _name The name of the uninterpreted function of the uninterpreted function to get.
         * @param _domain The domain of the uninterpreted function of the uninterpreted function to get.
         * @param _codomain The codomain of the uninterpreted function of the uninterpreted function to get.
         * @return The resulting uninterpreted function.
         */
        UninterpretedFunction newUninterpretedFunction(std::string&& _name, std::vector<Sort>&& _domain, Sort _codomain)
        {
            auto result = new UFContent(std::move(_name), std::move(_domain), _codomain);
            return newUF(result);
        }
};

/**
 * Gets the uninterpreted function with the given name, domain, arguments and codomain.
 * @param _name The name of the uninterpreted function of the uninterpreted function to get.
 * @param _domain The domain of the uninterpreted function of the uninterpreted function to get.
 * @param _codomain The codomain of the uninterpreted function of the uninterpreted function to get.
 * @return The resulting uninterpreted function.
 */
inline UninterpretedFunction newUninterpretedFunction(std::string _name, std::vector<Sort> _domain, Sort _codomain) {
    return UFManager::getInstance().newUninterpretedFunction(std::move(_name), std::move(_domain), _codomain);
}

}
