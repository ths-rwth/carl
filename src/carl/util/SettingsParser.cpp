#include "SettingsParser.h"

#include <boost/any.hpp>
#include <filesystem>

namespace carl::settings {

std::ostream& operator<<(std::ostream& os, const boost::any& val) {
	if (val.empty()) {
		return os << "<empty>";
	} else if (boost::any_cast<bool>(&val)) {
		return os << std::boolalpha << boost::any_cast<bool>(val);
	} else if (boost::any_cast<std::size_t>(&val)) {
		return os << boost::any_cast<std::size_t>(val);
	} else if (boost::any_cast<std::string>(&val)) {
		return os << boost::any_cast<std::string>(val);
	}
	return os << "Unknown type";
}


std::ostream& operator<<(std::ostream& os, OptionPrinter op) {
	if (op.parser.argv_zero == nullptr) {
		os << "Usage: <binary> [options]";
	} else {
		os << "Usage: " << op.parser.argv_zero << " [options]";
	}
	for (unsigned i = 0; i < op.parser.mPositional.max_total_count(); ++i) {
		os << " " << op.parser.mPositional.name_for_position(i);
	}
	os << std::endl;
	return os << op.parser.mAllOptions;
}
std::ostream& operator<<(std::ostream& os, SettingsPrinter sp) {
	for (const auto& option: sp.parser.mAllOptions.options()) {
		auto value = sp.parser.mValues[option->canonical_display_name()];
		os << option->canonical_display_name() << " = " << value.value() << std::endl;
	}
	return os;
}

void SettingsParser::warn_for_unrecognized(const po::parsed_options& parsed) const {
	auto res = po::collect_unrecognized(parsed.options, po::exclude_positional);
	for (const auto& s: res) {
		warn_for_unrecognized_option(s);
	}
}

void SettingsParser::parse_command_line(int argc, char* argv[]) {
	auto parsed = po::command_line_parser(argc, argv).allow_unregistered().options(mAllOptions).positional(mPositional).run();
	warn_for_unrecognized(parsed);
	po::store(parsed, mValues);
}

void SettingsParser::parse_config_file() {
	std::filesystem::path configfile(mValues[this->name_of_config_file()].as<std::string>());
	if (std::filesystem::is_regular_file(configfile)) {
		auto parsed = po::parse_config_file<char>(configfile.c_str(), mAllOptions, true);
		warn_for_unrecognized(parsed);
		po::store(parsed, mValues);
	} else {
		warn_config_file(configfile);
	}
}

void SettingsParser::finalize_settings() {
	for (const auto& f: mFinalizer) {
		f();
	}
}

void SettingsParser::finalize() {
	for (const auto& po: mOptions) {
		mAllOptions.add(po);
	}
}

void SettingsParser::parse_options(int argc, char* argv[]) {
	argv_zero = argv[0];
	parse_command_line(argc, argv);
	if (mValues.count(this->name_of_config_file())) {
		parse_config_file();
	}
	po::notify(mValues);
	finalize_settings();
}

}