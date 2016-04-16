/*
 * constraint.cpp
 *
 *  Created on: 16 Apr 2016
 *      Author: harold
 */

#include "constraint.h"

#include "common.h"

void define_relation(py::module& m) {
    py::enum_<carl::Relation>(m, "Relation")
        .value("EQ", carl::Relation::EQ)
        .value("NEQ", carl::Relation::NEQ)
        .value("LESS", carl::Relation::LESS)
        .value("LEQ", carl::Relation::LEQ)
        .value("GREATER", carl::Relation::GREATER)
        .value("GEQ", carl::Relation::GEQ)
        ;
}

void define_constraint(py::module& m) {
    py::class_<Constraint>(m, "Constraint")
        .def(py::init<bool>())
        .def(py::init<carl::Variable, carl::Relation, Rational>(), py::arg("var"), py::arg("rel"), py::arg("bound") = Rational(0))
        .def(py::init<Polynomial, carl::Relation>())
        .def("__str__", &streamToString<Constraint>)
        ;
}
