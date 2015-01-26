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
#include "../numbers/numbers.h"

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
		case SampleOrdering::IntRatSize:
			CHECK(compareInt(lhs, rhs));
			CHECK(compareRat(lhs, rhs));
			CHECK(compareSize(lhs, rhs));
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
		case SampleOrdering::Value:
			return carl::less<RealAlgebraicNumberPtr<Number>>()(lhs, rhs);
			break;
		default:
			CARL_LOG_FATAL("carl.cad.sampleset", "Ordering " << mOrdering << " was not implemented.");
			assert(false);
			return false;
	}
	return carl::less<RealAlgebraicNumberPtr<Number>>()(lhs, rhs);
}

template<typename Number>
bool SampleSet<Number>::SampleComparator::isOptimal(const RealAlgebraicNumberPtr<Number>& s) const {
	switch (mOrdering) {
		case SampleOrdering::IntRatRoot:
			return s->isNumericRepresentation() && carl::isInteger(s->value());
		case SampleOrdering::IntRatSize:
			return s->isNumericRepresentation() && carl::isInteger(s->value());
		case SampleOrdering::Interval:
			return !s->isNumericRepresentation();
		case SampleOrdering::NonRoot:
			return !s->isRoot();
		case SampleOrdering::RatRoot:
			return s->isNumericRepresentation();
		case SampleOrdering::Root:
			return s->isRoot();
		case SampleOrdering::Value:
			return true;
		default:
			CARL_LOG_FATAL("carl.cad.sampleset", "Ordering " << mOrdering << " was not implemented.");
			assert(false);
			return false;
	}
}

template<typename Number>
std::tuple<typename SampleSet<Number>::Iterator, bool, bool> SampleSet<Number>::insert(RealAlgebraicNumberPtr<Number> r) {
	CARL_LOG_TRACE("carl.cad.sampleset", this << " " << __func__ << "( " << r << " )");
	CARL_LOG_TRACE("carl.cad.sampleset", *this);
	assert(this->isConsistent());
	auto res = this->mSamples.insert(r);
	auto result = std::make_tuple(res.first, res.second, false);
	CARL_LOG_TRACE("carl.cad.sampleset", "\tinsert(): " << *res.first << ", " << res.second);
	if (res.second) {
		mHeap.push_back(r);
		std::push_heap(mHeap.begin(), mHeap.end(), mComp);
	} else if (!(*res.first)->isRoot() && r->isRoot()) {
		this->remove(res.first);
		std::get<0>(result) = std::get<0>(this->insert(r));
		std::get<2>(result) = true;
	} else if (!(*res.first)->isNumeric() && r->isNumeric()) {
		this->remove(res.first);
		std::get<0>(result) = std::get<0>(this->insert(RealAlgebraicNumberNR<Number>::create(r->value(), true)));
		std::get<2>(result) = true;
	}
	assert(this->isConsistent());
	CARL_LOG_TRACE("carl.cad.sampleset", *this);
	return result;
}

template<typename Number>
void SampleSet<Number>::pop() {
	CARL_LOG_TRACE("carl.cad.sampleset", this << " " << __func__ << "()");
	if (this->mHeap.empty()) return;
	this->mSamples.erase(mHeap.front());
	std::pop_heap(mHeap.begin(), mHeap.end(), mComp);
	mHeap.pop_back();
	assert(this->isConsistent());
}

template<typename Number>
bool SampleSet<Number>::simplify(const RealAlgebraicNumberIRPtr<Number> from, RealAlgebraicNumberNRPtr<Number> to) {
	CARL_LOG_TRACE("carl.cad.sampleset", this << " " << __func__ << "( " << from << " -> " << to << " )");
	assert(this->isConsistent());
	if (mSamples.count(from) > 0) {
		mSamples.erase(from);
		mSamples.insert(to);
		auto it = std::find(mHeap.begin(), mHeap.end(), from);
		*it = to;
		assert(this->isConsistent());
		return true;
	}
	return false;
}

template<typename Number>
std::pair<typename SampleSet<Number>::SampleSimplification, bool> SampleSet<Number>::simplify(bool fast) {
	CARL_LOG_TRACE("carl.cad.sampleset", this << " " << __func__ << "()");
	std::pair<SampleSimplification, bool> simplification;
	if (this->empty()) return simplification;
	simplification.second = false;
	for (auto n: this->mSamples) {
		if (n->isNumericRepresentation()) continue;
		auto nIR = std::static_pointer_cast<RealAlgebraicNumberIR<Number>>(n);
		if (!fast && !nIR->isNumeric() && nIR->getRefinementCount() == 0) {
			// Try at least one refinement.
			nIR->refine();
		}
		if (nIR->isNumeric()) {
			auto nNR = RealAlgebraicNumberNR<Number>::create(nIR->value(), nIR->isRoot());
			simplification.first[nIR] = nNR;
		}
	}
	for (auto it: simplification.first) {
		if (this->simplify(it.first, it.second)) {
			simplification.second = true;
		} else {
			assert(false);
		}
	}
	assert(this->isConsistent());
	return simplification;
}

template<typename Number>
std::ostream& operator<<(std::ostream& os, const SampleSet<Number>& s) {
	os << "SampleSet " << &s << " with ordering " << s.ordering() << std::endl;
	os << "samples: " << s.mSamples << std::endl;
	os << "heap: " << s.mHeap << std::endl;
	return os;
}

template<typename Number>
bool SampleSet<Number>::isConsistent() const {
	CARL_LOG_TRACE("carl.cad.sampleset", this << " " << __func__ << "()");
	CARL_LOG_TRACE("carl.cad.sampleset", "samples: " << mSamples);
	CARL_LOG_TRACE("carl.cad.sampleset", "heap:    " << mHeap);
	std::set<RealAlgebraicNumberPtr<Number>> queue(mHeap.begin(), mHeap.end());
	for (auto n: this->mSamples) {
		auto it = queue.find(n);
		if (it == queue.end()) {
			CARL_LOG_ERROR("carl.cad.sampleset", "Sample " << n << " is not in queue.");
			assert(queue.find(n) != queue.end());
		}
		queue.erase(it);
	}
	RealAlgebraicNumberPtr<Number> lastSample = nullptr;
	for (auto n: this->mSamples) {
		if (lastSample != nullptr && !carl::less<RealAlgebraicNumberPtr<Number>>()(lastSample, n)) {
			CARL_LOG_ERROR("carl.cad.sampleset", "samples: " << mSamples);
			CARL_LOG_ERROR("carl.cad.sampleset", "Samples in samples not in order: " << lastSample << " < " << n);
			assert(carl::less<RealAlgebraicNumberPtr<Number>>()(lastSample, n));
		}
		lastSample = n;
	}
	if (!queue.empty()) {
		CARL_LOG_ERROR("carl.cad.sampleset", "Additional samples in queue: " << queue);
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
	std::swap(lhs.mHeap, rhs.mHeap);
}

}
