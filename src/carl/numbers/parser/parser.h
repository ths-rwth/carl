#pragma once

#include <boost/optional.hpp>

#define BOOST_SPIRIT_USE_PHOENIX_V3
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_parse.hpp>

namespace carl {
namespace parser {

	namespace qi = boost::spirit::qi;
	namespace px = boost::phoenix;

	using Skipper = qi::space_type;

	/**
	 * Specialization of qi::real_policies for our rational types.
	 * Specifies that neither NaN nor Inf is allowed.
	 */
	template<typename T>
	struct RationalPolicies : qi::real_policies<T> {
	    template <typename It, typename Attr>
	    static bool parse_nan(It&, It const&, Attr&) { return false; }
	    template <typename It, typename Attr>
	    static bool parse_inf(It&, It const&, Attr&) { return false; }
	};

	/// Parses (signed) integers
	template<typename T>
	struct IntegerParser: qi::int_parser<T,10,1,-1> {};

	/// Parses decimals, including floating point and scientific notation
	template<typename T>
	struct DecimalParser: qi::real_parser<T, RationalPolicies<T>> {};
	
	/// Parses rationals, being two decimals separated by a slash
	template<typename T, typename Iterator = std::string::const_iterator>
	struct RationalParser: public qi::grammar<Iterator, T(), Skipper> {
		RationalParser(): RationalParser::base_type(main, "rational") {
			main = (number >> -(qi::lit('/') >> number))[qi::_val = px::bind(&RationalParser::makeRational, px::ref(*this), qi::_1, qi::_2)];
		}
		DecimalParser<T> number;
		qi::rule<Iterator, T(), Skipper> main;
		T makeRational(const T& a, const boost::optional<T>& b) const {
			if (!b) return a;
			return a / *b;
		}
	};
	
	template<typename T>
	bool parseInteger(const std::string& input, T& output) {
		return parse(input.begin(), input.end(), IntegerParser<T>(), output);
	}
	template<typename T>
	bool parseDecimal(const std::string& input, T& output) {
		return parse(input.begin(), input.end(), DecimalParser<T>(), output);
	}
	template<typename T>
	bool parseRational(const std::string& input, T& output) {
		Skipper s;
		auto it = input.begin();
		bool res = phrase_parse(it, input.end(), RationalParser<T>(), s, output);
		if (res) {
			return it == input.end();
		}
		return false;
	}
}
}
