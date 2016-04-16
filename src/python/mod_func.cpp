#include "mod_func.h"

#include "common.h"
#include "helpers.h"

carl::Variable getOrCreateVariable(std::string const & name, carl::VariableType type) {
	// Variables are constructed by the Pool. Note that for a given name,
	//two Variable instances may differ, but refer to the same id (data)
	auto& pool = carl::VariablePool::getInstance();
	carl::Variable res = pool.findVariableWithName(name);
	if (res != carl::Variable::NO_VARIABLE) {
		return res;
	}
	return pool.getFreshVariable(name, type);
}

// Be warned: Enabling something like this will break everything about Monomial,
// as to Python the shared_ptr (Arg) IS the Monomial
//  //PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);

/**
 * The actual module definition
 */
PYBIND11_PLUGIN(func) {
    py::module m("func", "pycarl module for functions");

    py::class_<carl::Monomial::Arg>(m, "Monomial")
        .def("__init__", [] (carl::Monomial::Arg& arg, carl::Variable v, carl::exponent e) -> void {
            auto m = carl::MonomialPool::getInstance().create(v, e);
            arg = m;
        })
        .def("tdeg", &carl::Monomial::tdeg)
        .def("__str__", &streamToString<carl::Monomial::Arg>)
        .def("__mul__", static_cast<carl::Monomial::Arg (*)(const carl::Monomial::Arg&, const carl::Monomial::Arg&)>(&carl::operator*))
        .def("__mul__", static_cast<carl::Monomial::Arg (*)(const carl::Monomial::Arg&, carl::Variable::Arg)>(&carl::operator*))
        ;

    py::class_<Term>(m, "Term")
        .def(py::init<Rational>())
        .def(py::init<carl::Variable::Arg>())
        .def(py::init<const carl::Monomial::Arg&>())
        .def(py::init<Rational, const carl::Monomial::Arg&>())
        .def(py::init<Rational, carl::Variable::Arg, carl::exponent>())
        .def("__str__", &streamToString<Term>)
        ;

    py::class_<Polynomial>(m, "Polynomial",
    py::doc("Represent a multivariate polynomial"))
        .def(py::init<carl::Variable::Arg>())
        .def(py::init<const carl::Monomial::Arg&>())
        .def(py::init<Rational>())
        .def("constant_part", &Polynomial::constantPart, py::return_value_policy::reference_internal)
        .def("evaluate", &Polynomial::evaluate<Rational>)
        .def("gather_variables", static_cast<std::set<carl::Variable> (Polynomial::*)() const>(&Polynomial::gatherVariables))
        .def_property_readonly("total_degree", &Polynomial::totalDegree)//, py::doc("The maximum degree of the terms in the polynomial"))
        .def("degree", &Polynomial::degree)//, py::doc("Computes the degree with respect to the given variable"))
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

    // Commented out: Use the Monomial constructors instead
    /*
    // Non-constructable class MonomialPool, static instance accessible via global
    py::class_<carl::MonomialPool>(m, "MonomialPoolInst")
        .def("create", static_cast<carl::Monomial::Arg (carl::MonomialPool::*)(carl::Variable::Arg, carl::exponent)>(&carl::MonomialPool::create))
        ;

    // Setup the global variables
    m.attr("MonomialPool") = py::cast(carl::MonomialPool::getInstance(), py::return_value_policy::reference);
    */

    return m.ptr();
}
