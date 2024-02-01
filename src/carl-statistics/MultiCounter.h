#pragma once

#include <boost/container/flat_map.hpp>

namespace carl::statistics {

template<typename T>
class MultiCounter {
    boost::container::flat_map<T,std::size_t> m_data;

public:
    void inc(const T& key, std::size_t inc) {
        m_data.try_emplace(key).first->second += inc;
    }

    void collect(std::map<std::string, std::string>& data, const std::string& key) const {
        std::stringstream ss;
		for (const auto& [k,v] : m_data) {
            serialize(ss, k);
			ss << "=" << v << ";";
		}
        data.emplace(key, ss.str());
    }
};

}