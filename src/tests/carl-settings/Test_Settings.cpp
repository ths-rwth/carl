#include <boost/program_options.hpp>
#include <carl/util/Singleton.h>
#include <carl-settings/settings_utils.h>
#include <carl-settings/Settings.h>
#include <carl-settings/SettingsParser.h>
#include <gtest/gtest.h>

namespace po = boost::program_options;

struct TestSettings {
	bool test_bool;
	std::string test_string;
	bool use_preset;
};
bool finalize_settings(TestSettings& s, po::variables_map& values) {
	if (s.use_preset) {
		carl::settings::overwrite_to(values, "bool", true);
		carl::settings::default_to(values, "string", "default");
		return true;
	}
	return false;
}

struct Settings: public carl::Singleton<Settings>, public carl::settings::Settings {
	friend carl::Singleton<Settings>;
private:
	Settings() {
		get<TestSettings>("test");
	};
};

class SettingsParser: public carl::settings::SettingsParser, public carl::Singleton<SettingsParser> {
	friend carl::Singleton<SettingsParser>;
};

TEST(Settings, all) {
	char *argv[] = {"test", "--bool", "--string", "test", "--preset"};
	int argc = sizeof(argv) / sizeof(char*);

	auto& parser = SettingsParser::getInstance();
	auto& settings = Settings::getInstance();
	auto& s = settings.get<TestSettings>("test");
	parser.add("Settings").add_options()
		("bool", po::bool_switch(&s.test_bool))
		("string", po::value<std::string>(&s.test_string))
		("preset", po::bool_switch(&s.use_preset))
	;
	parser.add_finalizer([&s](auto& values) {
		return finalize_settings(s, values);
	});
	EXPECT_NO_THROW(parser.finalize());
	EXPECT_NO_THROW(parser.parse_options(argc, argv));
	{
		std::stringstream ss;
		EXPECT_NO_THROW(ss << parser.print_help());
	}
	{
		std::stringstream ss;
		EXPECT_NO_THROW(ss << parser.print_options());
	}
}
