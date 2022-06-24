/**
 * @file Formula.tpp
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 * @author Ulrich Loup
 * @author Sebastian Junges
 * @since 2012-02-09
 * @version 2014-10-30
 */


#include "Formula.h"
#include "FormulaPool.h"
#include <carl-arith/poly/umvpoly/functions/Complexity.h>
#include "functions/Visit.h"

namespace carl
{

    template<typename Pol>
    void Formula<Pol>::init( FormulaContent<Pol>& _content )
    {
        _content.mProperties = Condition();
        switch( _content.mType )
        {
            case FormulaType::EXISTS:
            {
                ///@todo do something here
                break;
            }
            case FormulaType::FORALL:
            {
                ///@todo do something here
                break;
            }
            case FormulaType::TRUE:
            {
                _content.mProperties |= STRONG_CONDITIONS;
                if (std::holds_alternative<carl::Variable>(_content.mContent)) {
                    std::cout <<"Variable" << std::endl;
                } else if (std::holds_alternative<Constraint<Pol>>(_content.mContent)) {
                    std::cout <<"Constraint<Pol>" << std::endl;
                } else if (std::holds_alternative<VariableComparison<Pol>>(_content.mContent)) {
                    std::cout <<"VariableComparison<Pol>" << std::endl;
                }else if (std::holds_alternative<VariableAssignment<Pol>>(_content.mContent)) {
                    std::cout <<"VariableAssignment<Pol>" << std::endl;
                }else if (std::holds_alternative<BVConstraint>(_content.mContent)) {
                    std::cout <<"BVConstraint" << std::endl;
                }else if (std::holds_alternative<UEquality>(_content.mContent)) {
                    std::cout <<"UEquality" << std::endl;
                }else if (std::holds_alternative<Formula<Pol>>(_content.mContent)) {
                    std::cout <<"Formula<Pol>" << std::endl;
                }else if (std::holds_alternative<Formulas<Pol>>(_content.mContent)) {
                    std::cout <<"Formulas<Pol>" << std::endl;
                }else if (std::holds_alternative<QuantifierContent<Pol>>(_content.mContent)) {
                    std::cout <<"QuantifierContent<Pol>" << std::endl;
                }else {
                    std::cout << "err" << std::endl;
                }
		        addConstraintProperties( std::get<Constraint<Pol>>(_content.mContent), _content.mProperties );
		        break;
            }
            case FormulaType::FALSE:
            {
                _content.mProperties |= STRONG_CONDITIONS;
		        addConstraintProperties( std::get<Constraint<Pol>>(_content.mContent), _content.mProperties );
                break;
            }
            case FormulaType::BOOL:
            {
                _content.mProperties |= STRONG_CONDITIONS | PROP_CONTAINS_BOOLEAN;
                break;
            }
            case FormulaType::NOT:
            {
                Condition subFormulaConds = std::get<Formula<Pol>>(_content.mContent).mpContent->mProperties;
                if( PROP_IS_AN_ATOM <= subFormulaConds )
                    _content.mProperties |= PROP_IS_A_CLAUSE | PROP_IS_A_LITERAL | PROP_IS_IN_CNF | PROP_IS_LITERAL_CONJUNCTION;
                _content.mProperties |= (subFormulaConds & WEAK_CONDITIONS);
                break;
            }
            case FormulaType::OR:
            {
                _content.mProperties |= PROP_IS_A_CLAUSE | PROP_IS_IN_CNF | PROP_IS_IN_NNF;
				for (auto subFormula = std::get<Formulas<Pol>>(_content.mContent).begin(); subFormula != std::get<Formulas<Pol>>(_content.mContent).end(); ++subFormula)
				{
					Condition subFormulaConds = subFormula->properties();
                    if( !(PROP_IS_A_LITERAL<=subFormulaConds) )
                    {
                        _content.mProperties &= ~PROP_IS_A_CLAUSE;
                        _content.mProperties &= ~PROP_IS_IN_CNF;
                    }
                    if( !(PROP_IS_IN_NNF<=subFormulaConds) )
                        _content.mProperties &= ~PROP_IS_IN_NNF;
                    _content.mProperties |= (subFormulaConds & WEAK_CONDITIONS);
                }
                break;
            }
            case FormulaType::AND:
            {
                _content.mProperties |= PROP_IS_LITERAL_CONJUNCTION | PROP_IS_PURE_CONJUNCTION | PROP_IS_IN_CNF | PROP_IS_IN_NNF;
		        for (auto subFormula = std::get<Formulas<Pol>>(_content.mContent).begin(); subFormula != std::get<Formulas<Pol>>(_content.mContent).end(); ++subFormula)
                {
                    Condition subFormulaConds = subFormula->properties();
                    if( !(PROP_IS_A_CLAUSE<=subFormulaConds) )
                    {
                        _content.mProperties &= ~PROP_IS_PURE_CONJUNCTION;
                        _content.mProperties &= ~PROP_IS_LITERAL_CONJUNCTION;
                        _content.mProperties &= ~PROP_IS_IN_CNF;
                    }
                    else if( !(PROP_IS_A_LITERAL<=subFormulaConds) )
                    {
                        _content.mProperties &= ~PROP_IS_PURE_CONJUNCTION;
                        _content.mProperties &= ~PROP_IS_LITERAL_CONJUNCTION;
                    }
                    else if( !(PROP_IS_AN_ATOM <=subFormulaConds) )
                        _content.mProperties &= ~PROP_IS_PURE_CONJUNCTION;
                    if( !(PROP_IS_IN_NNF<=subFormulaConds) )
                        _content.mProperties &= ~PROP_IS_IN_NNF;
                    _content.mProperties |= (subFormulaConds & WEAK_CONDITIONS);
                }
                break;
            }
            case FormulaType::ITE:
            case FormulaType::IMPLIES:
            case FormulaType::IFF:
            case FormulaType::XOR:
            {
                _content.mProperties |= PROP_IS_IN_NNF;
				for (auto subFormula = std::get<Formulas<Pol>>(_content.mContent).begin(); subFormula != std::get<Formulas<Pol>>(_content.mContent).end(); ++subFormula)
                {
                    Condition subFormulaConds = subFormula->properties();
                    if( !(PROP_IS_IN_NNF<=subFormulaConds) )
                        _content.mProperties &= ~PROP_IS_IN_NNF;
                    _content.mProperties |= (subFormulaConds & WEAK_CONDITIONS);
                }
                break;
            }
            case FormulaType::CONSTRAINT:
            {
                _content.mProperties |= STRONG_CONDITIONS;
				addConstraintProperties(std::get<Constraint<Pol>>(_content.mContent), _content.mProperties);
                break;
            }
			case FormulaType::VARCOMPARE:
			{
				_content.mProperties |= STRONG_CONDITIONS | PROP_CONTAINS_REAL_VALUED_VARS | PROP_CONTAINS_INTEGER_VALUED_VARS;
				break;
			}
			case FormulaType::VARASSIGN:
			{
				_content.mProperties |= STRONG_CONDITIONS | PROP_CONTAINS_REAL_VALUED_VARS | PROP_CONTAINS_INTEGER_VALUED_VARS;
				break;
			}
            case FormulaType::BITVECTOR:
            {
                _content.mProperties |= STRONG_CONDITIONS | PROP_CONTAINS_BITVECTOR;
                break;
            }
            case FormulaType::UEQ:
            {
                _content.mProperties |= STRONG_CONDITIONS | PROP_CONTAINS_UNINTERPRETED_EQUATIONS;
                break;
            }
            default:
            {
				CARL_LOG_ERROR("carl.formula", "Undefined formula type " << _content.mType);
                assert( false );
            }
        }
    }

    template<typename Pol>
    void Formula<Pol>::addConstraintProperties( const Constraint<Pol>& _constraint, Condition& _properties )
    {
        if( carl::is_zero(_constraint.lhs()) )
        {
            _properties |= PROP_CONTAINS_LINEAR_POLYNOMIAL;
        }
        else
        {
            switch( _constraint.lhs().total_degree() )
            {
                case 0:
                    _properties |= PROP_CONTAINS_LINEAR_POLYNOMIAL;
                    break;
                case 1:
                    _properties |= PROP_CONTAINS_LINEAR_POLYNOMIAL;
                    break;
                case 2:
                    _properties |= PROP_CONTAINS_NONLINEAR_POLYNOMIAL;
                    break;
                case 3:
                    _properties |= PROP_CONTAINS_NONLINEAR_POLYNOMIAL;
                    _properties |= PROP_VARIABLE_DEGREE_GREATER_THAN_TWO;
                    break;
                case 4:
                    _properties |= PROP_CONTAINS_NONLINEAR_POLYNOMIAL;
                    _properties |= PROP_VARIABLE_DEGREE_GREATER_THAN_THREE;
                    break;
                default:
                    _properties |= PROP_CONTAINS_NONLINEAR_POLYNOMIAL;
                    _properties |= PROP_VARIABLE_DEGREE_GREATER_THAN_FOUR;
                    break;
            }
        }
        switch( _constraint.relation() )
        {
            case Relation::EQ:
                _properties |= PROP_CONTAINS_EQUATION;
                _properties |= PROP_CONTAINS_WEAK_INEQUALITY;
                break;
            case Relation::NEQ:
                _properties |= PROP_CONTAINS_STRICT_INEQUALITY;
                break;
            case Relation::LEQ:
                _properties |= PROP_CONTAINS_INEQUALITY;
                _properties |= PROP_CONTAINS_WEAK_INEQUALITY;
                break;
            case Relation::GEQ:
                _properties |= PROP_CONTAINS_INEQUALITY;
                _properties |= PROP_CONTAINS_WEAK_INEQUALITY;
                break;
            case Relation::LESS:
                _properties |= PROP_CONTAINS_STRICT_INEQUALITY;
                break;
            case Relation::GREATER:
                _properties |= PROP_CONTAINS_STRICT_INEQUALITY;
                break;
            default:
            {
            }
        }
        if( _constraint.hasIntegerValuedVariable() )
            _properties |= PROP_CONTAINS_INTEGER_VALUED_VARS;
        if( _constraint.hasRealValuedVariable() )
            _properties |= PROP_CONTAINS_REAL_VALUED_VARS;
        if( _constraint.isPseudoBoolean() )
            _properties |= PROP_CONTAINS_PSEUDOBOOLEAN;
    }
   

}    // namespace carl
