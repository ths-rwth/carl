/* 
 * File:   BVTermPool.cpp
 * Author: Andreas Krueger <andreas.krueger@rwth-aachen.de>
 */

#include "BVTermPool.h"
#include "BVTermContent.h"

namespace carl
{
    BVTermPool::BVTermPool():
		mpInvalid(add(new Term))
    {
    }

    BVTermPool::ConstTermPtr BVTermPool::create()
    {
        return this->mpInvalid;
    }

    BVTermPool::ConstTermPtr BVTermPool::create(BVTermType _type, BVValue&& _value)
    {
        return this->add(new Term(_type, std::move(_value)));
    }

    BVTermPool::ConstTermPtr BVTermPool::create(BVTermType _type, const BVVariable& _variable)
    {
        return this->add(new Term(_type, _variable));
    }

    BVTermPool::ConstTermPtr BVTermPool::create(BVTermType _type, const BVTerm& _operand, std::size_t _index)
    {
        if (_operand.isConstant()) {
            switch (_type) {
                case BVTermType::NOT: {
                    return create(BVTermType::CONSTANT, ~_operand.value());
                }
                case BVTermType::NEG: {
                    return create(BVTermType::CONSTANT, -_operand.value());
                }
                case BVTermType::LROTATE: {
                    return create(BVTermType::CONSTANT, _operand.value().rotateLeft(_index));
                }
                case BVTermType::RROTATE: {
                    return create(BVTermType::CONSTANT, _operand.value().rotateRight(_index));
                }
                case BVTermType::REPEAT: {
                    return create(BVTermType::CONSTANT, _operand.value().repeat(_index));
                }
                case BVTermType::EXT_U: {
                    return create(BVTermType::CONSTANT, _operand.value().extendUnsignedBy(_index));
                }
                case BVTermType::EXT_S: {
                    return create(BVTermType::CONSTANT, _operand.value().extendSignedBy(_index));
                }
                default:
                    CARL_LOG_WARN("carl.bitvector", "No simplification for " << _type << " BVTerm.");
            }
        }
        return this->add(new Term(_type, _operand, _index));
    }

    BVTermPool::ConstTermPtr BVTermPool::create(BVTermType _type, const BVTerm& _first, const BVTerm& _second)
    {
        // Catch expressions leading to an "undefined" result (i.e., division by zero)
        // As of SMT-LIB 2.6, this is defined as #b111...
        if (_second.isConstant() && _second.value().isZero()) {
			switch (_type) {
				case BVTermType::DIV_U:
					return create(BVTermType::CONSTANT, ~BVValue(_first.width(), 0));
				case BVTermType::DIV_S:
					if (_first.isConstant()) {
						if (_first.value().isZero()) {
							// Zero -> 1
							return create(BVTermType::CONSTANT, BVValue(_first.width(), 1));
						} else if (_first.value()[_first.width()-1]) {
							// Negative -> 1
							return create(BVTermType::CONSTANT, BVValue(_first.width(), 1));
						} else {
							// Positive -> _first
							return _first.mpContent;
						}
					}
					break;
				case BVTermType::MOD_S1:
				case BVTermType::MOD_S2:
				case BVTermType::MOD_U:
					return _first.mpContent;
				default:
					break;
			}
        }

        // Evaluate term if both terms arguments are constant
        if (_first.isConstant() && _second.isConstant()) {
            switch (_type) {
                case BVTermType::CONCAT: {
                    return create(BVTermType::CONSTANT, _first.value().concat(_second.value()));
                }
                case BVTermType::AND: {
                    return create(BVTermType::CONSTANT, _first.value() & _second.value());
                }
                case BVTermType::OR: {
                    return create(BVTermType::CONSTANT, _first.value() | _second.value());
                }
                case BVTermType::XOR: {
                    return create(BVTermType::CONSTANT, _first.value() ^ _second.value());
                }
                case BVTermType::NAND: {
                    return create(BVTermType::CONSTANT, ~(_first.value() & _second.value()));
                }
                case BVTermType::NOR: {
                    return create(BVTermType::CONSTANT, ~(_first.value() | _second.value()));
                }
                case BVTermType::XNOR: {
                    return create(BVTermType::CONSTANT, ~(_first.value() ^ _second.value()));
                }
                case BVTermType::ADD: {
                    return create(BVTermType::CONSTANT, _first.value() + _second.value());
                }
                case BVTermType::SUB: {
                    return create(BVTermType::CONSTANT, _first.value() - _second.value());
                }
                case BVTermType::MUL: {
                    return create(BVTermType::CONSTANT, _first.value() * _second.value());
                }
                case BVTermType::DIV_U: {
                    return create(BVTermType::CONSTANT, _first.value() / _second.value());
                }
                case BVTermType::DIV_S: {
                    return create(BVTermType::CONSTANT, _first.value().divideSigned(_second.value()));
                }
                case BVTermType::MOD_U: {
                    return create(BVTermType::CONSTANT, _first.value() % _second.value());
                }
                case BVTermType::MOD_S1: {
                    return create(BVTermType::CONSTANT, _first.value().remSigned(_second.value()));
                }
                case BVTermType::MOD_S2: {
                    return create(BVTermType::CONSTANT, _first.value().modSigned(_second.value()));
                }
                case BVTermType::EQ: {
                    assert(_first.width() == _second.width());
                    unsigned val = (_first.value() == _second.value()) ? 1 : 0;
                    return create(BVTermType::CONSTANT, BVValue(1, val));
                }
                case BVTermType::LSHIFT: {
                    return create(BVTermType::CONSTANT, _first.value() << _second.value());
                }
                case BVTermType::RSHIFT_LOGIC: {
                    return create(BVTermType::CONSTANT, _first.value() >> _second.value());
                }
                case BVTermType::RSHIFT_ARITH: {
                    return create(BVTermType::CONSTANT, _first.value().rightShiftArithmetic(_second.value()));
                }
                default:
                    CARL_LOG_WARN("carl.bitvector", "No simplification for " << _type << " BVTerm.");
            }
        }

        return this->add(new Term(_type, _first, _second));
    }

    BVTermPool::ConstTermPtr BVTermPool::create(BVTermType _type, const BVTerm& _operand, std::size_t _highest, std::size_t _lowest)
    {
        if(_operand.isConstant()) {
            if(_type == BVTermType::EXTRACT) {
                return create(BVTermType::CONSTANT, _operand.value().extract(_highest, _lowest));
            } else {
                CARL_LOG_WARN("carl.bitvector", "No simplification for " << _type << " BVTerm.");
            }
        }
        return this->add(new Term(_type, _operand, _highest, _lowest));
    }

    void BVTermPool::assignId(TermPtr _term, std::size_t _id)
    {
        _term->mId = _id;
    }
}

#define BV_TERM_POOL BVTermPool::getInstance()
