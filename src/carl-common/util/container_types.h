#pragma once

#include "pointerOperations.h"

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
    
}	// namespace carl
