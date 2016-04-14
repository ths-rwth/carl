#include <Python.h>
#include "pybind11/pybind11.h"

#include "definitions.h"
#include "helpers.h"
#include "carl/numbers/numbers.h"


// Thin wrappers for rationals
double ratToDouble(const Rational& r) { return carl::toDouble(r); }
std::string ratToString(const Rational& r ) { return carl::toString(r, true); }

namespace py = pybind11;

PYBIND11_PLUGIN(_numbers) {
        py::module m("Numbers", "Number plugin for pycarl");

        class_<Rational, Rational*>("Rational", "Class wrapping rational numbers")
        .def(init<int>())
        .def("__float__", &ratToDouble)
        .def("__str__", &ratToString)
        .def(self + self)
        .def(self - self)
        .def(self * self)
        .def(self / self)
        .def(self == self)
        .def(self != self)
        .def(-self)
        .def(self < self)
        .def(self > self)
        .def(self >= self)
        .def(self <= self)
        .def(self > int())
        .def(self < int())
        .def(self == int())
        .def(self != int())
        .def(self >= int())
        .def(self <= int())
        ;
        def("rationalize", static_cast<Rational (*)(double)>(&carl::rationalize), "Construct a rational number from a double");

        return m.ptr();
}


}
