/*
 * variable.cpp
 *
 *  Created on: 16 Apr 2016
 *      Author: harold
 */

#include "variable.h"

#include "common.h"

#include "carl/core/VariablePool.h"

carl::Variable getOrCreateVariable(std::string const & name, carl::VariableType type) {
    // Variables are constructed by the Pool. Note that for a given name,
    //two Variable instances may differ, but refer to the same id (data)
    auto& pool = carl::VariablePool::getInstance();
    carl::Variable res = pool.findVariableWithName(name);
    if (res != carl::Variable::NO_VARIABLE) {
        return res;
    }
    return pool.getFreshVariable(name, type);
}

void define_variabletype(py::module& m) {
    py::enum_<carl::VariableType>(m, "VariableType")
        .value("BOOL", carl::VariableType::VT_BOOL)
        .value("INT", carl::VariableType::VT_INT)
        .value("REAL", carl::VariableType::VT_REAL);
}

void define_variable(py::module& m) {
    py::class_<carl::Variable>(m, "Variable")
        .def("__init__",
            [](carl::Variable &instance, std::string name, carl::VariableType type) -> void {
                carl::Variable tmp = getOrCreateVariable(name, type);
                new (&instance) carl::Variable(tmp);
            }
        , py::arg("name"), py::arg("type") = carl::VariableType::VT_REAL)
        .def("__init__",
            [](carl::Variable &instance, carl::VariableType type) -> void {
                carl::Variable tmp = carl::VariablePool::getInstance().getFreshVariable(type);
                new (&instance) carl::Variable(tmp);
            }
        , py::arg("type") = carl::VariableType::VT_REAL)

        .def("__add__",  static_cast<Polynomial (*)(carl::Variable::Arg, const Polynomial&)>(&carl::operator+))
        .def("__add__",  static_cast<Polynomial (*)(carl::Variable::Arg, const Term&)>(&carl::operator+))
        .def("__add__",  static_cast<Polynomial (*)(carl::Variable::Arg, const Monomial&)>(&carl::operator+))
        .def("__add__",  static_cast<Polynomial (*)(carl::Variable::Arg, carl::Variable::Arg)>(&carl::operator+))
        .def("__add__",  static_cast<Polynomial (*)(carl::Variable::Arg, const Rational&)>(&carl::operator+))

        .def("__add__",  static_cast<Polynomial (*)(carl::Variable::Arg, const Polynomial&)>(&carl::operator-))
        .def("__sub__",  static_cast<Polynomial (*)(carl::Variable::Arg, const Term&)>(&carl::operator-))
        .def("__sub__",  static_cast<Polynomial (*)(carl::Variable::Arg, const Monomial&)>(&carl::operator-))
        .def("__sub__",  static_cast<Polynomial (*)(carl::Variable::Arg, carl::Variable::Arg)>(&carl::operator-))
        .def("__add__",  static_cast<Polynomial (*)(carl::Variable::Arg, const Rational&)>(&carl::operator-))

        .def("__mul__",  static_cast<Polynomial (*)(carl::Variable::Arg, const Polynomial&)>(&carl::operator*))
        .def("__mul__",  static_cast<Term (*)(carl::Variable::Arg, const Term&)>(&carl::operator*))
        .def("__mul__",  static_cast<Monomial (*)(carl::Variable::Arg, const Monomial&)>(&carl::operator*))
        .def("__mul__",  static_cast<Monomial (*)(carl::Variable::Arg, carl::Variable::Arg)>(&carl::operator*))
        .def("__mul__",  static_cast<Term (*)(carl::Variable::Arg, const Rational&)>(&carl::operator*))

        .def(py::self / Rational())

        .def("__pow__", [](carl::Variable::Arg var, carl::uint exp) -> Monomial {return carl::Monomial(var).pow(exp);})

        .def("__pos__", [](carl::Variable::Arg var) -> carl::Variable {return carl::Variable(var);})
        .def("__neg__", [](carl::Variable::Arg var) -> Term {return var * Rational(-1);})

        .def(py::self == py::self)
        .def(py::self != py::self)
        .def(py::self < py::self)
        .def(py::self <= py::self)
        .def(py::self > py::self)
        .def(py::self >= py::self)

        .def_property_readonly("name", &carl::Variable::getName)
        .def_property_readonly("type", &carl::Variable::getType)
        .def_property_readonly("id", &carl::Variable::getId)
        .def_property_readonly("rank", &carl::Variable::getRank)
        .def("__str__", &streamToString<carl::Variable>)
        ;
}
