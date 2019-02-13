#pragma once

#include "../core/logging.h"

#include <boost/program_options.hpp>

#include <vector>

#if __has_include(<filesystem>)
#include <filesystem>
namespace std {
inline void validate(boost::any& v, const std::vector<std::string>& values, std::filesystem::path*, int) {
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

/**
 * Hook to be called when settings have been parsed.
 * This allows to compute some advanced settings like a common prefix of paths.
 * This is the generic (no-op) version that individual settings may specialize.
 */
template<typename T, typename V>
inline void finalize_settings(T&, const V&) {}

namespace po = boost::program_options;

// Predeclaration.
class SettingsParser;

/// Helper class to nicely print the options that are available.
struct OptionPrinter {
	/// Reference to parser.
	const SettingsParser& parser;
};
/// Streaming operator for a option printer.
std::ostream& operator<<(std::ostream& os, OptionPrinter);

/// Helper class to nicely print the settings that were parsed.
struct SettingsPrinter {
	/// Reference to parser.
	const SettingsParser& parser;
};
/// Streaming operator for a settings printer.
std::ostream& operator<<(std::ostream& os, SettingsPrinter);

class SettingsParser {
	friend std::ostream& settings::operator<<(std::ostream& os, settings::OptionPrinter);
	friend std::ostream& settings::operator<<(std::ostream& os, settings::SettingsPrinter);
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
	std::vector<std::function<void()>> mFinalizer;

	/// Checks for unrecognized options that were found.
	void warn_for_unrecognized(const po::parsed_options& parsed) const;
	/// Parses the command line.
	void parse_command_line(int argc, char* argv[]);
	/// Parses the config file if one was configured.
	void parse_config_file();
	/// Calls the finalizer functions.
	void finalize_settings();

	virtual void warn_for_unrecognized_option(const std::string& s) const {
		CARL_LOG_WARN("carl.settings", "Ignoring unrecognized option " << s);
	}
	virtual void warn_config_file(const std::filesystem::path& file) const {
		CARL_LOG_WARN("carl.settings", "Could not load config file " << file);
	}
	virtual std::string name_of_config_file() const {
		return "config";
	}
public:
	/// Finalizes the parser.
	void finalize();

	/**
	 * Adds a new options_description with a title and a reference to the settings object.
	 * The settings object is needed to pass it to the finalizer function.
	 */
	po::options_description& add(const std::string& title) {
		return mOptions.emplace_back(po::options_description(title));
	}
	template<typename F>
	void add_finalizer(F&& f) {
		mFinalizer.emplace_back([this,f](){ f(mValues); });
	}

	/// Parse the options.
	void parse_options(int argc, char* argv[]);

	/**
	 * Print a help page.
	 * Returns a helper object so that it can be used as follows:
	 *   std::cout << parser.print_help() << std::endl;
	 */
	OptionPrinter print_help() const {
		return OptionPrinter{*this};
	}

	/**
	 * Print the parsed settings.
	 * Returns a helper object so that it can be used as follows:
	 *   std::cout << parser.print_options() << std::endl;
	 */
	SettingsPrinter print_options() const {
		return SettingsPrinter{*this};
	}
};

}