/*
 * term.cpp
 *
 *  Created on: 16 Apr 2016
 *      Author: harold
 */

#include "Interval.h"

#include "common.h"


void define_boundtype(py::module& m) {
    py::enum_<carl::BoundType>(m, "BoundType")
        .value("STRICT", carl::BoundType::STRICT)
        .value("WEAK", carl::BoundType::WEAK)
        .value("INFTY", carl::BoundType::INFTY);
}

void define_interval(py::module& m) {
    py::class_<Interval>(m, "Interval")
        .def(py::init<Rational, Rational>())
        .def("__str__", &streamToString<Interval>)
        ;
}
