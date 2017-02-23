#pragma once

#include <boost/optional.hpp>

#define BOOST_SPIRIT_USE_PHOENIX_V3
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_parse.hpp>

#include "../typetraits.h"

namespace carl {
namespace parser {

	namespace qi = boost::spirit::qi;
	namespace px = boost::phoenix;

	using Skipper = qi::space_type;
	
	template<bool is_int> struct isDivisible {};
	template<> struct isDivisible<true> {
		template<typename Attr>
		bool operator()(const Attr& n, std::size_t exp) {
			std::cout << "Checking mod(" << n << ", " << Attr(carl::pow(Attr(10), exp)) << ")" << std::endl;
			return isZero(carl::mod(n, Attr(carl::pow(Attr(10), exp))));
		}
	};
	template<> struct isDivisible<false> {
		template<typename Attr>
		bool operator()(const Attr&, std::size_t) {
			return true;
		}
	};

	/**
	 * Specialization of qi::real_policies for our rational types.
	 * Specifies that neither NaN nor Inf is allowed.
	 */
	template<typename T>
	struct RationalPolicies : qi::real_policies<T> {
		static constexpr bool T_is_int = carl::is_subset_of_integers<T>::value;
		static constexpr bool allow_leading_dot = true;
		static constexpr bool allow_trailing_dot = true;
		static constexpr bool expect_dot = false;
		
		template <typename It>
		static bool parse_dot(It& first, const It& last) {
			return qi::real_policies<T>::parse_dot(first, last);
		}
#if BOOST_VERSION >= 105900
		template <typename It, typename Attr>
		static bool parse_frac_n(It& first, const It& last, Attr& attr, int& frac_digits) {
			It save = first;
			if (T_is_int) {
				Attr local_attr;
				int local_frac_digits;
				bool res = qi::real_policies<T>::parse_frac_n(first, last, local_attr, local_frac_digits);
				if (!res) return false;
				if (!isZero(local_attr)) {
					first = save;
					return false;
				}
				return true;
			}
			return qi::real_policies<T>::parse_frac_n(first, last, attr, frac_digits);
		}
#else
		template <typename It, typename Attr>
		static bool parse_frac_n(It& first, const It& last, Attr& attr) {
			It save = first;
			if (T_is_int) {
				Attr local_attr;
				bool res = qi::real_policies<T>::parse_frac_n(first, last, local_attr);
				if (!res) return false;
				if (!isZero(local_attr)) {
					first = save;
					return false;
				}
				return true;
			}
			return qi::real_policies<T>::parse_frac_n(first, last, attr);
		}
#endif
		template <typename It, typename Attr>
		static bool parse_exp_n(It& first, const It& last, Attr& attr_) {
			bool res = qi::real_policies<T>::parse_exp_n(first, last, attr_);
			if (!res) return false;
			if (T_is_int) return attr_ >= 0;
			return true;
		}
		template <typename It, typename Attr>
		static bool parse_nan(It&, const It&, Attr&) {
			return false;
		}
		template <typename It, typename Attr>
		static bool parse_inf(It&, const It&, Attr&) {
			return false;
		}
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
	
	template<typename Parser, typename T>
	bool parse_impl(const std::string& input, T& output) {
		auto it = input.begin();
		if (parse(it, input.end(), Parser(), output)) {
			return it == input.end();
		}
		return false;
	}
	template<typename Parser, typename T, typename S>
	bool parse_impl(const std::string& input, T& output, const S& skipper) {
		auto it = input.begin();
		if (phrase_parse(it, input.end(), Parser(), skipper, output)) {
			return it == input.end();
		}
		return false;
	}
	
	template<typename T>
	bool parseInteger(const std::string& input, T& output) {
		return parse_impl<DecimalParser<T>>(input, output);
	}
	template<typename T>
	bool parseDecimal(const std::string& input, T& output) {
		return parse_impl<DecimalParser<T>>(input, output);
	}
	template<typename T>
	bool parseRational(const std::string& input, T& output) {
		return parse_impl<RationalParser<T>>(input, output, Skipper());
	}
}
}
