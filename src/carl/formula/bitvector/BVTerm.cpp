/**
 * @file BVTerm.cpp
 * @author Andreas Krueger <andreas.krueger@rwth-aachen.de>
 */

#include "BVTerm.h"
#include "BVTermContent.h"
#include "BVTermPool.h"

#include "../../core/Variables.h"

#include <utility>

namespace carl {
BVTerm::BVTerm()
	: mpContent(BVTermPool::getInstance().create()) {
}

BVTerm::BVTerm(BVTermType _type, BVValue _value)
	: mpContent(BVTermPool::getInstance().create(_type, std::move(_value))) {
}

BVTerm::BVTerm(BVTermType _type, const BVVariable& _variable)
	: mpContent(BVTermPool::getInstance().create(_type, _variable)) {
}

BVTerm::BVTerm(BVTermType _type, const BVTerm& _operand, std::size_t _index)
	: mpContent(BVTermPool::getInstance().create(_type, _operand, _index)) {
}

BVTerm::BVTerm(BVTermType _type, const BVTerm& _first, const BVTerm& _second)
	: mpContent(BVTermPool::getInstance().create(_type, _first, _second)) {
}

BVTerm::BVTerm(BVTermType _type, const BVTerm& _operand, std::size_t _first, std::size_t _last)
	: mpContent(BVTermPool::getInstance().create(_type, _operand, _first, _last)) {
}

std::size_t BVTerm::hash() const {
	return mpContent->hash();
}

std::size_t BVTerm::width() const {
	return mpContent->width();
}

BVTermType BVTerm::type() const {
	return mpContent->type();
}

void BVTerm::gatherVariables(carlVariables& vars) const {
	mpContent->gatherVariables(vars);
}

bool BVTerm::isInvalid() const {
	return mpContent->isInvalid();
}

const BVTerm& BVTerm::operand() const {
	if (mpContent->type() == BVTermType::EXTRACT) {
		return mpContent->as<BVExtractContent>().mOperand;
	} else {
		return mpContent->as<BVUnaryContent>().mOperand;
	}
}

std::size_t BVTerm::index() const {
	return mpContent->as<BVUnaryContent>().mIndex;
}

const BVTerm& BVTerm::first() const {
	return mpContent->as<BVBinaryContent>().mFirst;
}

const BVTerm& BVTerm::second() const {
	return mpContent->as<BVBinaryContent>().mSecond;
}

std::size_t BVTerm::highest() const {
	return mpContent->as<BVExtractContent>().mHighest;
}

std::size_t BVTerm::lowest() const {
	return mpContent->as<BVExtractContent>().mLowest;
}

const BVVariable& BVTerm::variable() const {
	return mpContent->as<BVVariable>();
}

const BVValue& BVTerm::value() const {
	return mpContent->as<BVValue>();
}

std::size_t BVTerm::complexity() const {
	return mpContent->complexity();
}

BVTerm BVTerm::substitute(const std::map<BVVariable, BVTerm>& _substitutions) const {
	BVTermType type = this->type();

	if (type == BVTermType::CONSTANT) {
		return *this;
	}
	if (type == BVTermType::VARIABLE) {
		auto iter = _substitutions.find(variable());
		if (iter != _substitutions.end()) {
			return iter->second;
		}
		return *this;
	}
	if (typeIsUnary(type)) {
		BVTerm operandSubstituted = operand().substitute(_substitutions);
		return BVTerm(type, operandSubstituted, index());
	}
	if (typeIsBinary(type)) {
		BVTerm firstSubstituted = first().substitute(_substitutions);
		BVTerm secondSubstituted = second().substitute(_substitutions);
		return BVTerm(type, firstSubstituted, secondSubstituted);
	}
	if (type == BVTermType::EXTRACT) {
		BVTerm operandSubstituted = operand().substitute(_substitutions);
		return BVTerm(type, operandSubstituted, highest(), lowest());
	}

	assert(false);
	return BVTerm();
}

bool operator==(const BVTerm& lhs, const BVTerm& rhs) {
	return lhs.mpContent == rhs.mpContent;;
}
bool operator<(const BVTerm& lhs, const BVTerm& rhs) {
	return *lhs.mpContent < *rhs.mpContent;;
}

std::ostream& operator<<(std::ostream& os, const BVTerm& term) {
	return os << *term.mpContent;
}



} // namespace carl
