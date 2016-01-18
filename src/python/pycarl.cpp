//#include <python3.4m/Python.h>
//#include <python2.7/Python.h>
#include <Python.h>
#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>

#include "helpers.h"

#include "carl/core/Variable.h"
#include "carl/core/Monomial.h"
#include "carl/core/MultivariatePolynomial.h"
#include "carl/core/RationalFunction.h"
#include "carl/util/parser/Parser.h"

#include <gmpxx.h>
typedef mpq_class Rational;

//typedef double Rational;
typedef carl::MultivariatePolynomial<Rational> Polynomial;
typedef carl::RationalFunction<Polynomial> RationalFunction;


/**
 * Following are some helper functions to provide some glue between Python and carl
 */
// Convert Rational to std::string
std::string rational_str(const Rational& rat) {
	return rat.get_str();
}

Polynomial parsePolynomial(const std::string& text) {
	carl::parser::Parser<Polynomial> p;
	return p.polynomial(text);
}

RationalFunction parseRationalFunction(const std::string& text) {
	carl::parser::Parser<Polynomial> p;
	return p.rationalFunction(text);
}

/**
 * The actual module definition
 */
BOOST_PYTHON_MODULE(_core)
{
	using namespace boost::python;

	// Register interable conversions.
	iterable_converter()
		.from_python<std::vector<double> >()
		.from_python<std::list<double> >().from_python<std::vector<std::string> >()
		.from_python<std::list<std::string> >()
		;
	dict_converter()
		.from_python<std::map<carl::Variable, Rational> >()
		;

	map_to_dict()
		.to_python<std::map<std::string, carl::Variable> >()
		;
	iter_to_tuple()
		.to_python<std::set<carl::Variable> >()
		;

	class_<Rational>("Rational", init<double>())
		.def("__float__", &Rational::get_d)
		.def("__str__", &rational_str)
		;
	rational_converter();
	//boost::python::implicitly_convertible<Rational,double>();
	//boost::python::implicitly_convertible<double,Rational>();

	enum_<carl::VariableType>("VariableType")
		.value("BOOL", carl::VariableType::VT_BOOL)
		.value("INT", carl::VariableType::VT_INT)
		.value("REAL", carl::VariableType::VT_REAL);

	class_<carl::Variable>("Variable", no_init)
		.def("__mul__", static_cast<carl::Monomial::Arg (*)(carl::Variable::Arg, carl::Variable::Arg)>(&carl::operator*))
		.def("__mul__", static_cast<carl::Monomial::Arg (*)(carl::Variable::Arg, const carl::Monomial::Arg&)>(&carl::operator*))
		.def(self_ns::str(self_ns::self))
		;
/*
	class_<carl::Monomial, carl::Monomial::Arg, boost::noncopyable>("Monomial", no_init)
		.def("tdeg", &carl::Monomial::tdeg)
		.def(self_ns::str(self_ns::self))
		.def("__mul__", static_cast<carl::Monomial::Arg (*)(const carl::Monomial::Arg&, const carl::Monomial::Arg&)>(&carl::operator*))
		.def("__mul__", static_cast<carl::Monomial::Arg (*)(const carl::Monomial::Arg&, carl::Variable::Arg)>(&carl::operator*))
		;
*/
	class_<carl::Term<Rational>>("Term")
		.def(init<Rational>())
		.def(init<carl::Variable::Arg>())
		.def(init<const carl::Monomial::Arg&>())
		.def(init<Rational, const carl::Monomial::Arg&>())
		.def(init<Rational, carl::Variable::Arg, carl::exponent>())
		.def(self_ns::str(self_ns::self))
		;

	class_<Polynomial>("Polynomial", init<Rational>())
		.def(init<carl::Variable::Arg>())
		.def(init<const carl::Monomial::Arg&>())
		.def("evaluate", &Polynomial::evaluate<Rational>)
		.def("gatherVariables", static_cast<std::set<carl::Variable> (Polynomial::*)() const>(&Polynomial::gatherVariables))
		.def(self_ns::str(self_ns::self))
		;

	class_<RationalFunction>("RationalFunction", init<Polynomial, Polynomial>())
		.def("evaluate", &RationalFunction::evaluate)
		.def("gatherVariables", static_cast<std::set<carl::Variable> (RationalFunction::*)() const>(&RationalFunction::gatherVariables))
		.add_property("numerator", &RationalFunction::nominator)
		.add_property("denominator", &RationalFunction::denominator)
		.def(self_ns::str(self_ns::self))
		;

	class_<carl::parser::Parser<Polynomial>, boost::noncopyable>("Parser")
		.def("polynomial", &carl::parser::Parser<Polynomial>::polynomial)
		.def("rationalFunction", &carl::parser::Parser<Polynomial>::rationalFunction)
		.def("addVariable", &carl::parser::Parser<Polynomial>::addVariable)
		;

	// Global string parser functions (no variable management)
	def("parsePolynomial", &parsePolynomial);
	def("parseRationalFunction", &parseRationalFunction);

	// Non-constructable class VariablePool, static instance accessible via global
	class_<carl::VariablePool, boost::noncopyable>("VariablePoolInst", no_init)
		.def("getFreshVariable", static_cast<carl::Variable (carl::VariablePool::*)(carl::VariableType)>(&carl::VariablePool::getFreshVariable))
		.def("getFreshVariable", static_cast<carl::Variable (carl::VariablePool::*)(const std::string&, carl::VariableType)>(&carl::VariablePool::getFreshVariable))
		;

	// Non-constructable class MonomialPool, static instance accessible via global
	class_<carl::MonomialPool, boost::noncopyable>("MonomialPoolInst", no_init)
		.def("create", static_cast<carl::Monomial::Arg (carl::MonomialPool::*)(carl::Variable::Arg, carl::exponent)>(&carl::MonomialPool::create))
		;

	// Setup the global variables
	scope().attr("VariablePool") = object(ptr(&carl::VariablePool::getInstance()));
	scope().attr("MonomialPool") = object(ptr(&carl::MonomialPool::getInstance()));
}
