/**
 * @file UFModel.h
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-24
 * @version 2014-10-24
 */

#pragma once

#include "SortValue.h"

#include "../../uninterpreted/UFManager.h"
#include "../../uninterpreted/UninterpretedFunction.h"

#include <iostream>
#include <map>
#include <utility>

namespace carl
{

/**
 * Implements a sort value, being a value of the uninterpreted domain specified by this sort.
 */
class UFModel : private std::map<std::vector<SortValue>,SortValue>
{
	private:
		UninterpretedFunction uf;
    public:
        
        explicit UFModel(const UninterpretedFunction& uf): uf(uf)
        {}

        bool extend( const std::vector<SortValue>& _args, const SortValue& _value );

        SortValue get( const std::vector<SortValue>& _args ) const;
        
        std::size_t getHash() const;

        /**
         * Prints the given uninterpreted function model on the given output stream.
         * @param _os The output stream to print on.
         * @param _ufm The uninterpreted function model to print.
         * @return The output stream after printing the given uninterpreted function model on it.
         */
        friend std::ostream& operator<<( std::ostream& _os, const UFModel& _ufm );

        /**
         * @param _ufm The uninterpreted function model to compare with.
         * @return true, if this uninterpreted function model equals the given one.
         */
        bool operator==( const UFModel& _ufm ) const;

        /**
         * @param _ufm The uninterpreted function model to compare with.
         * @return true, if this uninterpreted function model is less than the given one.
         */
        bool operator<( const UFModel& _ufm ) const;
};

}

namespace std
{
    /**
     * Implements std::hash for uninterpreted function model.
     */
    template<>
    struct hash<carl::UFModel>
    {
    public:
        /**
         * @param _ufm The uninterpreted function model to get the hash for.
         * @return The hash of the given uninterpreted function model.
         */
        std::size_t operator()( const carl::UFModel& _ufm ) const 
        {
            return _ufm.getHash();
        }
    };
}
