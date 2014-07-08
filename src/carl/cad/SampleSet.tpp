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
#define CHECK(expr) res = (expr); if (res.first) return res.second;
	std::pair<bool, bool> res;
	switch (mOrdering) {
		case SampleOrdering::IntRatRoot:
			CHECK(compareInt(lhs, rhs));
			CHECK(compareRat(lhs, rhs));
			CHECK(compareRoot(lhs, rhs));
			break;
		case SampleOrdering::Interval:
			CHECK(compareRat(rhs, lhs));
			break;
		case SampleOrdering::NonRoot:
			CHECK(compareRoot(rhs, lhs));
			break;
		case SampleOrdering::RatRoot:
			CHECK(compareRat(lhs, rhs));
			CHECK(compareRoot(lhs, rhs));
			break;
		case SampleOrdering::Root:
			CHECK(compareRoot(lhs, rhs));
			break;
		default:
			LOGMSG_FATAL("carl.cad.sampleset", "Ordering " << mOrdering << " was not implemented.");
			assert(false);
			return false;
	}
	return lhs < rhs;
}

template<typename Number>
bool SampleSet<Number>::SampleComparator::isOptimal(const RealAlgebraicNumberPtr<Number>& s) const {
	switch (mOrdering) {
		case SampleOrdering::IntRatRoot:
			return s->isNumeric() && carl::isInteger(s->value());
		case SampleOrdering::Interval:
			return !s->isNumericRepresentation();
		case SampleOrdering::NonRoot:
			return !s->isRoot();
		case SampleOrdering::RatRoot:
			return s->isNumeric();
		case SampleOrdering::Root:
			return s->isRoot();
		default:
			LOGMSG_FATAL("carl.cad.sampleset", "Ordering " << mOrdering << " was not implemented.");
			assert(false);
			return false;
	}
}

template<typename Number>
void SampleSet<Number>::pop() {
	LOGMSG_TRACE("carl.cad.sampleset", this << " " << __func__ << "()");
	if (this->mQueue.empty()) return;
	this->mSamples.erase(*mQueue.begin());
	this->mQueue.erase(mQueue.begin());
	assert(this->isConsistent());
}

template<typename Number>
bool SampleSet<Number>::simplify(const RealAlgebraicNumberIRPtr<Number> from, RealAlgebraicNumberNRPtr<Number> to) {
	LOGMSG_TRACE("carl.cad.sampleset", this << " " << __func__ << "( " << from << " -> " << to << " )");
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
	LOGMSG_TRACE("carl.cad.sampleset", this << " " << __func__ << "()");
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
	LOGMSG_TRACE("carl.cad.sampleset", this << " " << __func__ << "()");
	std::set<RealAlgebraicNumberPtr<Number>> queue(mQueue.begin(), mQueue.end());
	RealAlgebraicNumberPtr<Number> lastSample = nullptr;
	for (auto n: this->mSamples) {
		if (lastSample != nullptr) {
			if (!carl::Less<Number>()(lastSample, n)) {
				LOGMSG_INFO("carl.cad", "Samples not in order: " << lastSample << " < " << n);
				assert(carl::Less<Number>()(lastSample, n));
			}
		}
		if (queue.find(n) == queue.end()) {
			LOGMSG_INFO("carl.cad", "Sample " << n << " is not in queue.");
			assert(queue.find(n) != queue.end());
		}
		queue.erase(n);
		lastSample = n;
	}
	if (!queue.empty()) {
		LOGMSG_INFO("carl.cad", "Additional samples in queue: " << queue);
		assert(queue.empty());
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
