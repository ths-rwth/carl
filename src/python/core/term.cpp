/*
 * term.cpp
 *
 *  Created on: 16 Apr 2016
 *      Author: harold
 */

#include "term.h"

#include "common.h"

void define_term(py::module& m) {
    py::class_<Term>(m, "Term")
        .def(py::init<Rational>())
        .def(py::init<carl::Variable::Arg>())
        .def(py::init<const carl::Monomial::Arg&>())
        .def(py::init<Rational, const carl::Monomial::Arg&>())
        .def(py::init<Rational, carl::Variable::Arg, carl::exponent>())
        .def("__str__", &streamToString<Term>)
        ;
}
