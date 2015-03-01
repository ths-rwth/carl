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
    template<> inline mpq_class negate(bool b, const mpq_class& n) {
        if (b) return (n * -1);
        return n;
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
		operationScale.add("*", MUL)("/", DIV);
		operationPow.add("^", POW);
		varname = qi::lexeme[ (qi::alpha | qi::char_("_")) >> *(qi::alnum | qi::char_("_"))];
		variable = varname[qi::_val = px::bind(&PolynomialParser<Coeff>::newVariable, px::ref(*this), qi::_1)];
		monomial = (variable >> -('^' > exponentVal))[qi::_val = px::bind(&PolynomialParser<Coeff>::newMonomial, px::ref(*this), qi::_1, qi::_2)];
		term = ((   coeff[qi::_val = px::bind(&PolynomialParser<Coeff>::newTermC, px::ref(*this), qi::_1)]) |
		        (monomial[qi::_val = px::bind(&PolynomialParser<Coeff>::newTermM, px::ref(*this), qi::_1)]));
		expr = ("(" > expr_sum > ")")[qi::_val = qi::_1] |
		       term[qi::_val = px::bind(&PolynomialParser<Coeff>::newPoly, px::ref(*this), qi::_1)];
		expr_power= (expr >> *(operationPow > expr))[qi::_val = px::bind(&PolynomialParser<Coeff>::powExpr, px::ref(*this), qi::_1, qi::_2)];
		expr_product= (expr_power >> *(operationScale > expr_power))[qi::_val = px::bind(&PolynomialParser<Coeff>::mulExpr, px::ref(*this), qi::_1, qi::_2)];
		expr_sum = (expr_product >> *(operation > expr_product))[qi::_val = px::bind(&PolynomialParser<Coeff>::addExpr, px::ref(*this), qi::_1, qi::_2)];
		main = expr_sum;

		varname.name("varname");
		variable.name("variable");
		monomial.name("monomial");
		term.name("term");
		expr.name("expr");
		expr_power.name("expr_power");
		expr_product.name("expr_product");
		expr_sum.name("expr_sum");
		main.name("main");
	}
	
	void addVariable(Variable::Arg v) {
		varmap.add(VariablePool::getInstance().getName(v), v);
	}
	
private:
	enum Operation { ADD, SUB, MUL, DIV, POW };
	
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

	Poly<Coeff> mulExpr(const Poly<Coeff>& first, const std::vector<boost::fusion::vector2<Operation,Poly<Coeff>>>& ops) {
		Poly<Coeff> res = first;
		for (const auto& op: ops) {
			switch (boost::fusion::at_c<0>(op)) {
			case MUL: res *= boost::fusion::at_c<1>(op); break;
			case DIV: {
				const Poly<Coeff>& div = boost::fusion::at_c<1>(op);
				if (div.isConstant()) {
					res /= div.constantPart();
				}
			}
			}
		}
		return res;
	}
	Poly<Coeff> powExpr(const Poly<Coeff>& first, const std::vector<boost::fusion::vector2<Operation,Poly<Coeff>>>& ops) {
		Poly<Coeff> res = first;
		for (const auto& op: ops) {
			switch (boost::fusion::at_c<0>(op)) {
			case POW: {
				const Poly<Coeff>& div = boost::fusion::at_c<1>(op);
				if (div.isConstant()) {
					auto num = div.constantPart().get_num().get_ui();
					res = res.pow(num);
				}
			}
			}
		}
		return res;
	}
	Poly<Coeff> divExpr(Poly<Coeff> poly, const std::vector<Coeff>& ops) {
		for (const auto& op: ops) {
			poly *= carl::reciprocal(op);
		}
		return poly;
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
	qi::symbols<char, Operation> operationScale;
	qi::symbols<char, Operation> operationPow;
	qi::symbols<char, Variable> varmap;
	qi::rule<Iterator, std::string(), Skipper> varname;
	qi::uint_parser<exponent,10,1,-1> exponentVal;
	qi::real_parser<Coeff,RationalPolicies<Coeff>> coeff;
	qi::rule<Iterator, Variable(), Skipper> variable;
	qi::rule<Iterator, Monomial::Arg(), Skipper> monomial;
	qi::rule<Iterator, Term<Coeff>(), Skipper> term;
	qi::rule<Iterator, Poly<Coeff>(), Skipper> expr;
	qi::rule<Iterator, Poly<Coeff>(), Skipper> expr_product;
	qi::rule<Iterator, Poly<Coeff>(), Skipper> expr_power;
	qi::rule<Iterator, Poly<Coeff>(), Skipper> expr_sum;
	qi::rule<Iterator, Poly<Coeff>(), Skipper> main;
};


}
}
