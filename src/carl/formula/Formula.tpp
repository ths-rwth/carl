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
#include "../core/polynomialfunctions/Complexity.h"

namespace carl
{
	template<typename Pol>
	void Formula<Pol>::gatherVariables(carlVariables& vars) const {
		FormulaVisitor<Formula<Pol>> visitor;
		visitor.visit(*this,
			[&vars](const Formula<Pol>& f) {
				switch (f.getType()) {
					case FormulaType::BOOL:
						vars.add(f.boolean());
						break;
					case FormulaType::CONSTRAINT:
						f.constraint().gatherVariables(vars);
						break;
					case FormulaType::VARCOMPARE:
						f.variableComparison().gatherVariables(vars);
						break;
					case FormulaType::VARASSIGN:
						f.variableAssignment().gatherVariables(vars);
						break;
					case FormulaType::BITVECTOR:
						f.bvConstraint().gatherVariables(vars);
						break;
					case FormulaType::UEQ:
						f.uequality().gatherVariables(vars);
						break;
					case FormulaType::EXISTS:
					case FormulaType::FORALL:
						f.quantifiedFormula().gatherVariables(vars);
						break;
					default: break;
				}
			}
		);
	}

    template<typename Pol>
    void Formula<Pol>::gatherUFs(std::set<UninterpretedFunction>& ufs) const {
		FormulaVisitor<Formula<Pol>> visitor;
		visitor.visit(*this,
			[&ufs](const Formula<Pol>& f) {
				if (f.getType() == FormulaType::UEQ) {
					f.uequality().gatherUFs(ufs);
				}
			}
		);
    }

    template<typename Pol>
    void Formula<Pol>::gatherUVariables(std::set<UVariable>& uvs) const {
		FormulaVisitor<Formula<Pol>> visitor;
		visitor.visit(*this,
			[&uvs](const Formula<Pol>& f) {
				if (f.getType() == FormulaType::UEQ) {
					f.uequality().gatherUVariables(uvs);
				}
			}
		);
    }

    template<typename Pol>
    void Formula<Pol>::gatherBVVariables(std::set<BVVariable>& bvvs) const {
		FormulaVisitor<Formula<Pol>> visitor;
		visitor.visit(*this,
			[&bvvs](const Formula<Pol>& f) {
				if (f.getType() == FormulaType::BITVECTOR) {
					f.bvConstraint().gatherBVVariables(bvvs);
				}
			}
		);
    }

    template<typename Pol>
    size_t Formula<Pol>::complexity() const
    {
        size_t result = 0;
        carl::FormulaVisitor<Formula<Pol>> visitor;
        visitor.visit(*this,
            [&](const Formula& _f)
            {
                switch( _f.getType() )
                {
                    case FormulaType::TRUE:
                    case FormulaType::FALSE:
                        break;
                    case FormulaType::CONSTRAINT:
                        result += carl::complexity(_f.constraint()); break;
                    case FormulaType::BITVECTOR:
                        result += _f.bvConstraint().complexity(); break;
                    case FormulaType::UEQ:
                        result += _f.uequality().complexity(); break;
                    default:
                        ++result;
                }
            });
        return result;
    }

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
        if( carl::isZero(_constraint.lhs()) )
        {
            _properties |= PROP_CONTAINS_LINEAR_POLYNOMIAL;
        }
        else
        {
            switch( _constraint.lhs().totalDegree() )
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

    template<typename Pol>
    void Formula<Pol>::printProposition( std::ostream& _out, const std::string _init ) const
    {
        _out << _init;
        for( unsigned i = 0; i < properties().size(); ++i )
        {
            if( fmod( i, 10.0 ) == 0.0 )
                _out << " ";
            _out << properties()[i];
        }
        _out << std::endl;
    }

    template<typename Pol>
    Formula<Pol> Formula<Pol>::resolveNegation( bool _keepConstraint ) const
    {
        if( getType() != FormulaType::NOT ) return *this;
        FormulaType newType = getType();
        switch( subformula().getType() )
        {
            case FormulaType::BOOL:
                return *this;
            case FormulaType::UEQ:
                if( _keepConstraint )
                    return *this;
                else
                {
                    const UEquality& ueq = subformula().uequality();
                    return Formula<Pol>( ueq.lhs(), ueq.rhs(), !ueq.negated() );
                }
            case FormulaType::CONSTRAINT:
            {
                const Constraint<Pol>& constraint = subformula().constraint();
                if( _keepConstraint )
                    return *this;
                else
                {
                    switch( constraint.relation() )
                    {
                        case Relation::EQ:
                        {
                            return Formula<Pol>( constraint.lhs(), Relation::NEQ );
                        }
                        case Relation::LEQ:
                        {
                            return Formula<Pol>( -constraint.lhs(), Relation::LESS );
                        }
                        case Relation::LESS:
                        {
                            return Formula<Pol>( -constraint.lhs(), Relation::LEQ );
                        }
                        case Relation::GEQ:
                        {
                            return Formula<Pol>( constraint.lhs(), Relation::LESS );
                        }
                        case Relation::GREATER:
                        {
                            return Formula<Pol>( constraint.lhs(), Relation::LEQ );
                        }
                        case Relation::NEQ:
                        {
                            return Formula<Pol>( constraint.lhs(), Relation::EQ );
                        }
                        default:
                        {
                            assert( false );
                            std::cerr << "Unexpected relation symbol!" << std::endl;
                            return *this;
                        }
                    }
                }
            }
			case FormulaType::VARCOMPARE: {
				return Formula<Pol>(subformula().variableComparison().negation());
			}
			case FormulaType::VARASSIGN: {
				assert(false);
				return Formula<Pol>();
			}
            case FormulaType::BITVECTOR: {
                BVCompareRelation rel = inverse(subformula().bvConstraint().relation());
                return Formula<Pol>( BVConstraint::create(rel, subformula().bvConstraint().lhs(), subformula().bvConstraint().rhs()));
            }
            case FormulaType::TRUE: // (not true)  ->  false
                return Formula<Pol>( FormulaType::FALSE );
            case FormulaType::FALSE: // (not false)  ->  true
                return Formula<Pol>( FormulaType::TRUE );
            case FormulaType::NOT: // (not (not phi))  ->  phi
                return subformula().subformula();
            case FormulaType::IMPLIES:
            {
                assert( subformula().size() == 2 );
                // (not (implies lhs rhs))  ->  (and lhs (not rhs))
                Formulas<Pol> subFormulas;
                subFormulas.push_back( subformula().premise() );
                subFormulas.push_back( Formula<Pol>( NOT, subformula().conclusion() ) );
                return Formula<Pol>( AND, move( subFormulas ) );
            }
            case FormulaType::ITE: // (not (ite cond then else))  ->  (ite cond (not then) (not else))
            {
                return Formula<Pol>( ITE, {subformula().condition(), Formula<Pol>( NOT, subformula().firstCase() ), Formula<Pol>( NOT, subformula().secondCase() )} );
            }
            case FormulaType::IFF: // (not (iff phi_1 .. phi_n))  ->  (and (or phi_1 .. phi_n) (or (not phi_1) .. (not phi_n)))
            {
                Formulas<Pol> subFormulasA;
                Formulas<Pol> subFormulasB;
                for( auto& subFormula : subformula().subformulas() )
                {
                    subFormulasA.push_back( subFormula );
                    subFormulasB.push_back( Formula<Pol>( NOT, subFormula ) );
                }
                return Formula<Pol>( AND, {Formula<Pol>( OR, move( subFormulasA ) ), Formula<Pol>( OR, move( subFormulasB ) )} );
            }
            case FormulaType::XOR: // (not (xor phi_1 .. phi_n))  ->  (xor (not phi_1) phi_2 .. phi_n)
            {
                auto subFormula = subformula().subformulas().begin();
                Formulas<Pol> subFormulas;
                subFormulas.push_back( Formula<Pol>( NOT, *subFormula ) );
                ++subFormula;
                for( ; subFormula != subformula().subformulas().end(); ++subFormula )
                    subFormulas.push_back( *subFormula );
                return Formula<Pol>( XOR, move( subFormulas ) );
            }
            case FormulaType::AND: // (not (and phi_1 .. phi_n))  ->  (or (not phi_1) .. (not phi_n))
                newType = FormulaType::OR;
                break;
            case FormulaType::OR: // (not (or phi_1 .. phi_n))  ->  (and (not phi_1) .. (not phi_n))
                newType = FormulaType::AND;
                break;
            case FormulaType::EXISTS: // (not (exists (vars) phi)) -> (forall (vars) (not phi))
                newType = FormulaType::FORALL;
                break;
            case FormulaType::FORALL: // (not (forall (vars) phi)) -> (exists (vars) (not phi))
                newType = FormulaType::EXISTS;
                break;
            default:
                assert( false );
                std::cerr << "Unexpected type of formula!" << std::endl;
                return *this;
        }
        assert( newType != subformula().getType() );
        assert( subformula().getType() == FormulaType::AND || subformula().getType() == FormulaType::OR );
        Formulas<Pol> subFormulas;
        for( const Formula<Pol>& subsubformula : subformula().subformulas() )
            subFormulas.push_back( Formula<Pol>( FormulaType::NOT, subsubformula ) );
        return Formula<Pol>( newType, move( subFormulas ) );
    }

    template<typename Pol>
    Formula<Pol> Formula<Pol>::connectPrecedingSubformulas() const
    {
        assert( isNary() );
        if( subformulas().size() > 2 )
        {
            Formulas<Pol> tmpSubformulas;
            auto iter = subformulas().rbegin();
            ++iter;
            for( ; iter != subformulas().rend(); ++iter )
                tmpSubformulas.push_back( *iter );
            return Formula<Pol>( getType(), tmpSubformulas );
        }
        else
        {
            assert( subformulas().size() == 2 );
            return *(subformulas().begin());
        }
    }

    template<typename Pol>
    Formula<Pol> Formula<Pol>::substitute( const std::map<Variable, Formula<Pol>>& _booleanSubstitutions, const std::map<Variable, Pol>& _arithmeticSubstitutions ) const
    {
        switch( getType() )
        {
            case FormulaType::TRUE:
            {
                return *this;
            }
            case FormulaType::FALSE:
            {
                return *this;
            }
            case FormulaType::BOOL:
            {
                auto iter = _booleanSubstitutions.find( boolean() );
                if( iter != _booleanSubstitutions.end() )
                {
                    return iter->second;
                }
                return *this;
            }
            case FormulaType::CONSTRAINT:
            {
                Pol lhsSubstituted = carl::substitute(constraint().lhs(), _arithmeticSubstitutions);
                return Formula<Pol>( lhsSubstituted, constraint().relation() );
            }
            case FormulaType::NOT:
            {
                return Formula<Pol>( FormulaType::NOT, subformula().substitute( _booleanSubstitutions, _arithmeticSubstitutions ) );
            }
            case FormulaType::IMPLIES:
            {
                Formula<Pol> premiseSubstituted = premise().substitute( _booleanSubstitutions, _arithmeticSubstitutions );
                Formula<Pol> conclusionSubstituted = conclusion().substitute( _booleanSubstitutions, _arithmeticSubstitutions );
                return Formula<Pol>( FormulaType::IMPLIES, {premiseSubstituted, conclusionSubstituted} );
            }
            case FormulaType::ITE:
            {
                Formula<Pol> conditionSubstituted = condition().substitute( _booleanSubstitutions, _arithmeticSubstitutions );
                Formula<Pol> thenSubstituted = firstCase().substitute( _booleanSubstitutions, _arithmeticSubstitutions );
                Formula<Pol> elseSubstituted = secondCase().substitute( _booleanSubstitutions, _arithmeticSubstitutions );
                return Formula<Pol>( FormulaType::ITE, {conditionSubstituted, thenSubstituted, elseSubstituted} );
            }
            case FormulaType::EXISTS:
            case FormulaType::FORALL:
            {
                std::vector<Variable> vars( quantifiedVariables() );
                return Formula<Pol>(getType(), std::move( vars ), quantifiedFormula().substitute(_booleanSubstitutions, _arithmeticSubstitutions));
            }
            default:
            {
                assert( isNary() );
                Formulas<Pol> subformulasSubstituted;
                for( const Formula<Pol>& subFormula : subformulas() )
                    subformulasSubstituted.push_back( subFormula.substitute( _booleanSubstitutions, _arithmeticSubstitutions ) );
                return Formula<Pol>( getType(), subformulasSubstituted );
            }
        }
    }

//    #define CONSTRAINT_BOUND_DEBUG

    template<typename Pol>
    Formula<Pol> Formula<Pol>::addConstraintBound( ConstraintBounds& _constraintBounds, const Formula<Pol>& _constraint, bool _inConjunction )
    {
        #ifdef CONSTRAINT_BOUND_DEBUG
        std::cout << "add from a " << (_inConjunction ? "conjunction" : "disjunction") << " to " << &_constraintBounds << ":   " << _constraint << std::endl;
        #endif
        bool negated = _constraint.getType() == FormulaType::NOT;
        assert( _constraint.getType() == FormulaType::CONSTRAINT || (negated && _constraint.subformula().getType() == FormulaType::CONSTRAINT ) );
        const Constraint<Pol>& constraint = negated ? _constraint.subformula().constraint() : _constraint.constraint();
        assert( constraint.isConsistent() == 2 );
        typename Pol::NumberType boundValue;
        Relation relation = negated ? carl::inverse( constraint.relation() ) : constraint.relation();
        const Pol& lhs = constraint.lhs();
        Pol poly;
        bool multipliedByMinusOne = lhs.lterm().coeff() < typename Pol::NumberType( 0 );
        if( multipliedByMinusOne )
        {
            boundValue = constraint.constantPart();
            relation = carl::turn_around( relation );
            poly = Pol( -lhs + boundValue );
        }
        else
        {
            boundValue = -constraint.constantPart();
            poly = Pol( lhs + boundValue );
        }
        typename Pol::NumberType cf( poly.coprimeFactor() );
        assert( cf > 0 );
        boundValue *= cf;
        poly *= cf;
        #ifdef CONSTRAINT_BOUND_DEBUG
        std::cout << "try to add the bound  " << relation << boundValue << "  for the polynomial  " << poly << std::endl;
        #endif
        auto resA = _constraintBounds.insert( std::make_pair( std::move(poly), std::move( std::map<typename Pol::NumberType, std::pair<Relation, Formula<Pol>>>() ) ) );
        auto resB = resA.first->second.insert( std::make_pair( boundValue, std::make_pair( relation, _constraint ) ) );
        if( resB.second || resB.first->second.first == relation )
            return resB.first->second.second;
        switch( relation )
        {
            case Relation::EQ:
                if( _inConjunction )
                {
                    if( resB.first->second.first == Relation::LEQ || resB.first->second.first == Relation::GEQ )
                    {
                        resB.first->second.first = Relation::EQ;
                        resB.first->second.second = _constraint;
                        return resB.first->second.second;
                    }
                    else
                        return Formula( FormulaType::FALSE );
                }
                else
                {
                    switch( resB.first->second.first )
                    {
                        case Relation::LEQ:
                            return resB.first->second.second;
                        case Relation::GEQ:
                            return resB.first->second.second;
                        case Relation::LESS:
                            resB.first->second.first = Relation::LEQ;
                            resB.first->second.second = Formula<Pol>( lhs, multipliedByMinusOne ? Relation::GEQ : Relation::LEQ );
                            return resB.first->second.second;
                        case Relation::GREATER:
                            resB.first->second.first = Relation::GEQ;
                            resB.first->second.second = Formula<Pol>( lhs, multipliedByMinusOne ? Relation::LEQ : Relation::GEQ );
                            return resB.first->second.second;
                        default:
                            assert( resB.first->second.first == Relation::NEQ );
                            return Formula( FormulaType::FALSE );
                    }
                }
            case Relation::LEQ:
                if( _inConjunction )
                {
                    switch( resB.first->second.first )
                    {
                        case Relation::EQ:
                            return resB.first->second.second;
                        case Relation::GEQ:
                            resB.first->second.first = Relation::EQ;
                            resB.first->second.second = Formula<Pol>( lhs, Relation::EQ );
                            return resB.first->second.second;
                        case Relation::LESS:
                            return resB.first->second.second;
                        case Relation::GREATER:
                            return Formula( FormulaType::FALSE );
                        default:
                            assert( resB.first->second.first == Relation::NEQ );
                            resB.first->second.first = Relation::LESS;
                            resB.first->second.second = Formula<Pol>( lhs, multipliedByMinusOne ? Relation::GREATER : Relation::LESS );
                            return resB.first->second.second;
                    }
                }
                else
                {
                    switch( resB.first->second.first )
                    {
                        case Relation::EQ:
                            resB.first->second.first = Relation::LEQ;
                            resB.first->second.second = _constraint;
                            return resB.first->second.second;
                        case Relation::GEQ:
                            return Formula( FormulaType::FALSE );
                        case Relation::LESS:
                            resB.first->second.first = Relation::LEQ;
                            resB.first->second.second = _constraint;
                            return resB.first->second.second;
                        case Relation::GREATER:
                            return Formula( FormulaType::FALSE );
                        default:
                            assert( resB.first->second.first == Relation::NEQ );
                            return Formula( FormulaType::FALSE );
                    }
                }
            case Relation::GEQ:
                if( _inConjunction )
                {
                    switch( resB.first->second.first )
                    {
                        case Relation::EQ:
                            return resB.first->second.second;
                        case Relation::LEQ:
                            resB.first->second.first = Relation::EQ;
                            resB.first->second.second = Formula<Pol>( lhs, Relation::EQ );
                            return resB.first->second.second;
                        case Relation::LESS:
                            return Formula( FormulaType::FALSE );
                        case Relation::GREATER:
                            return resB.first->second.second;
                        default:
                            assert( resB.first->second.first == Relation::NEQ );
                            resB.first->second.first = Relation::GREATER;
                            resB.first->second.second = Formula<Pol>( lhs, multipliedByMinusOne ? Relation::LESS : Relation::GREATER );
                            return resB.first->second.second;
                    }
                }
                else
                {
                    switch( resB.first->second.first )
                    {
                        case Relation::EQ:
                            resB.first->second.first = Relation::GEQ;
                            resB.first->second.second = _constraint;
                            return resB.first->second.second;
                        case Relation::LEQ:
                            return Formula( FormulaType::FALSE );
                        case Relation::LESS:
                            return Formula( FormulaType::FALSE );
                        case Relation::GREATER:
                            resB.first->second.first = Relation::GEQ;
                            resB.first->second.second = _constraint;
                            return Formula( FormulaType::FALSE );
                        default:
                            assert( resB.first->second.first == Relation::NEQ );
                            return Formula( FormulaType::FALSE );
                    }
                }
            case Relation::LESS:
                if( _inConjunction )
                {
                    switch( resB.first->second.first )
                    {
                        case Relation::EQ:
                            return Formula( FormulaType::FALSE );
                        case Relation::LEQ:
                            resB.first->second.first = Relation::LESS;
                            resB.first->second.second = _constraint;
                            return resB.first->second.second;
                        case Relation::GEQ:
                            return Formula( FormulaType::FALSE );
                        case Relation::GREATER:
                            return Formula( FormulaType::FALSE );
                        default:
                            assert( resB.first->second.first == Relation::NEQ );
                            resB.first->second.first = Relation::LESS;
                            resB.first->second.second = _constraint;
                            return resB.first->second.second;
                    }
                }
                else
                {
                    switch( resB.first->second.first )
                    {
                        case Relation::EQ:
                            resB.first->second.first = Relation::LEQ;
                            resB.first->second.second = Formula<Pol>( lhs, multipliedByMinusOne ? Relation::GEQ : Relation::LEQ );
                            return resB.first->second.second;
                        case Relation::LEQ:
                            return resB.first->second.second;
                        case Relation::GEQ:
                            return Formula( FormulaType::FALSE );
                        case Relation::GREATER:
                            resB.first->second.first = Relation::NEQ;
                            resB.first->second.second = Formula<Pol>( lhs, Relation::NEQ );
                            return resB.first->second.second;
                        default:
                            assert( resB.first->second.first == Relation::NEQ );
                            return resB.first->second.second;
                    }
                }
            case Relation::GREATER:
                if( _inConjunction )
                {
                    switch( resB.first->second.first )
                    {
                        case Relation::EQ:
                            return Formula( FormulaType::FALSE );
                        case Relation::LEQ:
                            return Formula( FormulaType::FALSE );
                        case Relation::GEQ:
                            resB.first->second.first = Relation::GREATER;
                            resB.first->second.second = _constraint;
                            return resB.first->second.second;
                        case Relation::LESS:
                            return Formula( FormulaType::FALSE );
                        default:
                            assert( resB.first->second.first == Relation::NEQ );
                            resB.first->second.first = Relation::GREATER;
                            resB.first->second.second = _constraint;
                            return resB.first->second.second;
                    }
                }
                else
                {
                    switch( resB.first->second.first )
                    {
                        case Relation::EQ:
                            resB.first->second.first = Relation::GEQ;
                            resB.first->second.second = Formula<Pol>( lhs, multipliedByMinusOne ? Relation::LEQ : Relation::GEQ );
                            return resB.first->second.second;
                        case Relation::LEQ:
                            return Formula( FormulaType::FALSE );
                        case Relation::GEQ:
                            return resB.first->second.second;
                        case Relation::LESS:
                            resB.first->second.first = Relation::NEQ;
                            resB.first->second.second = Formula<Pol>( lhs, Relation::NEQ );
                            return resB.first->second.second;
                        default:
                            assert( resB.first->second.first == Relation::NEQ );
                            return resB.first->second.second;
                    }
                }
            default:
                assert( relation == Relation::NEQ );
                if( _inConjunction )
                {
                    switch( resB.first->second.first )
                    {
                        case Relation::EQ:
                            return Formula( FormulaType::FALSE );
                        case Relation::LEQ:
                            resB.first->second.first = Relation::LESS;
                            resB.first->second.second = Formula<Pol>( lhs, multipliedByMinusOne ? Relation::GREATER : Relation::LESS );
                            return resB.first->second.second;
                        case Relation::GEQ:
                            resB.first->second.first = Relation::GREATER;
                            resB.first->second.second = Formula<Pol>( lhs, multipliedByMinusOne ? Relation::LESS : Relation::GREATER );
                            return resB.first->second.second;
                        case Relation::LESS:
                            resB.first->second.first = Relation::LESS;
                            resB.first->second.second = Formula<Pol>( lhs, multipliedByMinusOne ? Relation::GREATER : Relation::LESS );
                            return resB.first->second.second;
                        default:
                            assert( resB.first->second.first == Relation::GREATER );
                            resB.first->second.first = Relation::GREATER;
                            resB.first->second.second = Formula<Pol>( lhs, multipliedByMinusOne ? Relation::LESS : Relation::GREATER );
                            return resB.first->second.second;
                    }
                }
                else
                {
                    switch( resB.first->second.first )
                    {
                        case Relation::EQ:
                            return Formula( FormulaType::FALSE );
                        case Relation::LEQ:
                            return Formula( FormulaType::FALSE );
                        case Relation::GEQ:
                            return Formula( FormulaType::FALSE );
                        case Relation::LESS:
                            return resB.first->second.second;
                        default:
                            assert( resB.first->second.first == Relation::GREATER );
                            return resB.first->second.second;
                    }
                }
        }
    }

    template<typename Pol>
    bool Formula<Pol>::swapConstraintBounds( ConstraintBounds& _constraintBounds, Formulas<Pol>& _intoFormulas, bool _inConjunction )
    {
        #ifdef CONSTRAINT_BOUND_DEBUG
        std::cout << "swap from " << &_constraintBounds << " to a " << (_inConjunction ? "conjunction" : "disjunction") << std::endl;
        #endif
        while( !_constraintBounds.empty() )
        {
            #ifdef CONSTRAINT_BOUND_DEBUG
            std::cout << "for the bounds of  " << _constraintBounds.begin()->first << std::endl;
            #endif
            const std::map<typename Pol::NumberType, std::pair<Relation, Formula<Pol>>>& bounds = _constraintBounds.begin()->second;
            assert( !bounds.empty() );
            if( bounds.size() == 1 )
            {
                _intoFormulas.push_back( bounds.begin()->second.second );
                #ifdef CONSTRAINT_BOUND_DEBUG
                std::cout << "   just add the only bound" << std::endl;
                #endif
            }
            else
            {
                auto mostSignificantLowerBound = bounds.end();
                auto mostSignificantUpperBound = bounds.end();
                auto moreSignificantCase = bounds.end();
                Formulas<Pol> lessSignificantCases;
                auto iter = bounds.begin();
                for( ; iter != bounds.end(); ++iter )
                {
                    #ifdef CONSTRAINT_BOUND_DEBUG
                    std::cout << "   bound is  " << iter->second.first << iter->first << std::endl;
                    #endif
                    if( (_inConjunction && iter->second.first == Relation::NEQ)
                        || (!_inConjunction && iter->second.first == Relation::EQ) )
                    {
                        if( moreSignificantCase == bounds.end() )
                        {
                            if( (_inConjunction && mostSignificantUpperBound == bounds.end())
                                || (!_inConjunction && mostSignificantLowerBound == bounds.end()) )
                            {
                                if( (_inConjunction && mostSignificantLowerBound != bounds.end())
                                    || (!_inConjunction && mostSignificantUpperBound != bounds.end()) )
                                {
                                    #ifdef CONSTRAINT_BOUND_DEBUG
                                    std::cout << "      case: " << __LINE__ << std::endl;
                                    #endif
                                    _intoFormulas.push_back( iter->second.second );
                                }
                                else
                                {
                                    #ifdef CONSTRAINT_BOUND_DEBUG
                                    std::cout << "      case: " << __LINE__ << std::endl;
                                    #endif
                                    lessSignificantCases.push_back( iter->second.second );
                                }
                            }
                        }
                    }
                    else if( (_inConjunction && (iter->second.first == Relation::GEQ || iter->second.first == Relation::GREATER)) // found a lower bound
                             || (!_inConjunction && (iter->second.first == Relation::LEQ || iter->second.first == Relation::LESS)) ) // found an upper bound
                    {
                        if( (_inConjunction && mostSignificantUpperBound != bounds.end()) // found already an upper bound -> conjunction is invalid!
                            || (!_inConjunction && mostSignificantLowerBound != bounds.end()) // found already a lower bound -> disjunction is valid!
                            || moreSignificantCase != bounds.end() )
                        {
                            #ifdef CONSTRAINT_BOUND_DEBUG
                            std::cout << "      case: " << __LINE__ << std::endl;
                            #endif
                            break;
                        }
                        else
                        {
                            #ifdef CONSTRAINT_BOUND_DEBUG
                            std::cout << "      case: " << __LINE__ << std::endl;
                            #endif
                            if( _inConjunction ) // update the strongest upper bound
                                mostSignificantLowerBound = iter;
                            else // update the weakest upper bound
                                mostSignificantUpperBound = iter;
                            lessSignificantCases.clear();
                        }
                    }
                    else if( (_inConjunction && iter->second.first == Relation::EQ)
                            || (!_inConjunction && iter->second.first == Relation::NEQ) )
                    {
                        // _inConjunction == true: found already another equality or an upper bound -> conjunction is invalid!
                        // _inConjunction == false: found already another bound with != as relation or a lower bound -> disjunction is valid!
                        if( moreSignificantCase != bounds.end() || (_inConjunction ? mostSignificantUpperBound : mostSignificantLowerBound) != bounds.end() )
                        {
                            #ifdef CONSTRAINT_BOUND_DEBUG
                            std::cout << "      case: " << __LINE__ << std::endl;
                            #endif
                            break;
                        }
                        // _inConjunction == true: found first equality
                        // _inConjunction == false: found first bound with !=
                        else
                        {
                            #ifdef CONSTRAINT_BOUND_DEBUG
                            std::cout << "      case: " << __LINE__ << std::endl;
                            #endif
                            moreSignificantCase = iter;
                        }
                    }
                    // _inConjunction == true: found an upper bound
                    // _inConjunction == false: found a lower bound
                    else
                    {
                        #ifdef CONSTRAINT_BOUND_DEBUG
                        std::cout << "      case: " << __LINE__ << std::endl;
                        #endif
                        assert( !_inConjunction || iter->second.first == Relation::LEQ || iter->second.first == Relation::LESS );
                        assert( _inConjunction || iter->second.first == Relation::GEQ || iter->second.first == Relation::GREATER );
                        if( _inConjunction && mostSignificantUpperBound == bounds.end() ) // first upper bound found = strongest upper bound
                        {
                            #ifdef CONSTRAINT_BOUND_DEBUG
                            std::cout << "      case: " << __LINE__ << std::endl;
                            #endif
                            mostSignificantUpperBound = iter;
                        }
                        else if( !_inConjunction && mostSignificantLowerBound == bounds.end() ) // first lower bound found = weakest lower bound
                        {
                            #ifdef CONSTRAINT_BOUND_DEBUG
                            std::cout << "      case: " << __LINE__ << std::endl;
                            #endif
                            mostSignificantLowerBound = iter;
                        }
                    }
                }
                if( iter != bounds.end() )
                    break;
                if( moreSignificantCase != bounds.end() )
                {
                    _intoFormulas.push_back( moreSignificantCase->second.second );
                }
                else
                {
                    #ifdef CONSTRAINT_BOUND_DEBUG
                    if( !(_inConjunction || mostSignificantUpperBound == bounds.end() || mostSignificantLowerBound == bounds.end()
                            || mostSignificantUpperBound->first > mostSignificantLowerBound->first)
                        || !( !_inConjunction || mostSignificantUpperBound == bounds.end() || mostSignificantLowerBound == bounds.end()
                             || mostSignificantLowerBound->first > mostSignificantUpperBound->first ) )
                    {
                        std::cout << "mostSignificantUpperBound:   " << mostSignificantUpperBound->first << "  [" << mostSignificantUpperBound->second.second << "]" << std::endl;
                        std::cout << "mostSignificantLowerBound:   " << mostSignificantLowerBound->first << "  [" << mostSignificantLowerBound->second.second << "]" << std::endl;
                    }
                    #endif
                    assert( !_inConjunction || mostSignificantUpperBound == bounds.end() || mostSignificantLowerBound == bounds.end()
                            || mostSignificantUpperBound->first > mostSignificantLowerBound->first );
                    assert( _inConjunction || mostSignificantUpperBound == bounds.end() || mostSignificantLowerBound == bounds.end()
                             || mostSignificantLowerBound->first > mostSignificantUpperBound->first );
                    if( mostSignificantUpperBound != bounds.end() )
                        _intoFormulas.push_back( mostSignificantUpperBound->second.second );
                    if( mostSignificantLowerBound != bounds.end() )
                        _intoFormulas.push_back( mostSignificantLowerBound->second.second );
                    _intoFormulas.insert(_intoFormulas.end(), lessSignificantCases.begin(), lessSignificantCases.end() );
                }
            }
            _constraintBounds.erase( _constraintBounds.begin() );
        }
        if( _constraintBounds.empty() )
        {
            #ifdef CONSTRAINT_BOUND_DEBUG
            std::cout << std::endl;
            #endif
            return false;
        }
        else
        {
            _constraintBounds.clear();
            #ifdef CONSTRAINT_BOUND_DEBUG
            std::cout << "is " << (_inConjunction ? "invalid" : "valid") << std::endl << std::endl;
            #endif
            return true;
        }
    }
}    // namespace carl
