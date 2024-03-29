#pragma once

#include <carl-common/memory/Singleton.h>

#include <memory>
#include <string>
#include <vector>

namespace carl {
namespace statistics {

class Statistics;

class StatisticsCollector: public carl::Singleton<StatisticsCollector> {
private:
	std::vector<std::unique_ptr<Statistics>> mStatistics;
public:
	template<typename T>
	T& get(const std::string& name) {
		auto& ptr = mStatistics.emplace_back(std::make_unique<T>());
		ptr->set_name(name);
		return static_cast<T&>(*ptr);
	}

	void collect();

	const auto& statistics() const {
		return mStatistics;
	}
};

template<typename T>
auto& get(const std::string& name) {
	return StatisticsCollector::getInstance().get<T>(name);
}

}
}