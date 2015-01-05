#include <cassert>

#include "carl/core/Variable.h"
#include "carl/core/VariablePool.h"

int main() {
	carl::VariablePool& pool = carl::VariablePool::getInstance();

	/*
	 * A carl::Variable object represents a single variable.
	 * A variable can have any type from carl::VariableType:
	 * - Real (VT_REAL)
	 * - Integer (VT_INT)
	 * - Boolean (VT_BOOL)
	 * - Uninterpreted (VT_UNINTERPRETED)
	 *
	 * A variable object consists only of a single unsigned variable, which
	 * encodes the following properties:
	 * - Rank
	 * - Id
	 * - Type
	 *
	 * The id is a unique identifier. All variables with the same id are
	 * considered equal. The id will directly be used for comparison.
	 * The rank can be used to change the default ordering at runtime, that is
	 * to impose an ordering that differs from the ordering of the ids.
	 * The type encodes the variable type.
	 *
	 * Usually, variables should be created using the carl::VariablePool.
	 * The VariablePool makes sure that a new variable gets a fresh consecutive
	 * ID and stores variable names.
	 */
	carl::Variable a = pool.getFreshVariable("x", carl::VariableType::VT_REAL);
	carl::Variable b = pool.getFreshVariable("y", carl::VariableType::VT_INT);

	assert(a.getType() == carl::VariableType::VT_REAL);
	assert(pool.getName(a) == "x");
	assert(b.getType() == carl::VariableType::VT_INT);
	assert(pool.getName(b) == "y");

	/*
	 * You can create anonymous variables by omitting the first argument.
	 * The variables will be printed in any output as "x_<id>" in this case.
	 * You can also omit the second argument, in this case the type is VT_REAL.
	 */
	carl::Variable c = pool.getFreshVariable(carl::VariableType::VT_INT);
	carl::Variable d = pool.getFreshVariable("d");
	carl::Variable e = pool.getFreshVariable();

	assert(c.getType() == carl::VariableType::VT_INT);
	assert(pool.getName(c) == "x_3");
	assert(pool.getName(d) == "d");
	assert(e.getType() == carl::VariableType::VT_REAL);
	assert(pool.getName(e) == "x_5");

	/*
	 * In short test cases, it may be easier to simply assign ids manually.
	 * To do this, you can use the carl::Variable constructors directly.
	 * The default constructor produces a variable of id zero, which is
	 * considered invalid. To check for such a case, you can compare with the
	 * staticly stored variable Variable::NO_VARIABLE.
	 */
	carl::Variable f;
	carl::Variable g(1);
	carl::Variable h(7);

	assert(f == carl::Variable::NO_VARIABLE);
	assert(g == a);
	assert(h.getId() == 7);

	/*
	 * As a variable objects technically consists of a single unsigned, most
	 * compilers should optimize and avoid the overhead of actually constructing
	 * objects. In this case, it may make a difference if you have
	 * - carl::Variable
	 * - const carl::Variable&
	 * as a function argument. Use Variable::Arg for function arguments and the
	 * preprocessor define VARIABLE_PASS_BY_VALUE to change the behavior.
	 */
}
