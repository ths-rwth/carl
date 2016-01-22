#include <Python.h>
#include <boost/python.hpp>

#include "definitions.h"
#include "helpers.h"
#include "carl/numbers/numbers.h"


// Thin wrappers for rationals
double ratToDouble(const Rational& r) { return carl::toDouble(r); }
std::string ratToString(const Rational& r ) { return carl::toString(r, true); }

BOOST_PYTHON_MODULE(_numbers) {

    using namespace boost::python;
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
    def("rationalize", static_cast<Rational (*)(double)>(&carl::rationalize));
}