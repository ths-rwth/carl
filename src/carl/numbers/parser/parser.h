#pragma once

#define BOOST_SPIRIT_USE_PHOENIX_V3
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_parse.hpp>

namespace carl {
namespace parser {
	
	/**
	 * Specialization of qi::real_policies for our rational types.
	 * Specifies that neither NaN nor Inf is allowed.
	 */
	template<typename T>
	struct RationalPolicies : boost::spirit::qi::real_policies<T> {
	    template <typename It, typename Attr>
	    static bool parse_nan(It&, It const&, Attr&) { return false; }
	    template <typename It, typename Attr>
	    static bool parse_inf(It&, It const&, Attr&) { return false; }
	};

	/**
	 * Parses integers.
	 */
	template<typename T>
	struct IntegerParser: boost::spirit::qi::uint_parser<T,10,1,-1> {};

	/**
	 * Parses to rationals.
	 */
	template<typename T>
	struct RationalParser: boost::spirit::qi::real_parser<T, RationalPolicies<T>> {};
	
	template<typename T>
	bool parseInteger(const std::string& input, T& output) {
		return parse(input.begin(), input.end(), IntegerParser<T>(), output);
	}
	template<typename T>
	bool parseRational(const std::string& input, T& output) {
		return parse(input.begin(), input.end(), RationalParser<T>(), output);
	}
}
}
