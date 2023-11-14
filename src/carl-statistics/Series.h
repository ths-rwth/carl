#pragma once

namespace carl::statistics {

class Series {
    std::size_t m_count = 0;
    std::size_t m_sum = 0;
    std::size_t m_min = 0;
    std::size_t m_max = 0;

public:
    void add(std::size_t n) {
        m_sum += n;
        m_min = m_count == 0 ? n : std::min(m_min, n);
        m_max = m_count == 0 ? n : std::max(m_max, n);
        m_count++;
    }

    void collect(std::map<std::string, std::string>& data, const std::string& key) const {
        data.emplace(key+".count", std::to_string(m_count));
        data.emplace(key+".sum",   std::to_string(m_sum));
        data.emplace(key+".min",   std::to_string(m_min));
        data.emplace(key+".max",   std::to_string(m_max));
        data.emplace(key+".avg",   std::to_string((float)m_sum/m_count));
    }
};

}