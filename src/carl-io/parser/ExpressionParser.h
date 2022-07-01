/**
 * @file PolynomialParser.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include "Common.h"
#include "ExpressionParserResult.h"

#if BOOST_VERSION >= 105900
#ifdef USE_CLN_NUMBERS
namespace boost { namespace spirit { namespace traits {
    template<> inline bool scale(int exp, cln::cl_RA& r, cln::cl_RA acc) {
        if (exp >= 0)
            r = acc * carl::pow(cln::cl_RA(10), (unsigned)exp);
        else
            r = acc / carl::pow(cln::cl_RA(10), (unsigned)(-exp));
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
            r = acc * carl::pow(mpq_class(10), (unsigned)exp);
        else
            r = acc / carl::pow(mpq_class(10), (unsigned)(-exp));
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
    template<> inline void scale(int exp, cln::cl_RA& r) {
        if (exp >= 0)
            r *= carl::pow(cln::cl_RA(10), (unsigned)exp);
        else
            r /= carl::pow(cln::cl_RA(10), (unsigned)(-exp));
    }
#if BOOST_VERSION < 107000
    template<> inline bool is_equal_to_one(const cln::cl_RA& value) {
        return value == 1;
    }
#endif
}}}
#endif
namespace boost { namespace spirit { namespace traits {
    template<> inline void scale(int exp, mpq_class& r) {
        if (exp >= 0)
            r *= carl::pow(mpq_class(10), (unsigned)exp);
        else
            r /= carl::pow(mpq_class(10), (unsigned)(-exp));
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
#endif

namespace carl::io {
namespace parser {

template<typename Pol>
struct ExpressionParser: public qi::grammar<Iterator, ExpressionType<Pol>(), Skipper> {
    typedef typename Pol::CoeffType CoeffType;
	using expr_type = ExpressionType<Pol>;

	class perform_addition: public boost::static_visitor<expr_type> {
	public:
		template<typename T, typename U>
		expr_type operator()(const T& lhs, const U& rhs) const {
			return Pol(lhs) + Pol(rhs);
		}

		expr_type operator()(const CoeffType& lhs, const CoeffType& rhs) const {
			return (lhs) + (rhs);
		}

		expr_type operator()(const RatFun<Pol>& lhs, const Monomial::Arg& rhs) const {
			return (lhs) + Pol(rhs);
		}

		expr_type operator()(const RatFun<Pol>& lhs, const Term<CoeffType>& rhs) const {
			return (lhs) + Pol(rhs);
		}

		template<typename T>
		typename std::enable_if<!std::is_same<Formula<Pol>, T>::value, expr_type>::type
		operator()(const RatFun<Pol>& lhs, const T& rhs) const {
			return (lhs) + (rhs);
		}

		template<typename T>
		typename std::enable_if<!std::is_same<Formula<Pol>, T>::value, expr_type>::type
		operator()(const T& lhs, const RatFun<Pol>& rhs) const {
			return (rhs) + Pol(lhs);
		}

		expr_type operator()(const RatFun<Pol>& lhs, const RatFun<Pol>& rhs) const {
			return (lhs) + (rhs);
		}

		template<typename T>
		expr_type operator()(const Formula<Pol>& lhs, const T& rhs) const {
			throw std::runtime_error("No addition for formula");
		}

		template<typename T>
		typename std::enable_if<!std::is_same<Formula<Pol>, T>::value, expr_type>::type
		operator()(const T& lhs, const Formula<Pol>& rhs) const {
			throw std::runtime_error("No addition for formula");
		}
	};

	class perform_subtraction: public boost::static_visitor<expr_type> {
	public:
		template<typename T, typename U>
		expr_type operator()(const T& lhs, const U& rhs) const {
			return Pol(lhs) - Pol(rhs);
		}

		expr_type operator()(const CoeffType& lhs, const CoeffType& rhs) const {
			return (lhs) - (rhs);
		}

		expr_type operator()(const RatFun<Pol>& lhs, const Monomial::Arg& rhs) const {
			return (lhs) - Pol(rhs);
		}

		expr_type operator()(const RatFun<Pol>& lhs, const Term<CoeffType>& rhs) const {
			return (lhs) - Pol(rhs);
		}

		template<typename T>
		typename std::enable_if<!std::is_same<Formula<Pol>, T>::value, expr_type>::type
		operator()(const RatFun<Pol>& lhs, const T& rhs) const {
			return (lhs) - (rhs);
		}

		template<typename T>
		typename std::enable_if<!std::is_same<Formula<Pol>, T>::value, expr_type>::type
		operator()(const T& lhs, const RatFun<Pol>& rhs) const {
			return (rhs) - Pol(lhs);
		}

		expr_type operator()(const RatFun<Pol>& lhs, const RatFun<Pol>& rhs) const {
			return (lhs) - (rhs);
		}

		template<typename T>
		expr_type operator()(const Formula<Pol>& lhs, const T& rhs) const {
			throw std::runtime_error("No subtraction for formula");
		}

		template<typename T>
		typename std::enable_if<!std::is_same<Formula<Pol>, T>::value, expr_type>::type
		operator()(const T& lhs, const Formula<Pol>& rhs) const {
			throw std::runtime_error("No subtraction for formula");
		}
	};

	class perform_multiplication: public boost::static_visitor<expr_type> {
	public:
		template<typename T, typename U>
		typename std::enable_if<!std::is_same<Formula<Pol>, T>::value, expr_type>::type
		 operator()(const T& lhs, const U& rhs) const {
			return lhs * rhs;
		}

		template<typename T>
		typename std::enable_if<!std::is_same<Formula<Pol>, T>::value, expr_type>::type
		operator()(const T& lhs, const RatFun<Pol>& rhs) const {
			return rhs * lhs;
		}

		expr_type operator()(const RatFun<Pol>& lhs, const Monomial::Arg& rhs) const {
			return (lhs) * Pol(rhs);
		}

		expr_type operator()(const RatFun<Pol>& lhs, const Term<CoeffType>& rhs) const {
			return (lhs) * Pol(rhs);
		}

		expr_type operator()(const Monomial::Arg& lhs, const RatFun<Pol>& rhs) const {
			return (rhs) * Pol(lhs);
		}

		expr_type operator()(const Term<CoeffType>& lhs, const RatFun<Pol>& rhs) const {
			return (rhs) * Pol(lhs);
		}

		template<typename T>
		expr_type operator()(const Formula<Pol>& lhs, const T& rhs) const {
			throw std::runtime_error("No multiplication for formula");
		}

		template<typename T>
		typename std::enable_if<!std::is_same<Formula<Pol>, T>::value, expr_type>::type
		operator()(const T& lhs, const Formula<Pol>& rhs) const {
			throw std::runtime_error("No multiplication for formula");
		}
	};

	class perform_division: public boost::static_visitor<expr_type> {
	public:
		expr_type operator()(const RatFun<Pol>& lhs, const CoeffType& rhs) const {
			return lhs / rhs;
		}

		template<typename T>
		typename std::enable_if<!std::is_base_of<Formula<Pol>, T>::value, expr_type>::type
		operator()(const RatFun<Pol>& lhs, const T& rhs) const {
			return lhs / rhs;
		}

		expr_type operator()(const RatFun<Pol>& lhs, const Monomial::Arg& rhs) const {
			return lhs / Pol(rhs);
		}

		expr_type operator()(const RatFun<Pol>& lhs, const Term<CoeffType>& rhs) const {
			return lhs / Pol(rhs);
		}

		expr_type operator()(const RatFun<Pol>& lhs, const RatFun<Pol>& rhs) const {
			return lhs / rhs;
		}

		template<typename T>
		typename std::enable_if<!std::is_same<Formula<Pol>, T>::value, expr_type>::type
		operator()(const T& lhs, const CoeffType& coeff) const {
			return lhs * reciprocal(coeff);
		}

		template<typename T>
		typename std::enable_if<!std::is_same<Formula<Pol>, T>::value, expr_type>::type
		operator()(const T& lhs, const RatFun<Pol>& rhs) const {
			// TODO: Not extremely efficient probably
			return  RatFun<Pol>(rhs.denominator(), rhs.nominator()) * Pol(lhs);
		}

		template<typename T, typename U>
		typename std::enable_if<!std::is_same<Formula<Pol>, T>::value, expr_type>::type
		operator()(const T& lhs, const U& rhs) const {
			return RatFun<Pol>(Pol(lhs), Pol(rhs));
		}

		template<typename T>
		expr_type operator()(const Formula<Pol>& lhs, const T& rhs) const {
			throw std::runtime_error("No division for formula");
		}

		template<typename T>
		typename std::enable_if<!std::is_same<Formula<Pol>, T>::value, expr_type>::type
		operator()(const T& lhs, const Formula<Pol>& rhs) const {
			throw std::runtime_error("No division for formula");
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

		expr_type operator()(const RatFun<Pol>& lhs) const {
			return RatFun<Pol>(lhs.nominator().pow(expVal), lhs.denominator().pow(expVal));
		}

		expr_type operator()(const CoeffType& lhs) const {
			return carl::pow(lhs, expVal);
		}

		expr_type operator()(const Variable& lhs) const {
			return createMonomial(lhs, expVal);
		}

		expr_type operator()(const Monomial::Arg& lhs) const {
			return lhs->pow(expVal);
		}

		expr_type operator()(const Formula<Pol>& lhs) const {
			throw std::runtime_error("No power for fomula");
		}
	};

	class perform_negate: public boost::static_visitor<expr_type> {
	public:
		template<typename T>
		expr_type operator()(const T& lhs) const {
			return lhs * CoeffType(-1);
		}

		expr_type operator()(const Formula<Pol>& lhs) const {
			throw std::runtime_error("No negate for fomula");
		}
	};

	class print_expr_type: public boost::static_visitor<> {
	public:
		void operator()(const RatFun<Pol>& expr) const {
			std::cout << "Rational function " << expr << std::endl;
		}

		void operator()(const Pol& expr) const {
			std::cout << "Polynomial " << expr << std::endl;
		}

		void operator()(const Term<CoeffType>& expr) const {
			std::cout << "Term " << expr << std::endl;
		}

		void operator()(const Monomial::Arg& expr) const {
			std::cout << "Monomial " << expr << std::endl;
		}

		void operator()(const CoeffType& expr) const {
			std::cout << "Coefficient " << expr << std::endl;
		}

		void operator()(const Variable& expr) const {
			std::cout << "Variable " << expr << std::endl;
		}

		void operator()(const Formula<Pol>& expr) const {
			std::cout << "Formula " << expr << std::endl;
		}
	};

	ExpressionParser(): ExpressionParser<Pol>::base_type(main, "polynomial") {
		/** Tokens */
		operationShift.add("+", ADD)("-", SUB);
		operationScale.add("*", MUL)("/", DIV);
		operationPow.add("^", POW)("**", POW);
		operationSign.add("-", NEG);

		operationNot.add("not", carl::FormulaType::NOT)("!", carl::FormulaType::NOT);
		operationImp.add("imp", carl::FormulaType::IMPLIES)("->", carl::FormulaType::IMPLIES);
		operationIff.add("iff", carl::FormulaType::IFF)("<->", carl::FormulaType::IFF);
		operationAnd.add("and", FormulaType::AND)("&", FormulaType::AND)("or", FormulaType::OR)("|", FormulaType::OR);

		varname = qi::lexeme[ (qi::alpha | qi::char_("_")) >> *(qi::alnum | qi::char_("_"))];
		variable = varname[qi::_val = px::bind(&ExpressionParser<Pol>::newVariable, px::ref(*this), qi::_1)];

		/** Rules */
		// operationScaleLA is a look-ahead hack to prevent the * operator from consuming ** (power)
		operationScaleLA = qi::lexeme[ operationScale >> !qi::lit("*") ][qi::_val = qi::_1];
		monomial = variable[qi::_val = qi::_1];
		boolean = (qi::true_[qi::_val = px::bind(&ExpressionParser<Pol>::makeBool, px::ref(*this), true)] | qi::false_[qi::_val = px::bind(&ExpressionParser<Pol>::makeBool, px::ref(*this), false)]);
		atom = (boolean[qi::_val = qi::_1] | monomial[qi::_val = qi::_1] | coeff[qi::_val = qi::_1]);
		expr = ("(" > expr_sum > ")")[qi::_val = qi::_1] | atom[qi::_val = qi::_1];
		expr_power = (expr >> *(operationPow > exponentVal))[qi::_val = px::bind(&ExpressionParser<Pol>::powExpr, px::ref(*this), qi::_1, qi::_2)];
		expr_sign = (*operationSign > expr_power)[qi::_val = px::bind(&ExpressionParser<Pol>::signExpr, px::ref(*this), qi::_1, qi::_2)];
		expr_product = (expr_sign >> *(operationScaleLA > expr_power))[qi::_val = px::bind(&ExpressionParser<Pol>::arithmeticExpr, px::ref(*this), qi::_1, qi::_2)];
		expr_sum = (expr_product >> *(operationShift > expr_product))[qi::_val = px::bind(&ExpressionParser<Pol>::arithmeticExpr, px::ref(*this), qi::_1, qi::_2)];
		main = expr_sum;

		varname.name("varname");
		variable.name("variable");
		monomial.name("monomial");
		atom.name("atom");
		expr.name("expr");
		expr_sign.name("expr_sign");
		expr_power.name("expr_power");
		expr_product.name("expr_product");
		expr_sum.name("expr_sum");
		main.name("main");
	}

	void addVariable(Variable::Arg v) {
		auto s = VariablePool::getInstance().get_name(v);
		varmap.add(s, v);
	}
	
private:
	enum Operation { ADD, SUB, MUL, DIV, POW, NEG };
	
	Variable newVariable(const std::string& s) {
		Variable* vptr = nullptr;
		if ((vptr = varmap.find(s)) != nullptr) {
			return *vptr;
		}
		Variable v = VariablePool::getInstance().find_variable_with_name(s);
		if (v != Variable::NO_VARIABLE) {
	        varmap.add(s, v);
	        return v;
		}

		v = fresh_real_variable(s);
		varmap.add(s, v);
		return v;
	}

	expr_type makeBool(bool value) {
		if (value) {
			return Formula<Pol>(FormulaType::TRUE);
		} else {
			return Formula<Pol>(FormulaType::FALSE);
		}
	}

	expr_type booleanBinaryExpr(const expr_type& first, const std::vector<boost::fusion::vector2<carl::FormulaType,expr_type>>& ops) {
		expr_type res = first;
		for (const auto& op: ops) {
			switch (boost::fusion::at_c<0>(op)) {
				case carl::FormulaType::AND: {
					res = boost::apply_visitor( perform_addition(), res, boost::fusion::at_c<1>(op) );
					break;
				}
				case carl::FormulaType::OR: {
					res = boost::apply_visitor( perform_subtraction(), res, boost::fusion::at_c<1>(op) );
					break;
				}
				case carl::FormulaType::IMPLIES: {
					res = boost::apply_visitor( perform_subtraction(), res, boost::fusion::at_c<1>(op) );
					break;
				}
				case carl::FormulaType::IFF: {
					res = boost::apply_visitor( perform_subtraction(), res, boost::fusion::at_c<1>(op) );
					break;
				}
			}
		}
		return res;
	}

	expr_type booleanUnaryExpr(const std::vector<carl::FormulaType>& ops, const expr_type& first) {
		expr_type res = first;
		for (const auto& op: ops) {
			switch (op) {
				case carl::FormulaType::NOT: {
					res = boost::apply_visitor( perform_negate(), res );
					break;
				}
			}
		}
		return res;
	}

	expr_type arithmeticExpr(const expr_type& first, const std::vector<boost::fusion::vector2<Operation,expr_type>>& ops) {
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

	expr_type signExpr(const std::vector<Operation>& ops, const expr_type& first) {
		expr_type res = first;
		for(auto op : ops) {
			switch (op) {
				case NEG: {
					res = boost::apply_visitor( perform_negate(), res );
					break;
				}
				default:
					throw std::runtime_error("Unknown unary operator");
			}
		}
		return res;
	}

	qi::symbols<char, carl::FormulaType> operationNot;
	qi::symbols<char, carl::FormulaType> operationImp;
	qi::symbols<char, carl::FormulaType> operationIff;
	qi::symbols<char, carl::FormulaType> operationAnd;
	qi::symbols<char, Operation> operationShift;
	qi::symbols<char, Operation> operationScale;
	qi::symbols<char, Operation> operationPow;
	qi::symbols<char, Operation> operationSign;
	qi::symbols<char, Variable> varmap;

	qi::rule<Iterator, std::string(), Skipper> varname;
	qi::uint_parser<exponent,10,1,-1> exponentVal;
	qi::real_parser<CoeffType,RationalPolicies<CoeffType>> coeff;
	qi::rule<Iterator, Variable(), Skipper> variable;

	qi::rule<Iterator, Operation(), Skipper> operationScaleLA;

	qi::rule<Iterator, expr_type(), Skipper> boolean;
	qi::rule<Iterator, expr_type(), Skipper> monomial;
	qi::rule<Iterator, expr_type(), Skipper> atom;

	qi::rule<Iterator, expr_type(), Skipper> expr;
	qi::rule<Iterator, expr_type(), Skipper> expr_sign;
	qi::rule<Iterator, expr_type(), Skipper> expr_product;
	qi::rule<Iterator, expr_type(), Skipper> expr_power;
	qi::rule<Iterator, expr_type(), Skipper> expr_sum;
	qi::rule<Iterator, expr_type(), Skipper> main;
};

}
}
