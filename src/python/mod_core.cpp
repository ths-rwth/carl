#include "mod_core.h"
#include "mod_func.h"

#include "common.h"
#include "helpers.h"

#include "carl/core/Monomial.h"
#include "carl/core/VariablePool.h"
#include "carl/numbers/numbers.h"

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

PYBIND11_PLUGIN(core) {
    py::module m("core");

    py::class_<Rational, Rational*>(m, "Rational", py::doc("Class wrapping rational numbers"))
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
    .def(py::self + py::self)

    .def("__mul__",  static_cast<Polynomial (*)(const Rational&, const Polynomial&)>(&carl::operator*))
    .def("__mul__",  static_cast<Term (*)(const Rational&, const Term&)>(&carl::operator*))
    .def("__mul__",  static_cast<Term (*)(const Rational&, const Monomial&)>(&carl::operator*))
    .def("__mul__",  static_cast<Term (*)(const Rational&, carl::Variable::Arg)>(&carl::operator*))
    .def(py::self * py::self)

    .def(py::self / py::self)

    .def("__pow__", static_cast<Rational (*)(const Rational&, std::size_t)>(&carl::pow))
    .def("__pos__", [](const Rational& var) -> Rational {return Rational(var);})
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

    py::enum_<carl::VariableType>(m, "VariableType")
        .value("BOOL", carl::VariableType::VT_BOOL)
        .value("INT", carl::VariableType::VT_INT)
        .value("REAL", carl::VariableType::VT_REAL);

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

    m.def("rationalize", static_cast<Rational (*)(double)>(&carl::rationalize), "Construct a rational number from a double");

    // Commented out, for Python Variable can be constructed directly
    /*
    // Non-constructable class VariablePool, static instance accessible via global
    py::class_<carl::VariablePool>(m, "VariablePoolInst")
        .def("get_fresh_variable", static_cast<carl::Variable (carl::VariablePool::*)(carl::VariableType)>(&carl::VariablePool::getFreshVariable))
        .def("get_fresh_variable", static_cast<carl::Variable (carl::VariablePool::*)(const std::string&, carl::VariableType)>(&carl::VariablePool::getFreshVariable))
        .def("find_variable_with_name", &carl::VariablePool::findVariableWithName)
        ;

    // Non-constructable class MonomialPool, static instance accessible via global
    py::class_<carl::MonomialPool>(m, "MonomialPoolInst")
        .def("create", static_cast<carl::Monomial::Arg (carl::MonomialPool::*)(carl::Variable::Arg, carl::exponent)>(&carl::MonomialPool::create))
        ;

    // Setup the global variables
    m.attr("VariablePool") = py::cast(carl::VariablePool::getInstance(), py::return_value_policy::reference);
    */

    return m.ptr();
}
