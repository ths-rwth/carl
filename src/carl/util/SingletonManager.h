#pragma once

#include <array>
#include <atomic>
#include <cassert>
#include <functional>
#include <memory>

namespace carl {
	
	class SingletonManager {
	private:
		using Pointer = std::unique_ptr<void, std::function<void(void*)>>;
		static constexpr std::size_t MAX_SINGLETONS = 10;
		std::array<Pointer, MAX_SINGLETONS> mSingletons;
		std::atomic<std::size_t> mIndex{0};
		
		template<typename T>
		std::size_t initialize() {
			assert(mIndex < mSingletons.size());
			mSingletons[mIndex] = Pointer(new T(), [](void* p){ delete static_cast<T*>(p); });
			return mIndex++;
		}
	public:
		template<typename T>
		T& get() {
			static std::size_t id = initialize<T>();
			return *static_cast<T*>(mSingletons[id].get());
		}
		
		static SingletonManager& instance() {
			static SingletonManager sm;
			return sm;
		}
	};
	
}
