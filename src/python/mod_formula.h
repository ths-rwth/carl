#pragma once

#include "mod_func.h"

#include "carl/formula/Constraint.h"
#include "carl/formula/Formula.h"

typedef carl::Constraint<Polynomial> Constraint;
typedef carl::Formula<Polynomial> Formula;
