#include "mod_parse.h"

#include "mod_core.h"
#include "mod_func.h"

#include "parser.h"

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pybind11/operators.h"

#include <boost/variant/get.hpp>
#include <boost/variant/static_visitor.hpp>
#include <python/mod_core.h>

namespace py = pybind11;

class to_object: public boost::static_visitor<py::object> {
public:
	template<typename T>
	py::object operator()(const T& expr) const {
		// Expr is on the stack, make a copy
		py::object res = py::cast(expr, py::return_value_policy::copy);
		if (!res) {
			// return void
			res = py::object(Py_None, true);
		}
		return res;
	}
};

py::handle parseString(const std::string& input) {
	carl::parser::ExpressionType<Polynomial> data = parse(input);

	py::object obj = boost::apply_visitor( to_object(), data );
	auto handl = obj.release();
	return handl;
}

namespace pybind11 {
namespace detail {
/**
 * Dummy type caster for handle, so function can return pybind11 handles directly
 */
template <> class type_caster<handle> {
public:
	bool load(handle src, bool) {
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
PYBIND11_PLUGIN(parse) {
    py::module m("parse", "pycarl parsing functions");

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
