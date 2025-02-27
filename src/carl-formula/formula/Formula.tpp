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
                _content.mProperties |= PROP_CONTAINS_QUANTIFIER_EXISTS;
				auto subFormula = std::get<QuantifierContent<Pol>>(_content.mContent).mFormula;
                _content.mProperties |= (subFormula.properties() & WEAK_CONDITIONS);
                _content.mProperties |= (subFormula.properties() & PROP_IS_IN_PNF);
                break;
            }
            case FormulaType::AUX_EXISTS:
            {
                _content.mProperties |= PROP_CONTAINS_QUANTIFIER_EXISTS;
				auto subFormula = std::get<AuxQuantifierContent<Pol>>(_content.mContent).mFormula;
                _content.mProperties |= (subFormula.properties() & WEAK_CONDITIONS);
                _content.mProperties |= (subFormula.properties() & ~PROP_IS_IN_PNF);
                break;
            }
            case FormulaType::FORALL:
            {
                _content.mProperties |= PROP_CONTAINS_QUANTIFIER_FORALL;
				auto subFormula = std::get<QuantifierContent<Pol>>(_content.mContent).mFormula;
                _content.mProperties |= (subFormula.properties() & WEAK_CONDITIONS);
                _content.mProperties |= (subFormula.properties() & PROP_IS_IN_PNF);
                break;
            }
            case FormulaType::TRUE:
            {
                _content.mProperties |= STRONG_CONDITIONS;
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
                if (!(PROP_CONTAINS_QUANTIFIER_EXISTS <= subFormulaConds) && !(PROP_CONTAINS_QUANTIFIER_FORALL <= subFormulaConds))
                    _content.mProperties |= PROP_IS_IN_PNF;
                break;
            }
            case FormulaType::OR:
            {
                _content.mProperties |= PROP_IS_A_CLAUSE | PROP_IS_IN_CNF | PROP_IS_IN_NNF | PROP_IS_IN_PNF;
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
                    if (PROP_CONTAINS_QUANTIFIER_EXISTS <= subFormulaConds || PROP_CONTAINS_QUANTIFIER_FORALL <= subFormulaConds)
                        _content.mProperties &= ~PROP_IS_IN_PNF;
                }
                break;
            }
            case FormulaType::AND:
            {
                _content.mProperties |= PROP_IS_LITERAL_CONJUNCTION | PROP_IS_PURE_CONJUNCTION | PROP_IS_IN_CNF | PROP_IS_IN_NNF | PROP_IS_IN_PNF;
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
                    if (PROP_CONTAINS_QUANTIFIER_EXISTS <= subFormulaConds || PROP_CONTAINS_QUANTIFIER_FORALL <= subFormulaConds)
                        _content.mProperties &= ~PROP_IS_IN_PNF;
                }
                break;
            }
            case FormulaType::ITE:
            case FormulaType::IMPLIES:
            case FormulaType::IFF:
            case FormulaType::XOR:
            {
                _content.mProperties |= PROP_IS_IN_NNF | PROP_IS_IN_PNF;
				for (auto subFormula = std::get<Formulas<Pol>>(_content.mContent).begin(); subFormula != std::get<Formulas<Pol>>(_content.mContent).end(); ++subFormula)
                {
                    Condition subFormulaConds = subFormula->properties();
                    if( !(PROP_IS_IN_NNF<=subFormulaConds) )
                        _content.mProperties &= ~PROP_IS_IN_NNF;
                    _content.mProperties |= (subFormulaConds & WEAK_CONDITIONS);
                    if (PROP_CONTAINS_QUANTIFIER_EXISTS <= subFormulaConds || PROP_CONTAINS_QUANTIFIER_FORALL <= subFormulaConds)
                        _content.mProperties &= ~PROP_IS_IN_PNF;
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
				_content.mProperties |= STRONG_CONDITIONS; //| PROP_CONTAINS_REAL_VALUED_VARS | PROP_CONTAINS_INTEGER_VALUED_VARS;
                _content.mProperties |= PROP_CONTAINS_NONLINEAR_POLYNOMIAL;
                _content.mProperties |= PROP_CONTAINS_ROOT_EXPRESSION;
				break;
			}
			case FormulaType::VARASSIGN:
			{
				_content.mProperties |= STRONG_CONDITIONS; //| PROP_CONTAINS_REAL_VALUED_VARS | PROP_CONTAINS_INTEGER_VALUED_VARS;
                _content.mProperties |= PROP_CONTAINS_ROOT_EXPRESSION;
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
