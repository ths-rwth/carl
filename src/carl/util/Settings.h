#pragma once

#include <any>
#include <map>
#include <string>

namespace carl::settings {

struct Settings {
private:
	std::map<std::string,std::any> mSettings;
public:
	template<typename T>
	T& get(const std::string& name) {
		auto res = mSettings.emplace(name, T{});
		return std::any_cast<T&>(res.first->second);
	}
};

}