#include <cassert>

#include "carl/core/Monomial.h"
#include "carl/core/MonomialPool.h"

int main() {
	/*
	 * A carl::Monomial object represents a monomial, being the product of
	 * multiple variables. Our representation consists of a list of (unique)
	 * carl::Variable objects, each associated with an exponent.
	 *
	 * Internally, a carl::Monomial stores the actual monomial as 
	 * std::vector<std::pair<carl::Variable,unsigned>>
	 * and, for caching purposes, the following data:
	 * - the total degree, that is the sum of all exponents,
	 * - the id, a unique identifier managed by the carl::MonomialPool and
	 * - the hash of the monomial.
	 *
	 * A Monomial can either be constructed from a carl::Variable and an
	 * exponent, or from an appropriate std::vector<> or std::initializer_list<>.
	 */
	carl::Variable x = carl::freshRealVariable("x");
	carl::Variable y = carl::freshRealVariable("y");

	carl::Monomial a(x, 2);
	carl::Monomial b({std::make_pair(x,3), std::make_pair(y, 2)});

	/*
	 * However, carl::Monomial objects are managed by a carl::MonomialPool,
	 * though the carl::MonomialPool seldomly accessed directly.
	 * This means, that there is only a single instance of every monomial and
	 * all objects that use this monomial have a std::shared_ptr of this 
	 * instance. To obtain this std::shared_ptr, use carl::createMonomial
	 * instead of the normal constructor.
	 * The type of the std::shared_ptr is defined by carl::Monomial::Arg.
	 */
	
	carl::Monomial::Arg c = carl::createMonomial(x, (carl::exponent)2);
	carl::Monomial::Arg d = carl::createMonomial(std::initializer_list<std::pair<carl::Variable, carl::exponent>>({std::make_pair(x,(carl::exponent)3), std::make_pair(y, (carl::exponent)2)}), (carl::exponent)5);

	assert(c == x*x);
	assert(d == x*x*x * y*y);

	/*
	 * As you see, the creation of monomials with multiple variables is somewhat
	 * clumsy, but in the above assertions a simpler way is already used.
	 * Two variables can be multiplied, resulting in a monomial. Monomials can 
	 * also be multiplied, resulting in a new monomial.
	 */
	carl::Monomial::Arg e = x*x*y;
	carl::Monomial::Arg f = x*y;
	carl::Monomial::Arg g = e*f;

	assert(g == d);
}
