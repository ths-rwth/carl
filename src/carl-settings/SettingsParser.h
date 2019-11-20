#pragma once

#include "settings_utils.h"

#include <boost/program_options.hpp>

#include <chrono>
#include <vector>

#if __has_include(<filesystem>)
#include <filesystem>
// NOLINTNEXTLINE(cert-dcl58-cpp): sorry but we need this.
namespace std {
inline void validate(boost::any& v, const std::vector<std::string>& values, std::filesystem::path* /*unused*/, int /*unused*/) {
	namespace pov = boost::program_options::validators;
	namespace fs = std::filesystem;
	fs::path p(pov::get_single_string(values));
	if (fs::exists(p)) {
		v = fs::canonical(p);
	} else {
		v = fs::absolute(p);
	}
}
}
#endif

namespace carl::settings {

namespace po = boost::program_options;

/**
 * Inserts value into variables_map if it is not yet set.
 * This method is intended as a helper for finalizer functions.
 */
template<typename T>
void default_to(po::variables_map& values, const std::string& name, const T& value) {
	auto it = values.find(name);
	if (it == values.end()) {
		values.emplace(name, po::variable_value(boost::any(value), true));
	} else if (it->second.defaulted()) {
		values.at(name) = po::variable_value(boost::any(value), false);
	}
}

/**
 * Inserts or overwrites value into variables_map.
 * This method is intended as a helper for finalizer functions.
 */
template<typename T>
void overwrite_to(po::variables_map& values, const std::string& name, const T& value) {
	auto it = values.find(name);
	if (it == values.end()) {
		values.emplace(name, po::variable_value(boost::any(value), true));
	} else {
		values.at(name) = po::variable_value(boost::any(value), false);
	}
}

// Predeclaration.
class SettingsParser;

/// Helper class to nicely print the options that are available.
struct OptionPrinter {
	/// Reference to parser.
	const SettingsParser& parser;
};
/// Streaming operator for a option printer.
std::ostream& operator<<(std::ostream& os, OptionPrinter op);

/// Helper class to nicely print the settings that were parsed.
struct SettingsPrinter {
	/// Reference to parser.
	const SettingsParser& parser;
};
/// Streaming operator for a settings printer.
std::ostream& operator<<(std::ostream& os, SettingsPrinter sp);

/**
 * Base class for a settings parser.
 */
class SettingsParser {
	friend std::ostream& settings::operator<<(std::ostream& os, settings::OptionPrinter op);
	friend std::ostream& settings::operator<<(std::ostream& os, settings::SettingsPrinter sp);
protected:
	/// Stores the name of the current binary.
	char* argv_zero = nullptr;
	/// Stores the positional arguments.
	po::positional_options_description mPositional;
	/// Accumulates all available options.
	po::options_description mAllOptions;
	/// Stores the parsed values.
	po::variables_map mValues;
	/// Stores the individual options until the parser is finalized.
	std::vector<po::options_description> mOptions;
	/// Stores hooks for setting object finalizer functions.
	std::vector<std::function<bool()>> mFinalizer;

	/// Checks for unrecognized options that were found.
	void warn_for_unrecognized(const po::parsed_options& parsed) const;
	/// Parses the command line.
	void parse_command_line(int argc, char* argv[], bool allow_unregistered);
	/// Parses the config file if one was configured.
	void parse_config_file(bool allow_unregistered);
	/// Calls the finalizer functions.
	bool finalize_settings();

	/// Prints a warning if an option was unrecognized. Can be overridden.
	virtual void warn_for_unrecognized_option(const std::string& s) const {
		std::cerr << "Ignoring unrecognized option " << s << std::endl;
	}
	/// Prints a warning if loading the config file failed. Can be overridden.
	virtual void warn_config_file(const std::string& file) const {
		std::cerr << "Could not load config file " << file << std::endl;
	}
	/// Gives the option name for the config file name. Can be overridden.
	virtual std::string name_of_config_file() const {
		return "config";
	}
public:
	/// Virtual destructor.
	virtual ~SettingsParser() = default;

	/// Finalizes the parser.
	void finalize();

	/**
	 * Adds a new options_description with a title and a reference to the settings object.
	 * The settings object is needed to pass it to the finalizer function.
	 */
	po::options_description& add(const std::string& title) {
		return mOptions.emplace_back(po::options_description(title));
	}
	/**
	 * Adds a finalizer function to be called after parsing.
	 * boost::program_options::notify() is called before running the finalizer functions.
	 * The finalizer function should accept a boost::program_options::variables_map as its only argument and should return a bool indicating whether it changed the variables map.
	 * If any finalizer changed the variables map, boost::program_options::notify() is called again afterwards.
	 */
	template<typename F>
	void add_finalizer(F&& f) {
		mFinalizer.emplace_back([this,f](){ return f(mValues); });
	}

	/**
	 * Parse the options.
	 * If allow_unregistered is set to true, we allow them but call warn_for_unrecognized_option() for each one.
	 * Otherwise an exception is raised when an unrecognized option is encountered.
	 */
	void parse_options(int argc, char* argv[], bool allow_unregistered = true);

	/**
	 * Print a help page.
	 * Returns a helper object so that it can be used as follows:
	 *   `std::cout << parser.print_help() << std::endl;`
	 */
	OptionPrinter print_help() const {
		return OptionPrinter{*this};
	}

	/**
	 * Print the parsed settings.
	 * Returns a helper object so that it can be used as follows:
	 *   `std::cout << parser.print_options() << std::endl;`
	 */
	SettingsPrinter print_options() const {
		return SettingsPrinter{*this};
	}
};

}