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

    template<> inline void scale(int exp, mpq_class& r) {
        if (exp >= 0)
            r *= carl::pow(mpq_class(10), (unsigned)exp);
        else
            r /= carl::pow(mpq_class(10), (unsigned)(-exp));
    }
    template<> inline bool is_equal_to_one(const mpq_class& value) {
        return value == 1;
    }
}}}

namespace carl {
namespace parser {

template<typename Coeff>
struct PolynomialParser: public qi::grammar<Iterator, Poly<Coeff>(), Skipper> {
	PolynomialParser(): PolynomialParser<Coeff>::base_type(main, "polynomial") {
		operation.add("+", ADD)("-", SUB);
		varname = qi::lexeme[ (qi::alpha | qi::char_("_")) >> *(qi::alnum | qi::char_("_"))];
		variable = varname[qi::_val = px::bind(&PolynomialParser<Coeff>::newVariable, px::ref(*this), qi::_1)];
		monomial = (variable >> -('^' > exponentVal))[qi::_val = px::bind(&PolynomialParser<Coeff>::newMonomial, px::ref(*this), qi::_1, qi::_2)];
		term = ((   coeff[qi::_val = px::bind(&PolynomialParser<Coeff>::newTermC, px::ref(*this), qi::_1)]) |
		        (monomial[qi::_val = px::bind(&PolynomialParser<Coeff>::newTermM, px::ref(*this), qi::_1)]));
		expr = ("(" > expr_sum > ")")[qi::_val = qi::_1] |
		       term[qi::_val = px::bind(&PolynomialParser<Coeff>::newPoly, px::ref(*this), qi::_1)];
		expr_product = (expr % "*")[qi::_val = px::bind(&PolynomialParser<Coeff>::mulExpr, px::ref(*this), qi::_1)];
		expr_sum = (expr_product >> *(operation > expr_product))[qi::_val = px::bind(&PolynomialParser<Coeff>::addExpr, px::ref(*this), qi::_1, qi::_2)];
		main = expr_sum;

		varname.name("varname");
		variable.name("variable");
		monomial.name("monomial");
		term.name("term");
		expr.name("expr");
		expr_product.name("expr_product");
		expr_sum.name("expr_sum");
		main.name("main");
	}
	
	void addVariable(Variable::Arg v) {
		varmap.add(VariablePool::getInstance().getName(v), v);
	}
	
private:
	enum Operation { ADD, SUB };
	
	Variable newVariable(const std::string& s) {
		Variable* vptr = nullptr;
		if ((vptr = varmap.find(s)) != nullptr) {
			return *vptr;
		}
		Variable v = freshRealVariable(s);
		varmap.add(s, v);
		return v;
	}
	Monomial::Arg newMonomial(Variable v, const boost::optional<exponent> e) const {
		if (e) {
			return createMonomial(v, e.get());
		} else {
			return createMonomial(v, 1);
		}
	}
	Term<Coeff> newTermC(Coeff c) {
		return Term<Coeff>(c);
	}
	Term<Coeff> newTermM(Monomial::Arg m) {
		return Term<Coeff>(m);
	}
	Poly<Coeff> newPoly(const Term<Coeff>& t) {
		return Poly<Coeff>(t);
	}

	Poly<Coeff> mulExpr(const std::vector<Poly<Coeff>>& ops) {
		Poly<Coeff> res(Coeff(1));
		for (const auto& op: ops) {
			res *= op;
		}
		return res;
	}
	Poly<Coeff> addExpr(const Poly<Coeff>& first, const std::vector<boost::fusion::vector2<Operation,Poly<Coeff>>>& ops) {
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
	qi::int_parser<exponent,10,1,-1> exponentVal;
	qi::real_parser<Coeff,RationalPolicies<Coeff>> coeff;
	qi::rule<Iterator, Variable(), Skipper> variable;
	qi::rule<Iterator, Monomial::Arg(), Skipper> monomial;
	qi::rule<Iterator, Term<Coeff>(), Skipper> term;
	qi::rule<Iterator, Poly<Coeff>(), Skipper> expr;
	qi::rule<Iterator, Poly<Coeff>(), Skipper> expr_product;
	qi::rule<Iterator, Poly<Coeff>(), Skipper> expr_sum;
	qi::rule<Iterator, Poly<Coeff>(), Skipper> main;
};


}
}
