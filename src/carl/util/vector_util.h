#pragma once

#include <vector>

namespace carl {

template<typename T>
void vector_concat(std::vector<T>& result, const std::vector<std::vector<T>>& input) {
	for (const auto& cur: input) {
		result.insert(result.end(), cur.begin(), cur.end());
	}
}

template<typename T>
std::vector<T> vector_concat(const std::vector<std::vector<T>>& input) {
	std::vector<T> result;
	vector_concat(result, input);
	return result;
}

}
