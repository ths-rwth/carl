#include <Python.h>
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pybind11/operators.h"

#include "definitions.h"

namespace py = pybind11;

template<typename T>
/**
 * Helper function to get a string out of the stream operator.
 * Used for __str__ functions.
 */
std::string streamToString(T const & t) {
	std::stringstream ss;
	ss << t;
	return ss.str();
}

/**
 * The actual module definition
 */
PYBIND11_PLUGIN(_core) {
	py::module m("core", "pycarl core");

	py::enum_<carl::VariableType>(m, "VariableType")
		.value("BOOL", carl::VariableType::VT_BOOL)
		.value("INT", carl::VariableType::VT_INT)
		.value("REAL", carl::VariableType::VT_REAL);

	py::class_<carl::Variable>(m, "Variable")
		.def("__mul__", static_cast<carl::Monomial::Arg (*)(carl::Variable::Arg, carl::Variable::Arg)>(&carl::operator*))
		.def("__mul__", static_cast<carl::Monomial::Arg (*)(carl::Variable::Arg, const carl::Monomial::Arg&)>(&carl::operator*))
		//.def("__add__", static_cast<Polynomial (*)(carl::Variable::Arg, Rational const&)>(&carl::operator+))
		//.def("__add__", static_cast<Polynomial (*)(carl::Variable::Arg, carl::Variable::Arg)>(&carl::operator+))
		//.def("__sub__", static_cast<Polynomial (*)(carl::Variable::Arg, Rational const&)>(&carl::operator-))
		//.def("__sub__", static_cast<Polynomial (*)(carl::Variable::Arg, carl::Variable::Arg)>(&carl::operator-))
		.def_property_readonly("name", &carl::Variable::getName)
		.def_property_readonly("type", &carl::Variable::getType)
		.def("__str__", &streamToString<carl::Variable>)
		;
/*
	class_<carl::Monomial, carl::Monomial::Arg, boost::noncopyable>("Monomial", no_init)
		.def("tdeg", &carl::Monomial::tdeg)
		.def(self_ns::str(self_ns::self))
		.def("__mul__", static_cast<carl::Monomial::Arg (*)(const carl::Monomial::Arg&, const carl::Monomial::Arg&)>(&carl::operator*))
		.def("__mul__", static_cast<carl::Monomial::Arg (*)(const carl::Monomial::Arg&, carl::Variable::Arg)>(&carl::operator*))
		;
*/
	py::class_<carl::Term<Rational>>(m, "Term")
		.def(py::init<Rational>())
		.def(py::init<carl::Variable::Arg>())
		.def(py::init<const carl::Monomial::Arg&>())
		.def(py::init<Rational, const carl::Monomial::Arg&>())
		.def(py::init<Rational, carl::Variable::Arg, carl::exponent>())
		.def("__str__", &streamToString<carl::Term<Rational>>)
		;

	py::class_<Polynomial>(m, "Polynomial",
	py::doc("Represent a multivariate polynomial"))
		.def(py::init<carl::Variable::Arg>())
		.def(py::init<const carl::Monomial::Arg&>())
		.def(py::init<Rational>())
		.def("constant_part", &Polynomial::constantPart, py::return_value_policy::reference_internal)
		.def("evaluate", &Polynomial::evaluate<Rational>)
		.def("gather_variables", static_cast<std::set<carl::Variable> (Polynomial::*)() const>(&Polynomial::gatherVariables))
		.def_property_readonly("total_degree", &Polynomial::totalDegree, py::doc("The maximum degree of the terms in the polynomial"))
		.def("degree", &Polynomial::degree, py::doc("Computes the degree with respect to the given variable"))
		.def_property_readonly("nr_terms", &Polynomial::nrTerms)
		.def("__str__", &streamToString<Polynomial>)
		.def(py::self - py::self)
		.def(py::self + py::self)
		.def(py::self * py::self)
		.def(py::self != py::self)
		.def(py::self == py::self)
		.def("__add__", [](Polynomial const & left, carl::Variable const& right) {return left + right;})
		.def("__sub__", [](Polynomial const & left, carl::Variable const& right) {return left - right;})
		;

	py::class_<carl::Cache<FactorizationPair>, std::shared_ptr<carl::Cache<FactorizationPair>>>(m, "FactorizationCache",
	py::doc("Cache storing all factorized polynomials"))
		;

	py::class_<FactorizedPolynomial>(m, "FactorizedPolynomial",
	py::doc("Represent a polynomial with its factorization"))
		.def(py::init<const Rational&>())
		.def(py::init<const Polynomial&, const std::shared_ptr<carl::Cache<FactorizationPair>>>())
		.def("constant_part", &FactorizedPolynomial::constantPart)
		.def("evaluate", &FactorizedPolynomial::evaluate<Rational>)
		.def("gather_variables", static_cast<std::set<carl::Variable> (FactorizedPolynomial::*)() const>(&FactorizedPolynomial::gatherVariables))
		.def("__str__", &streamToString<FactorizedPolynomial>)
		.def(py::self - py::self)
		.def(py::self + py::self)
		.def(py::self * py::self)
		.def(py::self == py::self)
		.def(py::self != py::self)
		;

	py::class_<RationalFunction>(m, "RationalFunction",
	py::doc("Represent a rational function, that is the fraction of two multivariate polynomials "))
		.def(py::init<Polynomial, Polynomial>())
		.def("evaluate", &RationalFunction::evaluate)
		.def("gather_variables", static_cast<std::set<carl::Variable> (RationalFunction::*)() const>(&RationalFunction::gatherVariables))
		.def_property_readonly("numerator", &RationalFunction::nominator)
		.def_property_readonly("denominator", &RationalFunction::denominator)
		.def("__str__", &streamToString<RationalFunction>)
		.def(py::self - py::self)
		.def(py::self + py::self)
		.def(py::self * py::self)
		.def(py::self / py::self)
		.def(py::self == py::self)
		.def(py::self != py::self)
		;

	py::class_<FactorizedRationalFunction>(m, "FactorizedRationalFunction",
	py::doc("Represent a rational function, that is the fraction of two factorized polynomials "))
		.def(py::init<FactorizedPolynomial, FactorizedPolynomial>())
		.def("evaluate", &FactorizedRationalFunction::evaluate)
		.def("gather_variables", static_cast<std::set<carl::Variable> (FactorizedRationalFunction::*)() const>(&FactorizedRationalFunction::gatherVariables))
		.def_property_readonly("numerator", &FactorizedRationalFunction::nominator)
		.def_property_readonly("denominator", &FactorizedRationalFunction::denominator)
		.def("__str__", &streamToString<FactorizedRationalFunction>)
		.def(py::self - py::self)
		.def(py::self + py::self)
		.def(py::self * py::self)
		.def(py::self / py::self)
		.def(py::self == py::self)
		.def(py::self != py::self)
		;

	// Non-constructable class VariablePool, static instance accessible via global
	py::class_<carl::VariablePool>(m, "VariablePoolInst")
		.def("get_fresh_variable", static_cast<carl::Variable (carl::VariablePool::*)(carl::VariableType)>(&carl::VariablePool::getFreshVariable))
		.def("get_fresh_variable", static_cast<carl::Variable (carl::VariablePool::*)(const std::string&, carl::VariableType)>(&carl::VariablePool::getFreshVariable))
		.def("find_variable_with_name", &carl::VariablePool::findVariableWithName)
		;

	// Non-constructable class MonomialPool, static instance accessible via global
	py::class_<carl::MonomialPool>(m, "MonomialPoolInst")
		.def("create", static_cast<carl::Monomial::Arg (carl::MonomialPool::*)(carl::Variable::Arg, carl::exponent)>(&carl::MonomialPool::create))
		;

	// Setup the global variables
	m.attr("VariablePool") = py::cast(carl::VariablePool::getInstance(), py::return_value_policy::reference);
	m.attr("MonomialPool") = py::cast(carl::MonomialPool::getInstance(), py::return_value_policy::reference);

	return m.ptr();
}
