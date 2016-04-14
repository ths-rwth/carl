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
        py::module m("numbers", "Number plugin for pycarl");

        py::class_<Rational, Rational*>("Rational", "Class wrapping rational numbers")
        .def(py::init<int>())
        .def("__float__", &ratToDouble)
        .def("__str__", &ratToString)
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self * py::self)
        .def(py::self / py::self)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def(-py::self)
        .def(py::self < py::self)
        .def(py::self > py::self)
        .def(py::self >= py::self)
        .def(py::self <= py::self)
        .def(py::self > int())
        .def(py::self < int())
        .def(py::self == int())
        .def(py::self != int())
        .def(py::self >= int())
        .def(py::self <= int())
        ;

        m.def("rationalize", static_cast<Rational (*)(double)>(&carl::rationalize), "Construct a rational number from a double");

        return m.ptr();
}


}
