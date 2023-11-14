#pragma once

namespace carl::statistics {

template<typename T, typename S>
void serialize(std::stringstream& ss, const std::pair<T,S>& pair) {
    ss << "[" << pair.first << "," << pair.second << "]";
}

template<typename T>
void serialize(std::stringstream& ss, const T& v) {
    ss << v;
}

}