#pragma once

#include "settings_utils.h"

#include <any>
#include <map>
#include <string>

namespace carl::settings {

/**
 * Base class for central settings class.
 * 
 * Wraps a map from a string identifier to some struct holding the actual settings, wrapped as `std::any`.
 * Simply call `.get<SettingsData>("identifier")` to obtain a reference to the settings data, which is created (and thereby initialized) lazily.
 */
struct Settings {
private:
	/// Maps identifier to any object holding the actual settings data.
	std::map<std::string,std::any> mSettings;
public:
	/// Get settings data of type `T` from the identifier `name`. Constructs the data object if it does not exist yet.
	template<typename T>
	T& get(const std::string& name) {
		auto res = mSettings.emplace(name, T{});
		return std::any_cast<T&>(res.first->second);
	}
};

}