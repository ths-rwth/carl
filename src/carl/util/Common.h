/**
 * Common.h
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 * @since 2013-10-07
 * @version 2014-10-30
 */

#pragma once


#include "../core/Variable.h"
#include "../numbers/numbers.h"
#include <carl-common/util/pointerOperations.h>

#include <cstring>
#include <map>
#include <memory>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace carl
{	
	// Structures.

	template<typename T>
	using pointerEqual = carl::equal_to<const T*, false>;
	template<typename T>
	using pointerEqualWithNull = carl::equal_to<const T*, true>;
	template<typename T>
	using sharedPointerEqual = carl::equal_to<std::shared_ptr<const T>, false>;
	template<typename T>
	using sharedPointerEqualWithNull = carl::equal_to<std::shared_ptr<const T>, true>;
	
	template<typename T>
	using pointerLess = carl::less<const T*, false>;
	template<typename T>
	using pointerLessWithNull = carl::less<const T*, true>;
	template<typename T>
	using sharedPointerLess = carl::less<std::shared_ptr<const T>*, false>;
	template<typename T>
	using sharedPointerLessWithNull = carl::less<std::shared_ptr<const T>, true>;
	
	template<typename T>
	using pointerHash = carl::hash<T*, false>;
	template<typename T>
	using pointerHashWithNull = carl::hash<T*, true>;
	template<typename T>
	using sharedPointerHash = carl::hash<std::shared_ptr<const T>*, false>;
	template<typename T>
	using sharedPointerHashWithNull = carl::hash<std::shared_ptr<const T>*, true>;
    
    // Further type definitions.
    template<typename T> 
    using EvaluationMap = std::map<Variable, T>;
    
//    template<typename Pol>
//    using VarInfo = VariableInformation<true, Pol>;
//    
//    template<typename Pol>
//    using VarInfoMap = std::map<Variable, VarInfo<Pol>>;
    
    using Variables = std::set<Variable>;

    using QuantifiedVariables = std::vector<Variables>;
    
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
    using FastSet = std::unordered_set<T, std::hash<T>>;
    
    template<typename T1,typename T2> 
    using FastMap = std::unordered_map<T1, T2, std::hash<T1>>;
    
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
    using Factors = std::map<Pol,uint>;
    
    // Constants.
    ///@todo move static variables to own cpp
    
    static constexpr uint MAX_DEGREE_FOR_FACTORIZATION = 6;
    
    static constexpr uint MIN_DEGREE_FOR_FACTORIZATION = 1;
    
    static constexpr uint MAX_DIMENSION_FOR_FACTORIZATION = 6;
    
    static constexpr uint MAX_NUMBER_OF_MONOMIALS_FOR_FACTORIZATION = 10;
    
    static constexpr bool FULL_EFFORT_FOR_DEFINITENESS_CHECK = false;
	
}	// namespace carl
