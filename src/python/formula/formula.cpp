/*
 * formula.cpp
 *
 *  Created on: 16 Apr 2016
 *      Author: harold
 */

#include "formula.h"

#include "common.h"

void define_formula_type(py::module& m) {
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
}

void define_formula(py::module& m) {
    py::class_<Formula>(m, "Formula")
        .def(py::init<bool>())
        .def(py::init<carl::Variable>(), "Create Formula given Boolean variable")
        .def(py::init<Constraint>())
        .def(py::init<carl::FormulaType, Formula>())
        .def(py::init<carl::FormulaType, carl::Formulas<Polynomial>>())
        .def("__str__", &streamToString<carl::Formula<Polynomial>>)
        ;
}
