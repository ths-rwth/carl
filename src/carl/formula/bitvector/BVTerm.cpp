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
    mpContent(BVTermPool::getInstance().create(_type, _value))
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

    std::string BVTerm::toString(const std::string _init, bool _oneline, bool _infix, bool _friendlyNames) const
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
#ifdef __VS
        if(mpContent->type() == BVTermType::EXTRACT) {
            return mpContent->mpExtractVS->mOperand;
        } else {
            return mpContent->mpUnaryVS->mOperand;
        }
#else
		if (mpContent->type() == BVTermType::EXTRACT) {
			return mpContent->mExtract.mOperand;
		}
		else {
			return mpContent->mUnary.mOperand;
		}
#endif
    }

    std::size_t BVTerm::index() const
    {
#ifdef __VS
        return mpContent->mpUnaryVS->mIndex;
#else
		return mpContent->mUnary.mIndex;
#endif
    }

    const BVTerm& BVTerm::first() const
    {
#ifdef __VS
		return mpContent->mpBinaryVS->mFirst;
#else
		return mpContent->mBinary.mFirst;
#endif
	}

    const BVTerm& BVTerm::second() const
    {
#ifdef __VS
		return mpContent->mpBinaryVS->mSecond;
#else
		return mpContent->mBinary.mSecond;
#endif
	}

    std::size_t BVTerm::highest() const
    {
#ifdef __VS
		return mpContent->mpExtractVS->mHighest;
#else
		return mpContent->mExtract.mHighest;
#endif
	}

    std::size_t BVTerm::lowest() const
    {
#ifdef __VS
		return mpContent->mpExtractVS->mLowest;
#else
		return mpContent->mExtract.mLowest;
#endif
	}

    const BVVariable& BVTerm::variable() const
    {
#ifdef __VS
        return *mpContent->mpVariableVS;
#else
		return mpContent->mVariable;
#endif
	}

    const BVValue& BVTerm::value() const
    {
#ifdef __VS
        return *mpContent->mpValueVS;
#else
		return mpContent->mValue;
#endif
	}

    size_t BVTerm::complexity() const
    {
        if(this->type() == BVTermType::CONSTANT)
            return 1;
        if(this->type() == BVTermType::VARIABLE)
            return mpContent->mWidth;
#ifdef __VS
        if(typeIsUnary(this->type()))
            return mpContent->mWidth + mpContent->mpUnaryVS->mOperand.complexity();
        if(typeIsBinary(this->type()))
            return mpContent->mWidth + mpContent->mpBinaryVS->mFirst.complexity() + mpContent->mpBinaryVS->mSecond.complexity();
        assert(this->type() == BVTermType::EXTRACT);
        return mpContent->mpExtractVS->mHighest - mpContent->mpExtractVS->mLowest + mpContent->mpExtractVS->mOperand.complexity();
#else
		if (typeIsUnary(this->type()))
			return mpContent->mWidth + mpContent->mUnary.mOperand.complexity();
		if (typeIsBinary(this->type()))
			return mpContent->mWidth + mpContent->mBinary.mFirst.complexity() + mpContent->mBinary.mSecond.complexity();
		assert(this->type() == BVTermType::EXTRACT);
		return mpContent->mExtract.mHighest - mpContent->mExtract.mLowest + mpContent->mExtract.mOperand.complexity();
#endif
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
        return *(this->mpContent) < *(rhs.mpContent);
    }

}
