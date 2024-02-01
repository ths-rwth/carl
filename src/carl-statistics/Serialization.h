#pragma once

#include <carl-common/util/streamingOperators.h>

namespace carl::statistics {

template<typename T, typename S>
inline void serialize(std::stringstream& ss, const std::pair<T,S>& pair) {
    ss << "[" << pair.first << "," << pair.second << "]";
}

template<typename T>
inline void serialize(std::stringstream& ss, const std::vector<T>& v) {
    ss << carl::stream_joined(";", v);
}

template<typename Key, typename Value, typename Comparator>
inline void serialize(std::stringstream& ss, const std::map<Key, Value, Comparator>& m) {
	return ss << stream_joined(";", m, [](auto& o, const auto& p){ o << p.first << "=" << p.second; });
}

template<typename T>
void serialize(std::stringstream& ss, const T& v) {
    ss << v;
}

}