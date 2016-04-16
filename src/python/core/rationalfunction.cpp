/*
 * rationalfunction.cpp
 *
 *  Created on: 16 Apr 2016
 *      Author: harold
 */

#include "rationalfunction.h"

#include "common.h"

void define_rationalfunction(py::module& m) {
    py::class_<RationalFunction>(m, "RationalFunction", py::doc("Represent a rational function, that is the fraction of two multivariate polynomials "))
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
}
