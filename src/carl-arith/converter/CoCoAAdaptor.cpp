#include "CoCoAAdaptor.h"

#ifdef USE_COCOA

#include <CoCoA/library.H>

namespace carl {
namespace cocoawrapper {

CoCoA::RingElem gcd(const CoCoA::RingElem& p, const CoCoA::RingElem& q) {
	return CoCoA::gcd(p, q);
}
CoCoA::factorization<CoCoA::RingElem> factor(const CoCoA::RingElem& p) {
	return CoCoA::factor(p);
}
std::vector<CoCoA::RingElem> ReducedGBasis(const std::vector<CoCoA::RingElem>& p) {
	return CoCoA::ReducedGBasis(CoCoA::ideal(p));
}
CoCoA::factorization<CoCoA::RingElem> SqFreeFactor(const CoCoA::RingElem& p) {
	return CoCoA::factor(p);
}

}
}

#endif
