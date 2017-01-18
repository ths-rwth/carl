/**
 * @file UFInstance.h
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-30
 * @version 2014-10-30
 */

#pragma once

#include "UVariable.h"
#include "UninterpretedFunction.h"

#include <iostream>

#include <utility>
#include <vector>

namespace carl
{
    /**
     * Implements an uninterpreted function instance.
     */
    class UFInstance
    {
        public:
            friend class UFInstanceManager;
        private:
        
            /// A unique id.
            std::size_t mId;
            
            /**
             * Constructs an uninterpreted function instance.
             * @param _id
             */
            explicit UFInstance(std::size_t id) noexcept: mId(id) {}
            
        public:
            
            /**
             * Default constructor.
             */
            UFInstance():
                mId( 0 )
            {}
            
            /**
             * @return The unique id of this uninterpreted function instance.
             */
            std::size_t id() const
            {
                return mId;
            }
            
            /**
             * @return The underlying uninterpreted function of this instance.
             */
            const UninterpretedFunction& uninterpretedFunction() const;

            /**
             * @return The arguments of this uninterpreted function instance.
             */
            const std::vector<UVariable>& args() const;
            
            /**
             * @param _ufun The uninterpreted function instance to compare with.
             * @return true, if this and the given uninterpreted function instance are equal.
             */
            bool operator==( const UFInstance& _ufun ) const
            {
                return mId == _ufun.id();
            }
            
            /**
             * @param _ufun The uninterpreted function instance to compare with.
             * @return true, if this uninterpreted function instance is less than the given one.
             */
            bool operator<( const UFInstance& _ufun ) const
            {
                return mId < _ufun.id();
            }
            
            /**
             * @return The string representation of this uninterpreted function instance.
             */
            std::string toString( bool _infix, bool _friendlyNames ) const;
            
            /**
             * Prints the given uninterpreted function instance on the given output stream.
             * @param _os The output stream to print on.
             * @param _ufun The uninterpreted function instance to print.
             * @return The output stream after printing the given uninterpreted function instance on it.
             */
            friend std::ostream& operator<<( std::ostream& _os, const UFInstance& _ufun );
    };
} // end namespace carl


namespace std
{
    /**
     * Implements std::hash for uninterpreted function instances.
     */
    template<>
    struct hash<carl::UFInstance>
    {
    public:
        /**
         * @param _ufi The uninterpreted function instance to get the hash for.
         * @return The hash of the given uninterpreted function instance.
         */
        std::size_t operator()( const carl::UFInstance& _ufi ) const 
        {
            return std::size_t(_ufi.id());
        }
    };
}
