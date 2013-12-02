
#include "EliminationSet.h"


namespace carl {
namespace CAD {

template <typename Coefficient>
std::pair<typename EliminationSet<Coefficient>::PolynomialSet::iterator, bool> EliminationSet<Coefficient>::insert(
		const UnivariatePolynomialPtr<Coefficient> r,
		const std::list<UnivariatePolynomialPtr<Coefficient>>& parents = std::list<UnivariatePolynomialPtr<Coefficient>>( 1, UnivariatePolynomialPtr<Coefficient>() ),
		bool avoidSingle = false
		)
{
	LOG4CPLUS_TRACE(logger, "Adding " << r << " to eliminationset.");
	std::pair<PolynomialSet::iterator, bool> insertValue = this->polynomials.insert(r);
	PolynomialSet::iterator pos = insertValue.first;

	if (
			(
				(parents.empty() || parents.front() == nullptr) && 
				this->hasParents( *pos )
			) || ( 
				!parents.empty() && 
				(parents.front() != nullptr) 
			) 
		) {
		bool oneParentFound = false;
		UnivariatePolynomialPtr<Coefficient> parent1;
		for (auto parent: parents) {
			auto children = this->childrenPerParent.find(parent);
			if (children == this->childrenPerParent.end()) {
				this->childrenPerParent[parent].insert(*pos);
			}
		}
	}

}

}
}
