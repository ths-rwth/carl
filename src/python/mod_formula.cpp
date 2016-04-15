#include "mod_formula.h"

#include "common.h"
#include "helpers.h"

PYBIND11_PLUGIN(formula) {
	py::module m("pycarl.formula", "pycarl formula handling");

	// Constraint relies on Rational
	m.import("pycarl");

	py::enum_<carl::Relation>(m, "Relation")
		.value("EQ", carl::Relation::EQ)
		.value("NEQ", carl::Relation::NEQ)
		.value("LESS", carl::Relation::LESS)
		.value("LEQ", carl::Relation::LEQ)
		.value("GREATER", carl::Relation::GREATER)
		.value("GEQ", carl::Relation::GEQ)
		;

	py::class_<Constraint>(m, "Constraint")
		.def(py::init<bool>())
		.def(py::init<carl::Variable, carl::Relation, Rational>(), py::arg("var"), py::arg("rel"), py::arg("bound") = Rational(0))
		.def(py::init<Polynomial, carl::Relation>())
		.def("__str__", &streamToString<Constraint>)
		;

	// Mostly just the operators are useful for Python, but as other values may be returned,
	// all of them are included
	py::enum_<carl::FormulaType>(m, "FormulaType")
		.value("ITE", carl::FormulaType::ITE)
		.value("EXISTS", carl::FormulaType::EXISTS)
		.value("FORALL", carl::FormulaType::FORALL)
		.value("TRUE", carl::FormulaType::TRUE)
		.value("FALSE", carl::FormulaType::FALSE)
		.value("BOOL", carl::FormulaType::BOOL)
		.value("NOT", carl::FormulaType::NOT)
		.value("IMPLIES", carl::FormulaType::IMPLIES)
		.value("AND", carl::FormulaType::AND)
		.value("OR", carl::FormulaType::OR)
		.value("XOR", carl::FormulaType::XOR)
		.value("IFF", carl::FormulaType::IFF)
		.value("CONSTRAINT", carl::FormulaType::CONSTRAINT)
		.value("BITVECTOR", carl::FormulaType::BITVECTOR)
		.value("UEQ", carl::FormulaType::UEQ)
		;

	py::class_<Formula>(m, "Formula")
		.def(py::init<bool>())
		.def(py::init<carl::Variable>(), "Create Formula given Boolean variable")
		.def(py::init<Constraint>())
		.def(py::init<carl::FormulaType, Formula>())
		.def(py::init<carl::FormulaType, carl::Formulas<Polynomial>>())
		.def("__str__", &streamToString<carl::Formula<Polynomial>>)
		;

	return m.ptr();
}
