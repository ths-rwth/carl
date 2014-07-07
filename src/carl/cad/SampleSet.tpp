/**
 * @file SampleSet.tpp
 * @ingroup cad
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include "SampleSet.h"

#include <cassert>

#include "../core/RealAlgebraicNumber.h"
#include "../util/debug.h"
#include "../numbers/operations.h"

namespace carl {
namespace cad {

template<typename Number>
bool SampleSet<Number>::SampleComparator::operator()(const RealAlgebraicNumberPtr<Number>& lhs, const RealAlgebraicNumberPtr<Number>& rhs) const {
	std::pair<bool, bool> res;
	switch (mOrdering) {
		case SampleOrdering::IntRatRoot:
			res = compareInt(lhs, rhs);
			if (res.first) return res.second;
			res = compareRat(lhs, rhs);
			if (res.first) return res.second;
			return compareRoot(lhs, rhs).second;
		case SampleOrdering::RatRoot:
			res = compareRat(lhs, rhs);
			if (res.first) return res.second;
			return compareRoot(lhs, rhs).second;
		default:
			assert(false);
			return false;
	}
}

template<typename Number>
void SampleSet<Number>::pop() {
	if (this->mQueue.empty()) return;
	this->mSamples.erase(*mQueue.begin());
	this->mQueue.erase(mQueue.begin());
	assert(this->isConsistent());
}

template<typename Number>
bool SampleSet<Number>::simplify(const RealAlgebraicNumberIRPtr<Number> from, RealAlgebraicNumberNRPtr<Number> to) {
	if (mSamples.count(from) > 0) {
		mSamples.erase(from);
		mSamples.insert(to);
		mQueue.erase(from);
		mQueue.insert(to);
		return true;
	}
	return false;
}

template<typename Number>
std::pair<typename SampleSet<Number>::SampleSimplification, bool> SampleSet<Number>::simplify() {
	std::pair<SampleSimplification, bool> simplification;
	if (this->empty()) return simplification;
	simplification.second = false;
	for (auto n: this->mSamples) {
		if (n->isNumericRepresentation()) continue;
		auto nIR = std::static_pointer_cast<RealAlgebraicNumberIR<Number>>(n);
		if (!nIR->isNumeric() && nIR->getRefinementCount() == 0) {
			// Try at least one refinement.
			nIR->refine();
		}
		if (nIR->isNumeric()) {
			auto nNR = RealAlgebraicNumberNR<Number>::create(nIR->value(), nIR->isRoot());
			if (this->simplify(nIR, nNR)) {
				simplification.first[nIR] = nNR;
				simplification.second = true;
			} else {
				assert(false);
			}
		}
	}
	return simplification;
}

template<typename Number>
std::ostream& operator<<(std::ostream& os, const SampleSet<Number>& s) {
	os << "SampleSet " << &s << " with ordering " << s.mQueue.key_comp().ordering() << std::endl;
	os << "samples: " << s.mSamples << std::endl;
	os << "queue: " << s.mQueue << std::endl;
	return os;
}

template<typename Number>
bool SampleSet<Number>::isConsistent() const {
	std::set<RealAlgebraicNumberPtr<Number>> queue(mQueue.begin(), mQueue.end());
	RealAlgebraicNumberPtr<Number> last = nullptr;
	for (auto n: this->mSamples) {
		if (last != nullptr) {
			if (!carl::Less<Number>()(last, n)) {
				LOGMSG_INFO("carl.cad", "Samples not in order: " << last << " < " << n);
				assert(false);
			}
		}
		if (queue.find(n) == queue.end()) {
			LOGMSG_INFO("carl.cad", "Sample " << n << " is not in queue.");
			assert(false);
		}
		queue.erase(n);
		last = n;
	}
	if (!queue.empty()) {
		LOGMSG_INFO("carl.cad", "Additional samples in queue: " << queue);
		assert(false);
	}
	return true;
}

}
}

namespace std {

template<typename Num>
void swap(carl::cad::SampleSet<Num>& lhs, carl::cad::SampleSet<Num>& rhs) {
	std::swap(lhs.mSamples, rhs.mSamples);
	std::swap(lhs.mQueue, rhs.mQueue);
}

}
