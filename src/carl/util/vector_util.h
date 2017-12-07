#pragma once

#include <cassert>
#include <vector>

namespace carl {

template<typename T>
void vector_concat(std::vector<T>& result, const std::vector<std::vector<T>>& input) {
	for (const auto& cur : input) {
		result.insert(result.end(), cur.begin(), cur.end());
	}
}

template<typename T>
std::vector<T> vector_concat(const std::vector<std::vector<T>>& input) {
	std::vector<T> result;
	vector_concat(result, input);
	return result;
}

template<typename T1, typename T2>
std::vector<std::pair<T1, T2>> vector_zip(const std::vector<T1>& v1, const std::vector<T2>& v2) {
	assert(v1.size() == v2.size());
	std::vector<std::pair<T1, T2>> res;
	auto it1 = v1.begin();
	for (auto it2 = v2.begin(); it2 != v2.end(); ++it1, ++it2) {
		res.emplace_back(*it1, *it2);
	}
	return res;
}

namespace detail {
template<typename It>
void advance_all(It& it) {
	++it;
}
template<typename It, typename... Its>
void advance_all(It& it, Its&... its) {
	advance_all(it);
	advance_all(its...);
}
template<typename Result, typename It, typename... Its>
void vector_zip_tuple(Result& res, It begin, It end, Its... its) {
	for (; begin != end; ++begin, advance_all(its...)) {
		res.emplace_back(*begin, *its...);
	}
}
} // namespace detail

template<typename Result, typename Input, typename... Inputs>
void vector_zip_tuple(Result& res, const Input& input, const Inputs&... inputs) {
	detail::vector_zip_tuple(res, input.begin(), input.end(), inputs.begin()...);
}

} // namespace carl
