#include <Python.h>
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pybind11/operators.h"

#include "definitions.h"
#include "parser.h"

#include <boost/variant/get.hpp>

namespace py = pybind11;

template<typename Expression>
class to_expr: public boost::static_visitor<Expression> {
public:
	template<typename T>
	Expression operator()(const T& expr) const {
		return Expression(expr);
	}
};

template<>
class to_expr<Polynomial>: public boost::static_visitor<Polynomial> {
public:
	Polynomial operator()(const Polynomial& p) const {
		return p;
	}

	Polynomial operator()(const RationalFunction& p) const {
		throw std::runtime_error("Cannot make polynomial out of rational function");
	}

	template<typename T>
	Polynomial operator()(const T& expr) const {
		return Polynomial(expr);
	}
};

template<>
class to_expr<RationalFunction>: public boost::static_visitor<RationalFunction> {
public:
	RationalFunction operator()(const RationalFunction& expr) const {
		return expr;
	}

	RationalFunction operator()(const Polynomial& expr) const {
		return RationalFunction(expr);
	}

	template<typename T>
	RationalFunction operator()(const T& expr) const {
		return RationalFunction(Polynomial(expr));
	}
};

class to_object: public boost::static_visitor<py::object> {
public:
	template<typename T>
	py::object operator()(const T& expr) const {
		py::object res = py::cast(expr, py::return_value_policy::take_ownership);
		if (!res) {
			// return void
			res = py::cast(nullptr);
		}
		return res;
	}
};

py::handle parseString(const std::string& input) {
	expr_type data = parse(input);

	py::object obj = boost::apply_visitor( to_object(), data );
	obj.inc_ref();
	return obj.release();
}

namespace pybind11 {
namespace detail {
/**
 * Dummy type caster for handle, so function can return pybind11 handles directly
 */
template <> class type_caster<handle> {
public:
	bool load(handle src, bool) {
		std::cout << "LOAD\n";
		value = handle(src).inc_ref();
		return true;
	}

	static handle cast(handle src, return_value_policy policy, handle parent) {
		switch(policy) {
			case return_value_policy::automatic:
			case return_value_policy::copy:
			case return_value_policy::take_ownership:
				return handle(src);
			case return_value_policy::reference:
				return handle(src).inc_ref();
			case return_value_policy::reference_internal:
				parent.inc_ref();
				return handle(src);
		}
		return handle(src);
	}
	PYBIND11_TYPE_CASTER(handle, _("handle"));
};

}
}

/**
 * The actual module definition
 */
PYBIND11_PLUGIN(_parsing) {
	py::module m("parsing", "pycarl parsing functions");

	//TODO: hardcoded module path, this is fragile
	// Import core as the result of parse depends on it
	py::module::import("pycarl.core._core");
	py::module::import("pycarl.numbers._numbers");
/*
	py::class_<carl::parser::Parser<Polynomial>>(m, "Parser",
	py::doc("Parser for polynomials and rational functions"))
		.def("polynomial", &carl::parser::Parser<Polynomial>::polynomial)
		.def("rational_function", &carl::parser::Parser<Polynomial>::rationalFunction)
		.def("add_variable", &carl::parser::Parser<Polynomial>::addVariable)
		;

	py::class_<carl::parser::ExpressionParser<Polynomial>>(m, "Parser",
	py::doc("Parser for polynomials and rational functions"))
		.def("add_variable", &carl::parser::ExpressionParser<Polynomial>::addVariable)
		;
*/

	m.def("parse", &parseString);

	//m.attr("ExpressionParser") = py::cast(ParserType(), py::return_value_policy::take_ownership);

	return m.ptr();
}
