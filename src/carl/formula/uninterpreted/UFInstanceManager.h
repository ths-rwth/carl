/**
 * @file UFInstanceManager.h
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-30
 * @version 2014-10-30
 */

#pragma once


#include "../../util/Common.h"
#include "../../util/Singleton.h"
#include "../../util/hash.h"
#include "../Sort.h"
#include "UFInstance.h"
#include "UVariable.h"

#include <cassert>
#include <iostream>
#include <set>
#include <unordered_map>
#include <utility>
#include <vector>

namespace carl
{

// Forward declaration.
class UFInstanceManager;

/**
 * The actual content of an uninterpreted function instance.
 */
class UFInstanceContent
{
    friend class UFInstanceManager;
    
    private:
        /// The underlying uninterpreted function of theinstance.
        UninterpretedFunction mUninterpretedFunction;
        /// The uninterpreted function instance's arguments.
        std::vector<UVariable> mArgs;

        UFInstanceContent(); // The default constructor is disabled.

        /**
         * Constructs the content of an uninterpreted function instance.
         * @param _uf The underlying function of the uninterpreted function instance to construct.
         * @param _args The arguments of the uninterpreted function instance to construct.
         */
        explicit UFInstanceContent( const UninterpretedFunction& _uf, std::vector<UVariable>&& _args ):
            mUninterpretedFunction( _uf ),
            mArgs( std::move( _args ) )
        {}

        /**
         * Constructs the content of an uninterpreted function instance.
         * @param _uf The underlying function of the uninterpreted function instance to construct.
         * @param _args The arguments of the uninterpreted function instance to construct.
         */
        explicit UFInstanceContent( const UninterpretedFunction& _uf, const std::vector<UVariable>& _args ):
            mUninterpretedFunction( _uf ),
            mArgs( _args )
        {}

        UFInstanceContent( const UFInstanceContent& ); // The copy constructor is disabled.

    public:
        
        /**
         * @return The underlying function of the uninterpreted function instance
         */
        const UninterpretedFunction& uninterpretedFunction() const
        {
            return mUninterpretedFunction;
        }
        
        /**
         * @return The arguments of the uninterpreted function instance.
         */
        const std::vector<UVariable>& args() const
        {
            return mArgs;
        }
        
        /**
         * @param _ufic The uninterpreted function instance's content to compare with.
         * @return true, if this uninterpreted function instance's content is less than the given one.
         */
        bool operator==( const UFInstanceContent& _ufic ) const
        {
            return (mUninterpretedFunction == _ufic.uninterpretedFunction() && mArgs == _ufic.args());
        }

        /**
         * @param _ufic The uninterpreted function instance's content to compare with.
         * @return true, if this uninterpreted function instance's content is less than the given one.
         */
        bool operator<( const UFInstanceContent& _ufic ) const
        {   
            if( mUninterpretedFunction < _ufic.uninterpretedFunction() )
                return true;
            if( _ufic.uninterpretedFunction() < mUninterpretedFunction )
                return false;
            if( mArgs.size() < _ufic.args().size() )
                return true;
            if( mArgs.size() > _ufic.args().size() )
                return false;
            auto argA = mArgs.begin();
            auto argB = _ufic.args().begin();
            while( argA != mArgs.end() )
            {
                assert( argB != _ufic.args().end() );
                if( *argA < *argB )
                    return true;
                if( *argB < *argA )
                    return false;
                ++argA;
                ++argB;
            }
            return false;
        }
};

} // end namespace carl

namespace std
{
/**
 * Implements std::hash for uninterpreted function instance's contents.
 */
template<>
struct hash<carl::UFInstanceContent>
{
public:
    /**
     * @param _ufun The uninterpreted function to get the hash for.
     * @return The hash of the given uninterpreted function.
     */
    std::size_t operator()( const carl::UFInstanceContent& _ufun ) const 
    {
        std::hash<carl::UVariable> h;
        std::size_t result = std::hash<carl::UninterpretedFunction>()(_ufun.uninterpretedFunction());
        for (const auto& arg: _ufun.args()) {
			carl::hash_add(result, h(arg));
        }
        return result;
    }
};
} // end namespace std

namespace carl
{
    
/**
 * Implements a manager for uninterpreted function instances, containing their actual contents and allocating their ids.
 */
class UFInstanceManager : public Singleton<UFInstanceManager>
{
    
        friend Singleton<UFInstanceManager>;
        
    private:
        // Members.

        /// Stores all instantiated uninterpreted function instance's contents and maps them to their unique id.
        FastPointerMap<UFInstanceContent, std::size_t> mUFInstanceIdMap;
        /// Maps the unique ids to the instantiated uninterpreted function instance's content.
        std::vector<const UFInstanceContent*> mUFInstances;

        /**
         * Constructs an uninterpreted function instances manager.
         */
        UFInstanceManager():
            mUFInstanceIdMap(),
            mUFInstances()
        {
            mUFInstances.emplace_back( nullptr ); // default value
        }
        ~UFInstanceManager() override {
            mUFInstanceIdMap.clear();
            for (auto& ptr: mUFInstances) delete ptr;
            mUFInstances.clear();
        }
        
        /**
         * Tries to add the given uninterpreted function instance's content to the so far stored uninterpreted function instance's 
         * contents. If it has already been stored, if will be deleted and the id of the already existing uninterpreted 
         * function instance's content will be used to create the returned uninterpreted function instance.
         * @param _sc The uninterpreted function instance's content to store.
         * @return The uninterpreted function instance corresponding to the given content.
         */
        UFInstance newUFInstance( const UFInstanceContent* _ufic );

    public:
        
        /**
         * @param _ufi An uninterpreted function instance.
         * @return The underlying uninterpreted function of the uninterpreted function of the given uninterpreted function instance.
         */
        const UninterpretedFunction& getUninterpretedFunction( const UFInstance& _ufi ) const
        {
            assert( _ufi.id() != 0 );
            assert( _ufi.id() < mUFInstances.size() );
            return mUFInstances[_ufi.id()]->uninterpretedFunction();
        }
        
        /**
         * @param _ufi An uninterpreted function instance.
         * @return The arguments of the given uninterpreted function instance.
         */
        const std::vector<UVariable>& getArgs( const UFInstance& _ufi ) const
        {
            assert( _ufi.id() != 0 );
            assert( _ufi.id() < mUFInstances.size() );
            return mUFInstances[_ufi.id()]->args();
        }
        
        /**
         * Prints the given uninterpreted function instance on the given output stream.
         * @param _out The output stream to print the given uninterpreted function instance on.
         * @param _ufi The uninterpreted function instance to print.
		 * @param _infix If infix notation shall be used.
		 * @param _friendlyNames Whether to use the friendly names for printing.
         * @return The output stream after printing the given uninterpreted function instance on it.
         */
        std::ostream& print( std::ostream& _out, const UFInstance& _ufi, bool _infix = false, bool _friendlyNames = true ) const;
        
        /**
         * Gets the uninterpreted function instance with the given name, domain, arguments and codomain.
         * @param _uf The underlying function of the uninterpreted function instance to get.
         * @param _args The arguments of the uninterpreted function instance to get.
         * @return The resulting uninterpreted function instance.
         */
        UFInstance newUFInstance( const UninterpretedFunction& _uf, std::vector<UVariable>&& _args )
        {
            auto result = new UFInstanceContent( _uf, std::move( _args ) );
            assert( argsCorrect( *result ) );
            return newUFInstance( result );
        }

        /**
         * Gets the uninterpreted function instance with the given name, domain, arguments and codomain.
         * @param _uf The underlying function of the uninterpreted function instance to get.
         * @param _args The arguments of the uninterpreted function instance to get.
         * @return The resulting uninterpreted function instance.
         */
        UFInstance newUFInstance( const UninterpretedFunction& _uf, const std::vector<UVariable>& _args )
        {
            auto result = new UFInstanceContent( _uf, _args );
            assert( argsCorrect( *result ) );
            return newUFInstance( result );
        }
            
        /**
         * @return true, if the arguments domains coincide with those of the domain.
         */
        static bool argsCorrect( const UFInstanceContent& _ufic );
};

/**
 * Gets the uninterpreted function instance with the given name, domain, arguments and codomain.
 * @param _uf The underlying function of the uninterpreted function instance to get.
 * @param _args The arguments of the uninterpreted function instance to get.
 * @return The resulting uninterpreted function instance.
 */
inline UFInstance newUFInstance( const UninterpretedFunction& _uf, std::vector<UVariable>&& _args )
{
    return UFInstanceManager::getInstance().newUFInstance( _uf, std::move( _args ) );
}

/**
 * Gets the uninterpreted function instance with the given name, domain, arguments and codomain.
 * @param _uf The underlying function of the uninterpreted function instance to get.
 * @param _args The arguments of the uninterpreted function instance to get.
 * @return The resulting uninterpreted function instance.
 */
inline UFInstance newUFInstance( const UninterpretedFunction& _uf, const std::vector<UVariable>& _args )
{
    return UFInstanceManager::getInstance().newUFInstance( _uf, _args );
}

} // end namespace carl
