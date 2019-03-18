#include "settings_utils.h"

#include <boost/program_options.hpp>
#include <boost/spirit/include/qi.hpp>

namespace carl::settings {

// Adapted from https://stackoverflow.com/questions/45071699/
void validate(boost::any& v, const std::vector<std::string>& values, carl::settings::duration* /*unused*/, int /*unused*/) {
	namespace pov = boost::program_options::validators;
	namespace qi = boost::spirit::qi;

	std::string s(pov::get_single_string(values));

	long value = 0;
	std::chrono::nanoseconds factor;
	qi::symbols<char, std::chrono::nanoseconds> unit;
	unit.add
		("ns", std::chrono::nanoseconds(1))
		("µs", std::chrono::microseconds(1))
		("us", std::chrono::microseconds(1))
		("ms", std::chrono::milliseconds(1))
		("s", std::chrono::seconds(1))
		("m", std::chrono::minutes(1))
		("h", std::chrono::hours(1))
	;
	if (qi::parse(s.begin(), s.end(), qi::long_ >> unit >> qi::eoi, value, factor)) {
		v = duration(value * factor);
	} else {
		throw boost::program_options::invalid_option_value(s);
	}
}

void validate(boost::any& v, const std::vector<std::string>& values, carl::settings::binary_quantity* /*unused*/, int /*unused*/) {
	namespace pov = boost::program_options::validators;
	namespace qi = boost::spirit::qi;

	std::string s(pov::get_single_string(values));

	std::size_t value = 0;
	std::size_t factor = 1;
	qi::symbols<char, std::size_t> unit;
	unit.add
		("K", static_cast<std::size_t>(1) << 10U)
		("Ki", static_cast<std::size_t>(1) << 10U)
		("M", static_cast<std::size_t>(1) << 20U)
		("Mi", static_cast<std::size_t>(1) << 20U)
		("G", static_cast<std::size_t>(1) << 30U)
		("Gi", static_cast<std::size_t>(1) << 30U)
		("T", static_cast<std::size_t>(1) << 40U)
		("Ti", static_cast<std::size_t>(1) << 40U)
		("P", static_cast<std::size_t>(1) << 50U)
		("Pi", static_cast<std::size_t>(1) << 50U)
		("E", static_cast<std::size_t>(1) << 60U)
		("Ei", static_cast<std::size_t>(1) << 60U)
	;
	if (qi::parse(s.begin(), s.end(), qi::ulong_long >> -unit >> qi::eoi, value, factor)) {
		v = binary_quantity(value * factor);
	} else {
		throw boost::program_options::invalid_option_value(s);
	}
}

void validate(boost::any& v, const std::vector<std::string>& values, carl::settings::metric_quantity* /*unused*/, int /*unused*/) {
	namespace pov = boost::program_options::validators;
	namespace qi = boost::spirit::qi;

	std::string s(pov::get_single_string(values));

	std::size_t value = 0;
	std::size_t factor = 1;
	qi::symbols<char, std::size_t> unit;
	unit.add
		("K", 1000)
		("M", 1000000)
		("G", 1000000000)
		("T", 1000000000000)
		("P", 1000000000000000)
		("E", 1000000000000000000)
	;
	if (qi::parse(s.begin(), s.end(), qi::ulong_long >> -unit >> qi::eoi, value, factor)) {
		v = metric_quantity(value * factor);
	} else {
		throw boost::program_options::invalid_option_value(s);
	}
}

}