/*
 * rational.cpp
 *
 *  Created on: 16 Apr 2016
 *      Author: harold
 */

#include "rational.h"

#include "common.h"

#include "carl/numbers/numbers.h"

void define_rational(py::module& m) {
    py::class_<Rational>(m, "Rational", "Class wrapping rational numbers")
        .def("__init__", [](Rational &instance, double val) -> void { auto tmp = carl::rationalize<Rational>(val); new (&instance) Rational(tmp); })
        .def("__init__", [](Rational &instance, int val) -> void { auto tmp = carl::rationalize<Rational>(val); new (&instance) Rational(tmp); })
        .def("__init__", [](Rational &instance,std::string val) -> void { auto tmp = carl::rationalize<Rational>(val); new (&instance) Rational(tmp); })

        .def("__add__",  static_cast<Polynomial (*)(const Rational&, const Polynomial&)>(&carl::operator+))
        .def("__add__",  static_cast<Polynomial (*)(const Rational&, const Term&)>(&carl::operator+))
        .def("__add__",  static_cast<Polynomial (*)(const Rational&, const Monomial&)>(&carl::operator+))
        .def("__add__",  static_cast<Polynomial (*)(const Rational&, carl::Variable::Arg)>(&carl::operator+))
        .def(py::self + py::self)

        .def("__sub__",  static_cast<Polynomial (*)(const Rational&, const Polynomial&)>(&carl::operator-))
        .def("__sub__",  static_cast<Polynomial (*)(const Rational&, const Term&)>(&carl::operator-))
        .def("__sub__",  static_cast<Polynomial (*)(const Rational&, const Monomial&)>(&carl::operator-))
        .def("__sub__",  static_cast<Polynomial (*)(const Rational&, carl::Variable::Arg)>(&carl::operator-))
        .def(py::self - py::self)

        .def("__mul__",  static_cast<Polynomial (*)(const Rational&, const Polynomial&)>(&carl::operator*))
        .def("__mul__",  static_cast<Term (*)(const Rational&, const Term&)>(&carl::operator*))
        .def("__mul__",  static_cast<Term (*)(const Rational&, const Monomial&)>(&carl::operator*))
        .def("__mul__",  static_cast<Term (*)(const Rational&, carl::Variable::Arg)>(&carl::operator*))
        .def(py::self * py::self)

        .def(PY_DIV, [](const Rational& lhs, const RationalFunction& rhs) { return RationalFunction(lhs) / rhs; })
        .def(PY_DIV, [](const Rational& lhs, const Polynomial& rhs) { return RationalFunction(lhs) / rhs; })
        .def(PY_DIV, [](const Rational& lhs, const Term& rhs) { return RationalFunction(lhs) / rhs; })
        .def(PY_DIV, [](const Rational& lhs, const Monomial& rhs) { return RationalFunction(lhs) / rhs; })
        .def(PY_DIV, [](const Rational& lhs, carl::Variable::Arg rhs) { return RationalFunction(lhs) / rhs; })
        .def(py::self / py::self)

        .def("__pow__", static_cast<Rational (*)(const Rational&, std::size_t)>(&carl::pow))
        .def("__pos__", [](const Rational& var) {return Rational(var);})
        .def(-py::self)

        .def(py::self == py::self)
        .def(py::self != py::self)
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
        .def("__eq__", [](const Rational& lhs, double rhs) { return lhs == carl::rationalize<Rational>(rhs); })
        .def("__neq__", [](const Rational& lhs, double rhs) { return lhs != carl::rationalize<Rational>(rhs); })
        .def("__gt__", [](const Rational& lhs, double rhs) { return lhs > carl::rationalize<Rational>(rhs); })
        .def("__ge__", [](const Rational& lhs, double rhs) { return lhs >= carl::rationalize<Rational>(rhs); })
        .def("__lt__", [](const Rational& lhs, double rhs) { return lhs < carl::rationalize<Rational>(rhs); })
        .def("__le__", [](const Rational& lhs, double rhs) { return lhs <= carl::rationalize<Rational>(rhs); })

        .def("__int__",  [](const Rational& val) -> int {
            return carl::toInt<carl::sint>(carl::getNum(val)) /
                carl::toInt<carl::sint>(carl::getDenom(val));
        })
        .def("__float__", static_cast<double (*)(Rational const&)>(&carl::toDouble))
        .def("__str__", [](Rational const& r) {return carl::toString(r, true);})

        .def_property_readonly("numerator", [](const Rational& val) -> int {
            return carl::toInt<carl::sint>(carl::getNum(val));
        })
        .def_property_readonly("denominator", [](const Rational& val) -> int {
            return carl::toInt<carl::sint>(carl::getDenom(val));
        })
        ;
}
