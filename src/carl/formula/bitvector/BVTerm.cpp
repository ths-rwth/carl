/**
 * @file BVTerm.cpp
 * @author Andreas Krueger <andreas.krueger@rwth-aachen.de>
 */

#include "BVTermPool.h"
#include "BVTerm.h"

namespace carl
{
    BVTerm::BVTerm() :
    mpContent(BVTermPool::getInstance().create())
    {
    }

    BVTerm::BVTerm(BVTermType _type, BVValue _value) :
    mpContent(BVTermPool::getInstance().create(_type, std::move(_value)))
    {
    }

    BVTerm::BVTerm(BVTermType _type, const BVVariable& _variable) :
    mpContent(BVTermPool::getInstance().create(_type, _variable))
    {
    }

    BVTerm::BVTerm(BVTermType _type, const BVTerm& _operand, std::size_t _index) :
    mpContent(BVTermPool::getInstance().create(_type, _operand, _index))
    {
    }

    BVTerm::BVTerm(BVTermType _type, const BVTerm& _first, const BVTerm& _second) :
    mpContent(BVTermPool::getInstance().create(_type, _first, _second))
    {
    }

    BVTerm::BVTerm(BVTermType _type, const BVTerm& _operand, std::size_t _first, std::size_t _last) :
    mpContent(BVTermPool::getInstance().create(_type, _operand, _first, _last))
    {
    }

    std::string BVTerm::toString(const std::string& _init, bool _oneline, bool _infix, bool _friendlyNames) const
    {
        return mpContent->toString(_init, _oneline, _infix, _friendlyNames);
    }

    std::size_t BVTerm::hash() const
    {
        return mpContent->hash();
    }

    std::size_t BVTerm::width() const
    {
        return mpContent->width();
    }

    BVTermType BVTerm::type() const
    {
        return mpContent->type();
    }
    
    void BVTerm::collectVariables(std::set<BVVariable>& vars) const {
        mpContent->collectVariables(vars);
    }

    bool BVTerm::isInvalid() const
    {
        return mpContent->isInvalid();
    }

    const BVTerm& BVTerm::operand() const
    {
		if (mpContent->type() == BVTermType::EXTRACT) {
			return mpContent->getExtract().mOperand;
		}
		else {
			return mpContent->getUnary().mOperand;
		}
    }

    std::size_t BVTerm::index() const
    {
		return mpContent->getUnary().mIndex;
    }

    const BVTerm& BVTerm::first() const
    {
		return mpContent->getBinary().mFirst;
	}

    const BVTerm& BVTerm::second() const
    {
		return mpContent->getBinary().mSecond;
	}

    std::size_t BVTerm::highest() const
    {
		return mpContent->getExtract().mHighest;
	}

    std::size_t BVTerm::lowest() const
    {
		return mpContent->getExtract().mLowest;
	}

    const BVVariable& BVTerm::variable() const
    {
		return mpContent->getVariable();
	}

    const BVValue& BVTerm::value() const
    {
		return mpContent->getValue();
	}

    size_t BVTerm::complexity() const
    {
        if(this->type() == BVTermType::CONSTANT)
            return 1;
        if(this->type() == BVTermType::VARIABLE)
            return mpContent->mWidth;
		if (typeIsUnary(this->type()))
			return mpContent->mWidth + mpContent->getUnary().mOperand.complexity();
		if (typeIsBinary(this->type()))
			return mpContent->mWidth + mpContent->getBinary().mFirst.complexity() + mpContent->getBinary().mSecond.complexity();
		assert(this->type() == BVTermType::EXTRACT);
		return mpContent->getExtract().mHighest - mpContent->getExtract().mLowest + mpContent->getExtract().mOperand.complexity();
    }
    
    BVTerm BVTerm::substitute(const std::map<BVVariable,BVTerm>& _substitutions) const
    {
        BVTermType type = this->type();

        if(type == BVTermType::CONSTANT) {
            return *this;
        }
        if(type == BVTermType::VARIABLE) {
            auto iter = _substitutions.find(variable());
            if(iter != _substitutions.end())
            {
                return iter->second;
            }
            return *this;
        }
        if(typeIsUnary(type)) {
            BVTerm operandSubstituted = operand().substitute(_substitutions);
            return BVTerm(type, operandSubstituted, index());
        }
        if(typeIsBinary(type)) {
            BVTerm firstSubstituted = first().substitute(_substitutions);
            BVTerm secondSubstituted = second().substitute(_substitutions);
            return BVTerm(type, firstSubstituted, secondSubstituted);
        }
        if(type == BVTermType::EXTRACT) {
            BVTerm operandSubstituted = operand().substitute(_substitutions);
            return BVTerm(type, operandSubstituted, highest(), lowest());
        }

        assert(false);
        return BVTerm();
    }

    bool BVTerm::operator<(const BVTerm& rhs) const {
        return *mpContent < *(rhs.mpContent);
    }

}
