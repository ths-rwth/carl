#include "gtest/gtest.h"

#include <memory>
#include <list>

#include <carl-cad/ConflictGraph.h>
#include "carl/util/platform.h"

#include "../Common.h"

using namespace carl;

TEST(ConflictGraph, BasicOperations)
{
    cad::ConflictGraph<Rational> cg;
}
