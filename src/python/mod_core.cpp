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
    .def(py::init<int>())
    .def("__float__", static_cast<double (*)(Rational const&)>(&carl::toDouble))
    .def("__str__", [](Rational const& r) {return carl::toString(r, true);})
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
        )
        .def("__init__",
            [](carl::Variable &instance, carl::VariableType type) -> void {
                carl::Variable tmp = carl::VariablePool::getInstance().getFreshVariable(type);
                new (&instance) carl::Variable(tmp);
            }
        )
        //.def(py::self + py::self)
        //.def(py::self - py::self)
        .def(py::self * py::self)
        .def_property_readonly("name", &carl::Variable::getName)
        .def_property_readonly("type", &carl::Variable::getType)
        .def("__str__", &streamToString<carl::Variable>)
        .def("__mul__", static_cast<carl::Monomial::Arg (*)(carl::Variable::Arg, const carl::Monomial::Arg&)>(&carl::operator*))
        .def("__add__", static_cast<Polynomial (*)(carl::Variable::Arg, Rational const&)>(&carl::operator+))
        .def("__sub__", static_cast<Polynomial (*)(carl::Variable::Arg, Rational const&)>(&carl::operator-))
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
