/**
 * @file PolynomialParser.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include "Common.h"

#include <boost/version.hpp>
#include <carl-arith/numbers/numbers.h>

#if BOOST_VERSION >= 105900
#ifdef USE_CLN_NUMBERS
namespace boost { namespace spirit { namespace traits {
    template<> inline bool scale(int exp, cln::cl_RA& r, cln::cl_RA acc) {
        if (exp >= 0)
            r = acc * carl::pow(cln::cl_RA(10), unsigned(exp));
        else
            r = acc / carl::pow(cln::cl_RA(10), unsigned(-exp));
		return true;
    }
#if BOOST_VERSION < 107000
    template<> inline bool is_equal_to_one(const cln::cl_RA& value) {
        return value == 1;
    }
#endif
}}}
#endif
namespace boost { namespace spirit { namespace traits {
    template<> inline bool scale(int exp, mpq_class& r, mpq_class acc) {
        if (exp >= 0)
            r = acc * carl::pow(mpq_class(10), unsigned(exp));
        else
            r = acc / carl::pow(mpq_class(10), unsigned(-exp));
		return true;
    }
#if BOOST_VERSION < 107000
    template<> inline bool is_equal_to_one(const mpq_class& value) {
        return value == 1;
    }
#endif
    template<> inline mpq_class negate(bool neg, const mpq_class& n) {
        return neg ? mpq_class(-n) : n;
    }
}}}
#else
#ifdef USE_CLN_NUMBERS
namespace boost { namespace spirit { namespace traits {
    template<> inline void scale(int exp, cln::cl_RA& n) {
        if (exp >= 0)
            n *= carl::pow(cln::cl_RA(10), unsigned(exp));
        else
            n /= carl::pow(cln::cl_RA(10), unsigned(-exp));
    }
#if BOOST_VERSION < 107000
    template<> inline bool is_equal_to_one(const cln::cl_RA& value) {
        return value == 1;
    }
#endif
}}}
#endif
namespace boost { namespace spirit { namespace traits {
    template<> inline void scale(int exp, mpq_class& n) {
        if (exp >= 0)
            n *= carl::pow(mpq_class(10), unsigned(exp));
        else
            n /= carl::pow(mpq_class(10), unsigned(-exp));
    }
    template<> inline bool is_equal_to_one(const mpq_class& value) {
        return value == 1;
    }
    template<> inline mpq_class negate(bool neg, const mpq_class& n) {
        return neg ? mpq_class(-n) : n;
    }
}}}
#endif

namespace carl::io {
namespace parser {

template<typename Pol>
struct PolynomialParser: public qi::grammar<Iterator, Pol(), Skipper> {
	PolynomialParser():
		PolynomialParser<Pol>::base_type(main, "polynomial"),
		operation(), varmap(), varname(), number(),
		variable(), monomial(), term(), polynomial(),
		expr(), expr_product(), expr_sum(), main()
	{
		operation.add("+", ADD)("-", SUB);
		varname = qi::lexeme[ (qi::alpha | qi::char_("~!@$%^&_=<>.?/")) > *(qi::alnum | qi::char_("~!@$%^&_=<>.?/"))];
		variable = (varmap[qi::_val = qi::_1]) | (varname[qi::_val = px::bind(&PolynomialParser<Pol>::newVariable, px::ref(*this), qi::_1)]);
		monomial = ((variable >> ("^" >> number | qi::attr(typename Pol::CoeffType(1)))) % "*")[qi::_val = px::bind(&PolynomialParser<Pol>::newMonomial, px::ref(*this), qi::_1)];
		term = (-number >> -monomial)[qi::_val = px::bind(&PolynomialParser<Pol>::newTerm, px::ref(*this), qi::_1, qi::_2)];
		polynomial = (term >> *(operation >> term))[qi::_val = px::bind(&PolynomialParser<Pol>::addTerms, px::ref(*this), qi::_1, qi::_2)];
		expr = ("(" >> expr_sum >> ")") | polynomial;
		expr_product = (expr % "*")[qi::_val = px::bind(&PolynomialParser<Pol>::mul, px::ref(*this), qi::_1)];
		expr_sum = (expr_product >> *(operation >> expr_product))[qi::_val = px::bind(&PolynomialParser<Pol>::addPolynomials, px::ref(*this), qi::_1, qi::_2)];
		main = expr_sum;
	}
	
	void addVariable(Variable::Arg v) {
		varmap.add(VariablePool::getInstance().get_name(v), v);
	}
	
private:
	enum Operation { ADD, SUB };
	
	Variable newVariable(const std::string& s) {
		Variable v = fresh_real_variable(s);
		varmap.add(s, v);
		return v;
	}
	Monomial::Arg newMonomial(const std::vector<boost::fusion::vector2<Variable,typename Pol::CoeffType>>& data) const {
		Monomial::Arg res;
		for (const auto& term: data) {
			res = res * createMonomial(boost::fusion::at_c<0>(term), exponent(carl::to_int<uint>(boost::fusion::at_c<1>(term))));
		}
		return res;
	}
	Term<typename Pol::CoeffType> newTerm(const boost::optional<typename Pol::CoeffType>& c, const boost::optional<Monomial::Arg>& m) {
		if (c && m) return Term<typename Pol::CoeffType>(c.get(), m.get());
		else if (c) return Term<typename Pol::CoeffType>(c.get());
		else if (m) return Term<typename Pol::CoeffType>(m.get());
		CARL_LOG_ERROR("carl.parser", "Parsed an empty term.");
		return Term<typename Pol::CoeffType>();
	}
	Pol addTerms(const Term<typename Pol::CoeffType>& first, const std::vector<boost::fusion::vector2<Operation,Term<typename Pol::CoeffType>>>& ops) {
		Pol res(first);
		for (const auto& op: ops) {
			switch (boost::fusion::at_c<0>(op)) {
			case ADD: res += boost::fusion::at_c<1>(op); break;
			case SUB: res -= boost::fusion::at_c<1>(op); break;
			}
		}
		return res;
	}
	Pol mul(const std::vector<Pol>& ops) {
		Pol res(typename Pol::CoeffType(1));
		for (const auto& op: ops) res *= op;
		return res;
	}
	Pol addPolynomials(const Pol& first, const std::vector<boost::fusion::vector2<Operation,Pol>>& ops) {
		Pol res = first;
		for (const auto& op: ops) {
			switch (boost::fusion::at_c<0>(op)) {
			case ADD: res += boost::fusion::at_c<1>(op); break;
			case SUB: res -= boost::fusion::at_c<1>(op); break;
			}
		}
		return res;
	}
	
	qi::symbols<char, Operation> operation;
	qi::symbols<char, Variable> varmap;
	qi::rule<Iterator, std::string(), Skipper> varname;
	qi::real_parser<typename Pol::CoeffType,RationalPolicies<typename Pol::CoeffType>> number;
	qi::rule<Iterator, Variable(), Skipper> variable;
	qi::rule<Iterator, Monomial::Arg(), Skipper> monomial;
	qi::rule<Iterator, Term<typename Pol::CoeffType>(), Skipper> term;
	qi::rule<Iterator, Pol(), Skipper, qi::locals<Pol>> polynomial;
	qi::rule<Iterator, Pol(), Skipper> expr;
	qi::rule<Iterator, Pol(), Skipper> expr_product;
	qi::rule<Iterator, Pol(), Skipper, qi::locals<Pol>> expr_sum;
	qi::rule<Iterator, Pol(), Skipper> main;
};


}
}
