/**
 * @file UninterpretedFunction.h
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 * @since 2014-10-30
 * @version 2014-10-30
 */

#pragma once

#include "../../util/hash.h"
#include "../Sort.h"

#include <iostream>
#include <utility>
#include <vector>

namespace carl
{
	/**
	 * Implements an uninterpreted function.
	 */
	class UninterpretedFunction
	{
		public:
			friend class UFManager;
			/// A unique id to identify this uninterpreted function in it's manager.
			using IDType = std::size_t;
		
		private:
		
			/// A unique id.
			IDType mId = 0;
			
			/**
			 * Constructs an uninterpreted function.
			 * @param _id
			 */
			explicit UninterpretedFunction( IDType _id ):
				mId( _id )
			{}
			
		public:
			
			/**
			 * Default constructor.
			 */
			UninterpretedFunction() noexcept = default;
			
			/**
			 * @return The unique id of this uninterpreted function instance.
			 */
			IDType id() const
			{
				return mId;
			}
			
			/**
			 * @return The name of this uninterpreted function.
			 */
			const std::string& name() const;

			/**
			 * @return The domain of this uninterpreted function.
			 */
			const std::vector<Sort>& domain() const;

			/**
			 * @return The codomain of this uninterpreted function.
			 */
			const Sort& codomain() const;
			
			/**
			 * @param _ufun The uninterpreted function to compare with.
			 * @return true, if this and the given uninterpreted function are equal.
			 */
			bool operator==( const UninterpretedFunction& _ufun ) const
			{
				return mId == _ufun.id();
			}
			
			/**
			 * @param _ufun The uninterpreted function to compare with.
			 * @return true, if this uninterpreted function is less than the given one.
			 */
			bool operator<( const UninterpretedFunction& _ufun ) const
			{
				return mId < _ufun.id();
			}
			
			/**
			 * Prints the given uninterpreted function on the given output stream.
			 * @param _os The output stream to print on.
			 * @param _ufun The uninterpreted function to print.
			 * @return The output stream after printing the given uninterpreted function on it.
			 */
			friend std::ostream& operator<<( std::ostream& _os, const UninterpretedFunction& _ufun );
	};
} // end namespace carl


namespace std
{
	/**
	 * Implements std::hash for uninterpreted functions.
	 */
	template<>
	struct hash<carl::UninterpretedFunction> {
		/**
		 * @param _uf The uninterpreted function to get the hash for.
		 * @return The hash of the given uninterpreted function.
		 */
		std::size_t operator()( const carl::UninterpretedFunction& _uf ) const {
			return std::size_t(_uf.id());
		}
	};
}
