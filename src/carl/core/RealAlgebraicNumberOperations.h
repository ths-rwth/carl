/* 
 * File:   RealAlgebraicNumberOperations.h
 * Author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * 
 * This file should never be included directly but only via RealAlgebraicNumber.h
 */

#pragma once

namespace carl {

template<typename Number>
struct Equal {
	bool operator()(const RealAlgebraicNumber<Number>* lhs, const RealAlgebraicNumber<Number>* rhs) {
		if (lhs == rhs) return true;
		if (lhs->isNumeric()) {
			if (rhs->isNumeric()) {
				return lhs->value() == rhs->value();
			} else {
				// TODO
			}
		} else {
			if (rhs->isNumeric()) {
				// TODO
			} else {
				// TODO
			}
		}
	}
};
template<typename Number>
struct Inequal {
private:
	Equal<Number> e;
public:
	bool operator()(const RealAlgebraicNumber<Number>* lhs, const RealAlgebraicNumber<Number>* rhs) {
		return !e(lhs, rhs);
	}
};


template<typename Number>
struct Less {
	bool operator()(const RealAlgebraicNumber<Number>* lhs, const RealAlgebraicNumber<Number>* rhs) {
		if (lhs == rhs) return false;
		if (lhs->isNumeric()) {
			if (rhs->isNumeric()) {
				return lhs->value() < rhs->value();
			} else {
				return lhs->value() < static_cast<const RealAlgebraicNumberIR<Number>*>(rhs)->getInterval().left();
			}
		} else {
			if (rhs->isNumeric()) {
				return static_cast<const RealAlgebraicNumberIR<Number>*>(rhs)->getInterval().right() < rhs->value();
			} else {
				// TODO
			}
		}
	}
};

template<typename Number>
struct Greater {
private:
	Less<Number> l;
public:
	bool operator()(const RealAlgebraicNumber<Number>* lhs, const RealAlgebraicNumber<Number>* rhs) {
		return l(rhs, lhs);
	}
};

}