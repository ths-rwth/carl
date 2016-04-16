/*
 * polynomial.cpp
 *
 *  Created on: 16 Apr 2016
 *      Author: harold
 */

#include "polynomial.h"

#include "common.h"

void define_polynomial(py::module& m) {
    py::class_<Polynomial>(m, "Polynomial", py::doc("Represent a multivariate polynomial"))
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
}
