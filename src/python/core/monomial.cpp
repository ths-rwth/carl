/*
 * monomial.cpp
 *
 *  Created on: 16 Apr 2016
 *      Author: harold
 */

#include "monomial.h"

#include "common.h"

void define_monomial(py::module& m) {
    py::class_<carl::Monomial::Arg>(m, "Monomial")
        .def("__init__", [] (carl::Monomial::Arg& arg, carl::Variable v, carl::exponent e) -> void {
            auto m = carl::MonomialPool::getInstance().create(v, e);
            arg = m;
        })
        .def("tdeg", &carl::Monomial::tdeg)
        .def("__str__", &streamToString<carl::Monomial::Arg>)
        .def(py::self * py::self)
        //.def(py::self * carl::Variable::Arg())
        ;
}
