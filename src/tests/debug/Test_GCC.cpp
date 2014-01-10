#include <type_traits>

enum class enabled {};
constexpr enabled dummy = {};

template<typename T, typename std::enable_if<std::is_integral<T>::value, enabled>::type = dummy>
class A {};

template<typename T, typename isFraction = typename std::enable_if<std::is_integral<T>::value, enabled>::type>
class B {};

template<typename T>
class B<T, typename std::enable_if<std::is_integral<T>::value, enabled>::type> : public A<T> {
};

template<typename T> void f(const A<T>* m) {}
template<typename T> void f(const B<T>* m) {}

int main() {
	A<int>* map = nullptr;
	f(map);
	return 0;
}
