/* Compile with
 *	g++ Test_GCC.cpp -std=c++11 [-save-temps]
 * Output:
Test_GCC.cpp: In function ‘int main()’:
Test_GCC.cpp:27:7: internal compiler error: in unify, at cp/pt.c:17171
  f(map);
       ^
Please submit a full bug report,
with preprocessed source if appropriate.
See <https://bugs.archlinux.org/> for instructions.
 */

#include <type_traits>

enum class enabled {};
constexpr enabled dummy = {};

template<typename T, typename std::enable_if<std::is_integral<T>::value, enabled>::type = dummy>
class A {};

template<typename T> void f(const A<T>* m) {}

int main() {
	A<int>* map = nullptr;
	f(map);
	return 0;
}
