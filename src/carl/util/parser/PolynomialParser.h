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
struct PolynomialParser: public qi::grammar<Iterator,
		boost::variant< Coeff, Variable, Monomial::Arg, Term<Coeff>, Poly<Coeff>, RatFun<Coeff> >(),
		Skipper> {
	using expr_type = boost::variant< Coeff, Variable, Monomial::Arg, Term<Coeff>, Poly<Coeff>, RatFun<Coeff> >;

	class perform_addition: public boost::static_visitor<expr_type> {
	public:
		template<typename T, typename U>
		expr_type operator()(const T& lhs, const U& rhs) const {
			return Poly<Coeff>(lhs) + Poly<Coeff>(rhs);
		}

		expr_type operator()(const RatFun<Coeff>& lhs, const Monomial::Arg& rhs) const {
			return (lhs) + Poly<Coeff>(rhs);
		}

		expr_type operator()(const RatFun<Coeff>& lhs, const Term<Coeff>& rhs) const {
			return (lhs) + Poly<Coeff>(rhs);
		}

		template<typename T>
		expr_type operator()(const RatFun<Coeff>& lhs, const T& rhs) const {
			return (lhs) + (rhs);
		}

		template<typename T>
		expr_type operator()(const T& lhs, const RatFun<Coeff>& rhs) const {
			return (rhs) + Poly<Coeff>(lhs);
		}

		expr_type operator()(const RatFun<Coeff>& lhs, const RatFun<Coeff>& rhs) const {
			return (lhs) + (rhs);
		}
	};

	class perform_subtraction: public boost::static_visitor<expr_type> {
	public:
		template<typename T, typename U>
		expr_type operator()(const T& lhs, const U& rhs) const {
			return Poly<Coeff>(lhs) - Poly<Coeff>(rhs);
		}

		expr_type operator()(const RatFun<Coeff>& lhs, const Monomial::Arg& rhs) const {
			return (lhs) - Poly<Coeff>(rhs);
		}

		expr_type operator()(const RatFun<Coeff>& lhs, const Term<Coeff>& rhs) const {
			return (lhs) - Poly<Coeff>(rhs);
		}

		template<typename T>
		expr_type operator()(const RatFun<Coeff>& lhs, const T& rhs) const {
			return (lhs) - (rhs);
		}

		template<typename T>
		expr_type operator()(const T& lhs, const RatFun<Coeff>& rhs) const {
			return (rhs) - Poly<Coeff>(lhs);
		}

		expr_type operator()(const RatFun<Coeff>& lhs, const RatFun<Coeff>& rhs) const {
			return (lhs) - (rhs);
		}
	};

	class perform_multiplication: public boost::static_visitor<expr_type> {
	public:
		template<typename T, typename U>
		expr_type operator()(const T& lhs, const U& rhs) const {
			return lhs * rhs;
		}

		template<typename T>
		expr_type operator()(const T& lhs, const RatFun<Coeff>& rhs) const {
			return rhs * lhs;
		}

		expr_type operator()(const RatFun<Coeff>& lhs, const Monomial::Arg& rhs) const {
			return (lhs) * Poly<Coeff>(rhs);
		}

		expr_type operator()(const RatFun<Coeff>& lhs, const Term<Coeff>& rhs) const {
			return (lhs) * Poly<Coeff>(rhs);
		}

		expr_type operator()(const Monomial::Arg& lhs, const RatFun<Coeff>& rhs) const {
			return (rhs) * Poly<Coeff>(lhs);
		}

		expr_type operator()(const Term<Coeff>& lhs, const RatFun<Coeff>& rhs) const {
			return (rhs) * Poly<Coeff>(lhs);
		}
	};

	class perform_division: public boost::static_visitor<expr_type> {
	public:
		expr_type operator()(const RatFun<Coeff>& lhs, const Coeff& rhs) const {
			return lhs / rhs;
		}

		template<typename T>
		expr_type operator()(const RatFun<Coeff>& lhs, const T& rhs) const {
			return lhs / rhs;
		}

		expr_type operator()(const RatFun<Coeff>& lhs, const Monomial::Arg& rhs) const {
			return lhs / Poly<Coeff>(rhs);
		}

		expr_type operator()(const RatFun<Coeff>& lhs, const Term<Coeff>& rhs) const {
			return lhs / Poly<Coeff>(rhs);
		}

		expr_type operator()(const RatFun<Coeff>& lhs, const RatFun<Coeff>& rhs) const {
			return lhs / rhs;
		}

		template<typename T>
		expr_type operator()(const T& lhs, const Coeff& coeff) const {
			return lhs * reciprocal(coeff);
		}

		template<typename T>
		expr_type operator()(const T& lhs, const RatFun<Coeff>& rhs) const {
			// TODO: Not extremely efficient probably
			return  RatFun<Coeff>(rhs.denominator(), rhs.nominator()) * Poly<Coeff>(lhs);
		}

		template<typename T, typename U>
		expr_type operator()(const T& lhs, const U& rhs) const {
			return RatFun<Coeff>(Poly<Coeff>(lhs), Poly<Coeff>(rhs));
		}
	};

	class perform_power: public boost::static_visitor<expr_type> {
	public:
		exponent expVal;

		perform_power(exponent exp) : expVal(exp) {};

		template<typename T>
		expr_type operator()(const T& lhs) const {
			return lhs.pow(expVal);
		}

		expr_type operator()(const RatFun<Coeff>& lhs) const {
			return RatFun<Coeff>(lhs.nominator().pow(expVal), lhs.denominator().pow(expVal));
		}

		expr_type operator()(const Coeff& lhs) const {
			return carl::pow(lhs, expVal);
		}

		expr_type operator()(const Variable& lhs) const {
			return createMonomial(lhs, expVal);
		}

		expr_type operator()(const Monomial::Arg& lhs) const {
			return lhs->pow(expVal);
		}
	};

	class to_poly: public boost::static_visitor<Poly<Coeff>> {
	public:
		Poly<Coeff> operator()(const Poly<Coeff>& p) const {
			return p;
		}

		template<typename T>
		Poly<Coeff> operator()(const T& expr) const {
			return Poly<Coeff>(expr);
		}
	};

	PolynomialParser(): PolynomialParser<Coeff>::base_type(main, "polynomial") {
		/** Tokens */
		operation.add("+", ADD)("-", SUB);
		operationScale.add("*", MUL)("/", DIV);
		operationPow.add("^", POW)("**", POW);
		varname = qi::lexeme[ (qi::alpha | qi::char_("_")) >> *(qi::alnum | qi::char_("_"))];
		variable = varname[qi::_val = px::bind(&PolynomialParser<Coeff>::newVariable, px::ref(*this), qi::_1)];

		/** Rules */
		operationScaleLA = qi::lexeme[ operationScale >> !qi::lit("*") ][qi::_val = qi::_1];
		monomial = variable[qi::_val = px::bind(&PolynomialParser<Coeff>::newMonomial, px::ref(*this), qi::_1)];
		atom = (monomial[qi::_val = qi::_1] | coeff[qi::_val = qi::_1]);
		expr = ("(" > expr_sum > ")")[qi::_val = qi::_1] | atom[qi::_val = qi::_1];
		expr_power = (expr >> *(operationPow > exponentVal))[qi::_val = px::bind(&PolynomialParser<Coeff>::powExpr, px::ref(*this), qi::_1, qi::_2)];
		expr_product = (expr_power >> *(operationScaleLA > expr_power))[qi::_val = px::bind(&PolynomialParser<Coeff>::mulExpr, px::ref(*this), qi::_1, qi::_2)];
		expr_sum = (expr_product >> *(operation > expr_product))[qi::_val = px::bind(&PolynomialParser<Coeff>::addExpr, px::ref(*this), qi::_1, qi::_2)];
		main = expr_sum;//[qi::_val = px::bind(&PolynomialParser<Coeff>::makePoly, px::ref(*this), qi::_1)];

		varname.name("varname");
		variable.name("variable");
		monomial.name("monomial");
		atom.name("atom");
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
	expr_type newMonomial(Variable v) const {
		return createMonomial(v, 1);
	}

	expr_type addExpr(const expr_type& first, const std::vector<boost::fusion::vector2<Operation,expr_type>>& ops) {
		expr_type res = first;
		for (const auto& op: ops) {
			switch (boost::fusion::at_c<0>(op)) {
				case ADD: {
					res = boost::apply_visitor( perform_addition(), res, boost::fusion::at_c<1>(op) );
					break;
				}
				case SUB: {
					res = boost::apply_visitor( perform_subtraction(), res, boost::fusion::at_c<1>(op) );
					break;
				}
			}
		}
		return res;
	}
	expr_type mulExpr(const expr_type& first, const std::vector<boost::fusion::vector2<Operation,expr_type>>& ops) {
		expr_type res = first;
		for (const auto& op: ops) {
			switch (boost::fusion::at_c<0>(op)) {
				case MUL: {
					res = boost::apply_visitor( perform_multiplication(), res, boost::fusion::at_c<1>(op) );
					break;
				}
				case DIV: {
					res = boost::apply_visitor( perform_division(), res, boost::fusion::at_c<1>(op) );
					break;
				}
			}
		}
		return res;
	}
	
	expr_type powExpr(const expr_type& first, const std::vector<boost::fusion::vector2<Operation,exponent>>& ops) {
		expr_type res = first;
		for (const auto& op: ops) {
			switch (boost::fusion::at_c<0>(op)) {
				case POW: {
					res = boost::apply_visitor( perform_power(boost::fusion::at_c<1>(op)), res );
					break;
				}
			}
		}
		return res;
	}

	Poly<Coeff> makePoly(expr_type expr) {
		return boost::apply_visitor( to_poly(), expr );
	}

	qi::symbols<char, Operation> operation;
	qi::symbols<char, Operation> operationScale;
	qi::symbols<char, Operation> operationPow;
	qi::symbols<char, Variable> varmap;

	qi::rule<Iterator, std::string(), Skipper> varname;
	qi::uint_parser<exponent,10,1,-1> exponentVal;
	qi::real_parser<Coeff,RationalPolicies<Coeff>> coeff;
	qi::rule<Iterator, Variable(), Skipper> variable;

	qi::rule<Iterator, Operation(), Skipper> operationScaleLA;

	qi::rule<Iterator, expr_type(), Skipper> monomial;
	qi::rule<Iterator, expr_type(), Skipper> atom;

	qi::rule<Iterator, expr_type(), Skipper> expr;
	qi::rule<Iterator, expr_type(), Skipper> expr_product;
	qi::rule<Iterator, expr_type(), Skipper> expr_power;
	qi::rule<Iterator, expr_type(), Skipper> expr_sum;
	qi::rule<Iterator, expr_type(), Skipper> main;
};


}
}
