#include "Singleton.h"

#include "../core/MonomialPool.h"
#include "../core/VariablePool.h"

namespace carl {
	
	template<typename T>
	T& SingletonInstantiated<T>::getInstance() {
		static T t;
		return t;
	}
	
	template class SingletonInstantiated<MonomialPool>;
	//template class SingletonInstantiated<VariablePool>;
}
