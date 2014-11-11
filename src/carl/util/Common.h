/**
 * Common.h
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 * @since 2013-10-07
 * @version 2014-10-30
 */

#pragma once

#include <vector>
#include <set>
#include <map>
#include <string.h>
#include <unordered_set>
#include <unordered_map>
#include <memory>

#include "../core/Variable.h"
//#include "../core/VariableInformation.h"

namespace carl
{	
	// Structures.
	
	template<typename T> 
	struct pointerEqual
	{
		bool operator()( const T* _argA, const T* _argB ) const
		{
			return (*_argA)==(*_argB);
		}
	};
	
	template<typename T> 
	struct pointerEqualWithNull
	{
		bool operator()( const T* _argA, const T* _argB ) const
		{
			if( _argA == nullptr || _argB == nullptr )
				return _argA == _argB;
			return (*_argA)==(*_argB);
		}
	};
	
	template<typename T> 
	struct pointerLess
	{
		bool operator()( const T* _argA, const T* _argB ) const
		{
			return (*_argA)<(*_argB);
		}
	};

	template<typename T> 
	struct pointerHash
	{
		size_t operator()( const T* _arg ) const
		{
			return std::hash<T>()( *_arg );
		}
	};

	template<typename T> 
	struct pointerHashWithNull
	{
		size_t operator()( const T* _arg ) const
		{
			if( _arg == nullptr )
				return 0;
			return std::hash<T>()( *_arg );
		}
	};
	
	template<typename T> 
	struct sharedPointerEqual
	{
		bool operator()( const std::shared_ptr<const T>& _argA, const std::shared_ptr<const T>& _argB ) const
		{
            return (*_argA)==(*_argB);
		}
	};
	
	template<typename T> 
	struct sharedPointerEqualWithNull
	{
		bool operator()( const std::shared_ptr<const T>& _argA, const std::shared_ptr<const T>& _argB ) const
		{
			if( _argA == nullptr || _argB == nullptr )
				return _argA == _argB;
			return (*_argA)==(*_argB);
		}
	};
	
	template<typename T> 
	struct sharedPointerLess
	{
		bool operator()( const std::shared_ptr<const T>& _argA, const std::shared_ptr<const T>& _argB ) const
		{
			return (*_argA)<(*_argB);
		}
	};

	template<typename T> 
	struct sharedPointerHash
	{
		size_t operator()( const std::shared_ptr<const T>& _arg ) const
		{
            return std::hash<T>()( *_arg );
		}
	};

    template<typename T> 
    struct sharedPointerHashWithNull
    {
        size_t operator()( const std::shared_ptr<const T>& _arg ) const
        {
            if( _arg == nullptr )
                return 0;
            return std::hash<T>()( *_arg );
        }
    };
    
    // Further type definitions.
    template<typename T> 
    using EvaluationMap = std::map<Variable, T>;
    
//    template<typename Pol>
//    using VarInfo = VariableInformation<true, Pol>;
//    
//    template<typename Pol>
//    using VarInfoMap = std::map<Variable, VarInfo<Pol>>;
    
    typedef std::set<Variable> Variables;

	typedef std::vector<Variables> QuantifiedVariables;
    
    template<typename T> 
    using PointerSet = std::set<const T*, pointerLess<T>>;
    
    template<typename T> 
    using PointerMultiSet = std::multiset<const T*, pointerLess<T>>;
    
    template<typename T1,typename T2> 
    using PointerMap = std::map<const T1*, T2, pointerLess<T1>>;
    
    template<typename T> 
    using SharedPointerSet = std::set<std::shared_ptr<const T>, sharedPointerLess<T>>;
    
    template<typename T> 
    using SharedPointerMultiSet = std::multiset<std::shared_ptr<const T>, sharedPointerLess<T>>;
    
    template<typename T1,typename T2> 
    using SharedPointerMap = std::map<std::shared_ptr<const T1>, T2, sharedPointerLess<T1>>;
    
    template<typename T> 
    using FastSet = std::unordered_set<const T, std::hash<T>>;
    
    template<typename T1,typename T2> 
    using FastMap = std::unordered_map<const T1, T2, std::hash<T1>>;
    
    template<typename T> 
    using FastPointerSet = std::unordered_set<const T*, pointerHash<T>, pointerEqual<T>>;
    
    template<typename T1,typename T2> 
    using FastPointerMap = std::unordered_map<const T1*, T2, pointerHash<T1>, pointerEqual<T1>>;
    
    template<typename T> 
    using FastSharedPointerSet = std::unordered_set<std::shared_ptr<const T>, sharedPointerHash<T>, sharedPointerEqual<T>>;
    
    template<typename T1,typename T2> 
    using FastSharedPointerMap = std::unordered_map<std::shared_ptr<const T1>, T2, sharedPointerHash<T1>, sharedPointerEqual<T1>>;
    
    template<typename T> 
    using FastPointerSetB = std::unordered_set<const T*, pointerHashWithNull<T>, pointerEqualWithNull<T>>;
    
    template<typename T1,typename T2> 
    using FastPointerMapB = std::unordered_map<const T1*, T2, pointerHashWithNull<T1>, pointerEqualWithNull<T1>>;
    
    template<typename T> 
    using FastSharedPointerSetB = std::unordered_set<std::shared_ptr<const T>, sharedPointerHashWithNull<T>, pointerEqualWithNull<T>>;
    
    template<typename T1,typename T2> 
    using FastSharedPointerMapB = std::unordered_map<std::shared_ptr<const T1>, T2, sharedPointerHashWithNull<T1>, pointerEqualWithNull<T1>>;
    
    template<typename Pol>
    using Factors = FastMap<Pol,exponent>;
    
    // Constants.
    ///@todo move static variables to own cpp
    
    static const unsigned MAX_DEGREE_FOR_FACTORIZATION = 6;
    
    static const unsigned MIN_DEGREE_FOR_FACTORIZATION = 2;
    
    static const unsigned MAX_DIMENSION_FOR_FACTORIZATION = 6;
    
    static const unsigned MAX_NUMBER_OF_MONOMIALS_FOR_FACTORIZATION = 7;
    
    // Macros.

	#define CIRCULAR_SHIFT(_intType, _value, _shift) ((_value << _shift) | (_value >> (sizeof(_intType)*8 - _shift)))
	
}	// namespace carl




