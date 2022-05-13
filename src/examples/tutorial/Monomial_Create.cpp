#include <cassert>

#include <carl/poly/umvpoly/Monomial.h>
#include <carl/poly/umvpoly/MonomialPool.h>

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
	 */
	carl::Variable x = carl::fresh_real_variable("x");
	carl::Variable y = carl::fresh_real_variable("y");

	/*
	 * However, carl::Monomial objects are managed by a carl::MonomialPool,
	 * though the carl::MonomialPool seldomly accessed directly.
	 * This means, that there is only a single instance of every monomial and
	 * all objects that use this monomial have a std::shared_ptr of this 
	 * instance. To obtain this std::shared_ptr, use carl::createMonomial
	 * instead of the normal constructor.
	 * The type of the std::shared_ptr is defined by carl::Monomial::Arg.
	 */
	
	auto a = carl::createMonomial(x, (carl::exponent)2);
	auto b = carl::createMonomial(std::initializer_list<std::pair<carl::Variable, carl::exponent>>({std::make_pair(x,(carl::exponent)3), std::make_pair(y, (carl::exponent)2)}), (carl::exponent)5);

	assert(a == x*x);
	assert(b == x*x*x * y*y);

	/*
	 * As you see, the creation of monomials with multiple variables is somewhat
	 * clumsy, but in the above assertions a simpler way is already used.
	 * Two variables can be multiplied, resulting in a monomial. Monomials can 
	 * also be multiplied, resulting in a new monomial.
	 */
	auto c = x*x*y;
	auto d = x*y;
	auto e = c*d;

	assert(e == b);
}
