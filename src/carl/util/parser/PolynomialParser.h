/**
 * @file PolynomialParser.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include "Common.h"

namespace boost { namespace spirit { namespace traits {
    template<> inline void scale(int exp, cln::cl_RA& r) {
        if (exp >= 0)
            r *= carl::pow(cln::cl_RA(10), (unsigned)exp);
        else
            r /= carl::pow(cln::cl_RA(10), (unsigned)(-exp));
    }
    template<> inline bool is_equal_to_one(const cln::cl_RA& value) {
        return value == 1;
    }
}}}

namespace carl {
namespace parser {

template<typename Coeff>
struct PolynomialParser: public qi::grammar<Iterator, Poly<Coeff>(), Skipper> {
	PolynomialParser(): PolynomialParser<Coeff>::base_type(main, "polynomial") {
		operation.add("+", ADD)("-", SUB);
		varname = qi::lexeme[ (qi::alpha | qi::char_("~!@$%^&*_+=<>.?/-")) > *(qi::alnum | qi::char_("~!@$%^&*_+=<>.?/-"))];
		variable = (varmap[qi::_val = qi::_1]) | (varname[qi::_val = px::bind(&PolynomialParser<Coeff>::newVariable, px::ref(*this), qi::_1)]);
		monomial = ((variable >> ("^" >> number | qi::attr(Coeff(1)))) % "*")[qi::_val = px::bind(&PolynomialParser<Coeff>::newMonomial, px::ref(*this), qi::_1)];
		term = (-number >> -monomial)[qi::_val = px::bind(&PolynomialParser<Coeff>::newTerm, px::ref(*this), qi::_1, qi::_2)];
		polynomial = (term >> *(operation >> term))[qi::_val = px::bind(&PolynomialParser<Coeff>::addTerms, px::ref(*this), qi::_1, qi::_2)];
		expr = ("(" >> expr_sum >> ")") | polynomial;
		expr_product = (expr % "*")[qi::_val = px::bind(&PolynomialParser<Coeff>::mul, px::ref(*this), qi::_1)];
		expr_sum = (expr_product >> *(operation >> expr_product))[qi::_val = px::bind(&PolynomialParser<Coeff>::addPolynomials, px::ref(*this), qi::_1, qi::_2)];
		main = expr_sum;
	}
	
	void addVariable(Variable::Arg v) {
		varmap.add(VariablePool::getInstance().getName(v), v);
	}
	
private:
	enum Operation { ADD, SUB };
	
	Variable newVariable(const std::string& s) {
		Variable v = freshRealVariable(s);
		varmap.add(s, v);
		return v;
	}
	Monomial::Arg newMonomial(const std::vector<boost::fusion::vector2<Variable,Coeff>>& data) const {
		Monomial::Arg res;
		for (const auto& term: data) {
			res = res * createMonomial(boost::fusion::at_c<0>(term), exponent(carl::toInt<std::size_t>(boost::fusion::at_c<1>(term))));
		}
		return res;
	}
	Term<Coeff> newTerm(const boost::optional<Coeff>& c, const boost::optional<Monomial::Arg>& m) {
		if (c && m) return Term<Coeff>(c.get(), m.get());
		else if (c) return Term<Coeff>(c.get());
		else if (m) return Term<Coeff>(m.get());
		CARL_LOG_ERROR("carl.parser", "Parsed an empty term.");
		return Term<Coeff>();
	}
	Poly<Coeff> addTerms(const Term<Coeff>& first, const std::vector<boost::fusion::vector2<Operation,Term<Coeff>>>& ops) {
		Poly<Coeff> res(first);
		for (const auto& op: ops) {
			switch (boost::fusion::at_c<0>(op)) {
			case ADD: res += boost::fusion::at_c<1>(op); break;
			case SUB: res -= boost::fusion::at_c<1>(op); break;
			}
		}
		return res;
	}
	Poly<Coeff> mul(const std::vector<Poly<Coeff>>& ops) {
		Poly<Coeff> res(Coeff(1));
		for (const auto& op: ops) res *= op;
		return res;
	}
	Poly<Coeff> addPolynomials(const Poly<Coeff>& first, const std::vector<boost::fusion::vector2<Operation,Poly<Coeff>>>& ops) {
		Poly<Coeff> res = first;
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
	qi::real_parser<Coeff,RationalPolicies<Coeff>> number;
	qi::rule<Iterator, Variable(), Skipper> variable;
	qi::rule<Iterator, Monomial::Arg(), Skipper> monomial;
	qi::rule<Iterator, Term<Coeff>(), Skipper> term;
	qi::rule<Iterator, Poly<Coeff>(), Skipper, qi::locals<Poly<Coeff>>> polynomial;
	qi::rule<Iterator, Poly<Coeff>(), Skipper> expr;
	qi::rule<Iterator, Poly<Coeff>(), Skipper> expr_product;
	qi::rule<Iterator, Poly<Coeff>(), Skipper, qi::locals<Poly<Coeff>>> expr_sum;
	qi::rule<Iterator, Poly<Coeff>(), Skipper> main;
};


}
}