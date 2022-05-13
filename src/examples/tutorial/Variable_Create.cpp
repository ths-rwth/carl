#include <cassert>

#include <carl/core/Variable.h>
#include <carl/core/VariablePool.h>

int main() {
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
	carl::Variable a = carl::fresh_real_variable("x");
	carl::Variable b = carl::fresh_integer_variable("y");

	assert(a.type() == carl::VariableType::VT_REAL);
	assert(a.name() == "x");
	assert(b.type() == carl::VariableType::VT_INT);
	assert(b.name() == "y");

	/*
	 * You can create anonymous variables by omitting the first argument.
	 * The variables will be printed in any output as "x_<id>" in this case.
	 * You can also omit the second argument, in this case the type is VT_REAL.
	 */
	carl::Variable c = carl::fresh_integer_variable();
	carl::Variable d = carl::fresh_real_variable("d");
	carl::Variable e = carl::fresh_real_variable();

	assert(c.type() == carl::VariableType::VT_INT);
	assert(d.name() == "d");
	assert(e.type() == carl::VariableType::VT_REAL);
	
	/*
	 * As a variable objects technically consists of a single unsigned, most
	 * compilers should optimize and avoid the overhead of actually constructing
	 * objects. In this case, it may make a difference if you have
	 * - carl::Variable
	 * - const carl::Variable&
	 * as a function argument. Usually, carl::Variable should be preferred.
	 */
}
