#pragma once

#include "../../util/SFINAE.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_line_pos_iterator.hpp>

namespace carl {
namespace parser {
	namespace spirit = boost::spirit;
	namespace qi = boost::spirit::qi;

	struct ErrorHandler {
		template<typename> struct result { typedef qi::error_handler_result type; };
		template<typename T1, typename T2>
		qi::error_handler_result operator()(T1 b, T1 e, T1 where, T2 const& what) const {
			static_assert(carl::is_instantiation_of<spirit::line_pos_iterator, T1>::value, "The iterators must be a line_pos_iterator.");
			auto line_start = spirit::get_line_start(b, where);
			auto line_end = std::find(where, e, '\n');
			std::string line(++line_start, line_end);
		
			std::cout << "Parsing error at " << spirit::get_line(where) << ":" << spirit::get_column(line_start, where) << std::endl;
			std::cout << "expected" << std::endl << "\t" << what.tag << ": " << what << std::endl;
			std::cout << "but got" << std::endl << "\t" << std::string(where, line_end) << std::endl;
			std::cout << "in line " << spirit::get_line(where) << std::endl << "\t" << line << std::endl;
			return qi::fail;
		}
	};

}
}
