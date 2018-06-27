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
#include "ConstraintPool.h"

using namespace std;

namespace carl
{
    template<typename Pol>
    void Formula<Pol>::collectVariables( Variables& _vars, bool _booleanVars, bool _realVars, bool _integerVars, bool _uninterpretedVars, bool _bitvectorVars ) const
    {
        std::set<BVVariable>* bvVars = nullptr;
        std::set<UVariable>* ueVars = nullptr;
        if( _bitvectorVars )
            bvVars = new std::set<BVVariable>();
        bool considerUninterpreted = _uninterpretedVars || propertyHolds( PROP_CONTAINS_UNINTERPRETED_EQUATIONS );
        if( considerUninterpreted )
            ueVars = new std::set<UVariable>();
        collectVariables_( _vars, bvVars, ueVars, _booleanVars, _realVars, _integerVars, _uninterpretedVars, _bitvectorVars );
        if( _bitvectorVars )
        {
            assert( bvVars != nullptr );
            for (const auto& v: *bvVars) _vars.insert(v.variable());
            delete bvVars;
        }
        if( considerUninterpreted )
        {   
            for (const auto& v: *ueVars) {
                switch( SortManager::getInstance().getType(v.domain()) )
                {
                    case VariableType::VT_UNINTERPRETED:
                        if( _uninterpretedVars )
                            _vars.insert(v());
                        break;
                    case VariableType::VT_BOOL:
                        if( _booleanVars )
                            _vars.insert(v());
                        break;
                    case VariableType::VT_REAL:
                        if( _realVars )
                            _vars.insert(v());
                        break;
                    default:
                        assert( SortManager::getInstance().getType(v.domain()) == VariableType::VT_INT );
                        if( _integerVars )
                            _vars.insert(v());
                        break;
                }
            }
            delete ueVars;
        }
    }
    
    template<typename Pol>
    void Formula<Pol>::collectVariables_( Variables& _vars, std::set<BVVariable>* _bvVars, std::set<UVariable>* _ueVars, bool _booleanVars, bool _realVars, bool _integerVars, bool _uninterpretedVars, bool _bitvectorVars ) const
    {
        switch( getType() )
        {
            case FormulaType::TRUE:
            case FormulaType::FALSE:
                break;
            case FormulaType::BOOL:
                if( _booleanVars )
                    _vars.insert( boolean() );
                break;
            case FormulaType::CONSTRAINT:
                for( auto var : constraint().variables() )
                {
                    if( _integerVars )
                        _vars.insert( var );
                    if( _realVars )
                        _vars.insert( var );
                }
                break;
			case FormulaType::VARCOMPARE:
				variableComparison().collectVariables(_vars);
				break;
			case FormulaType::VARASSIGN:
				variableAssignment().collectVariables(_vars);
				break;
            case FormulaType::BITVECTOR:
                if( _bitvectorVars ) 
                {
                    assert( _bvVars != nullptr );
                    bvConstraint().collectVariables(*_bvVars);
                }
                break;
            case FormulaType::UEQ:
                if( _uninterpretedVars )
                {
                    assert( _ueVars != nullptr );
                    uequality().collectUVariables(*_ueVars);
                }
                break;
			case FormulaType::PBCONSTRAINT:
				if (_booleanVars) {
					for (auto var: pbConstraint().gatherVariables()) {
						_vars.insert(var);
					}
				}
				break;
            case FormulaType::NOT:
                subformula().collectVariables_( _vars, _bvVars, _ueVars, _booleanVars, _realVars, _integerVars, _uninterpretedVars, _bitvectorVars );
                break;
            case FormulaType::EXISTS:
            case FormulaType::FORALL:
                quantifiedFormula().collectVariables_( _vars, _bvVars, _ueVars, _booleanVars, _realVars, _integerVars, _uninterpretedVars, _bitvectorVars );
                break;
            default:
            {
                for( const Formula<Pol>& subFormula : subformulas() )
                    subFormula.collectVariables_( _vars, _bvVars, _ueVars, _booleanVars, _realVars, _integerVars, _uninterpretedVars, _bitvectorVars );
            }
        }
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
                        result += _f.constraint().complexity(); break;
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
#ifdef __VS
                addConstraintProperties( *_content.mpConstraintVS, _content.mProperties );
#else                
		addConstraintProperties( _content.mConstraint, _content.mProperties );
#endif
		break;
            }
            case FormulaType::FALSE:
            {
                _content.mProperties |= STRONG_CONDITIONS;
#ifdef __VS
                addConstraintProperties( *_content.mpConstraintVS, _content.mProperties );
#else                
		addConstraintProperties( _content.mConstraint, _content.mProperties );
#endif
                break;
            }
            case FormulaType::BOOL:
            {
                _content.mProperties |= STRONG_CONDITIONS | PROP_CONTAINS_BOOLEAN;
                break;
            }
            case FormulaType::NOT:
            {
#ifdef __VS
                Condition subFormulaConds = _content.mpSubformulaVS->mpContent->mProperties;
#else                
                Condition subFormulaConds = _content.mSubformula.mpContent->mProperties;
#endif
                if( PROP_IS_AN_ATOM <= subFormulaConds )
                    _content.mProperties |= PROP_IS_A_CLAUSE | PROP_IS_A_LITERAL | PROP_IS_IN_CNF | PROP_IS_LITERAL_CONJUNCTION;
                _content.mProperties |= (subFormulaConds & WEAK_CONDITIONS);
                break;
            }
            case FormulaType::OR:
            {
                _content.mProperties |= PROP_IS_A_CLAUSE | PROP_IS_IN_CNF | PROP_IS_IN_NNF;
#ifdef __VS
				for (auto subFormula = _content.mpSubformulasVS->begin(); subFormula != _content.mpSubformulasVS->end(); ++subFormula)
#else
				for (auto subFormula = _content.mSubformulas.begin(); subFormula != _content.mSubformulas.end(); ++subFormula)
#endif
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
#ifdef __VS
		for (auto subFormula = _content.mpSubformulasVS->begin(); subFormula != _content.mpSubformulasVS->end(); ++subFormula)
#else
		for (auto subFormula = _content.mSubformulas.begin(); subFormula != _content.mSubformulas.end(); ++subFormula)
#endif
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
#ifdef __VS
				for (auto subFormula = _content.mpSubformulasVS->begin(); subFormula != _content.mpSubformulasVS->end(); ++subFormula)
#else
				for (auto subFormula = _content.mSubformulas.begin(); subFormula != _content.mSubformulas.end(); ++subFormula)
#endif
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
#ifdef __VS
                addConstraintProperties( *_content.mpConstraintVS, _content.mProperties );
#else
				addConstraintProperties(_content.mConstraint, _content.mProperties);
#endif
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
            case FormulaType::PBCONSTRAINT:
            {
                _content.mProperties |= STRONG_CONDITIONS | PROP_CONTAINS_PSEUDOBOOLEAN;
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
        if( _constraint.lhs().isZero() )
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
        if( _constraints.isPseudoBoolean() )
            _properties |= PROP_CONTAINS_PSEUDOBOOLEAN;
    }
    
    template<typename Pol>
    std::string Formula<Pol>::toString( bool _withActivity, unsigned _resolveUnequal, const std::string _init, bool _oneline, bool _infix, bool _friendlyNames, bool _withVariableDefinition ) const
    {
        std::string result = "";
        if( _withVariableDefinition )
        {
            std::stringstream os;
            
            carl::SortManager::getInstance().exportDefinitions(os);
            
            carl::FormulaVisitor<Formula<Pol>> visitor;
            Variables vars;
            std::set<UVariable> uvars;
            std::set<BVVariable> bvvars;
            visitor.visit(*this,
                    [&](const Formula& _f) 
                    {
                        switch(_f.getType())
                        {
                            case FormulaType::BOOL:
                                vars.insert( _f.boolean() );
                                break;
                            case FormulaType::CONSTRAINT:
                                for( auto var : _f.constraint().variables() ) vars.insert( var );
                                break;
                            case FormulaType::UEQ:
                                _f.uequality().collectUVariables( uvars );
                                break;
                            case FormulaType::BITVECTOR:
                                _f.bvConstraint().collectVariables(bvvars);
                                break;
                            default:
                                break;
                        }
                    });
            for( auto var : vars )
                os << "(declare-fun " << var << " () " << var.type() << ")\n";
            for( const auto& uvar : uvars )
                os << "(declare-fun " << uvar() << " () " << uvar.domain() << ")\n";
            for( const auto& bvvar : bvvars )
                os << "(declare-fun " << bvvar << " () " << bvvar.sort() << ")\n";
            for (const auto& ufc: UFManager::getInstance().ufContents()) {
                if (ufc == nullptr) continue;
                os << "(declare-fun " << ufc->name() << " (";
                for (const auto& s: ufc->domain()) os << s << " ";
                os << ") " << ufc->codomain() << ")\n";
            }
            result += os.str();
            result += "(assert ";
        }
        result += mpContent->toString( _withActivity, _resolveUnequal, _init, _oneline, _infix, _friendlyNames );
        if( _withVariableDefinition )
            result += ")\n";
        return result;
    }
    
    template<typename Pol>
    ostream& operator<<( ostream& _ostream, const Formula<Pol>& _formula )
    {
        return (_ostream << _formula.toString( false, 0, "", true, false, true ));
    }

    template<typename Pol>
    void Formula<Pol>::printProposition( ostream& _out, const string _init ) const
    {
        _out << _init;
        for( unsigned i = 0; i < properties().size(); ++i )
        {
            if( fmod( i, 10.0 ) == 0.0 ) 
                _out << " ";
            _out << properties()[i];
        }
        _out << endl;
    }
    
    template<typename Pol>
    string Formula<Pol>::toRedlogFormat( bool _withVariables ) const
    {
        string result = "";
        string oper = formulaTypeToString( getType() );
        switch( getType() )
        {
            // unary cases
            case FormulaType::TRUE:
                result += " " + oper + " ";
                break;
            case FormulaType::FALSE:
                result += " " + oper + " ";
                break;
            case FormulaType::NOT:
                result += " " + oper + "( " + subformula().toRedlogFormat( _withVariables ) + " )";
                break;
            case FormulaType::CONSTRAINT:
                result += constraint().toString( 1 );
                break;
            case FormulaType::BOOL:
                result += VariablePool::getInstance().getName( boolean(), true ) + " = 1";
                break;
            case FormulaType::IMPLIES:
                result += "( " + premise().toRedlogFormat( _withVariables ) + " " + oper + " " + premise().toRedlogFormat( _withVariables ) + " )";
                break;
            default:
            {
                // recursive print of the subformulas
                if( _withVariables )
                { // add the variables
                    result += "( ex( {";
                    result += variableListToString( "," );
                    result += "}, (";
                    // Make pseudo Booleans.
                    std::set<Variable> boolVars;
                    booleanVars(boolVars);
                    for (auto v: boolVars) {
                        result += "(" + v.name() + " = 0 or " + v.name() + " = 1) and ";
                    }
                }
                else
                    result += "( ";
                typename Formulas<Pol>::const_iterator it = subformulas().begin();
                // do not quantify variables again.
                result += it->toRedlogFormat( false );
                for( ++it; it != subformulas().end(); ++it ) // do not quantify variables again.
                    result += " " + oper + " " + it->toRedlogFormat( false );
                if( _withVariables )
                    result += " ) )";
                result += " )";
            }
        }
        return result;
    }

    template<typename Pol>
    string Formula<Pol>::variableListToString( string _separator, const unordered_map<string, string>& _variableIds ) const
    {
        Variables realVars;
        realValuedVars( realVars );
        std::set<Variable> boolVars;
        booleanVars( boolVars );
        auto i = realVars.begin();
        auto j = boolVars.begin();
        string result;
        if( i != realVars.end() )
        {
            stringstream sstream;
            sstream << *i;
            unordered_map<string, string>::const_iterator vId = _variableIds.find( sstream.str() );
            result += vId == _variableIds.end() ? sstream.str() : vId->second;
            for( ++i; i != realVars.end(); ++i )
            {
                result += _separator;
                vId = _variableIds.find(sstream.str());
                result += vId == _variableIds.end() ? sstream.str() : vId->second;
            }
        }
        else if( j != boolVars.end() )
        {
            string boolName = VariablePool::getInstance().getName( *j, true );
            unordered_map<string, string>::const_iterator vId = _variableIds.find(boolName);
            result += vId == _variableIds.end() ? boolName : vId->second;
            for( ++j; j != boolVars.end(); ++j )
            {
                boolName = VariablePool::getInstance().getName( *j, true );
                result += _separator;
                vId = _variableIds.find(boolName);
                result += vId == _variableIds.end() ? boolName : vId->second;
            }
        }
        return result;
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
                            cerr << "Unexpected relation symbol!" << endl;
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
                cerr << "Unexpected type of formula!" << endl;
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
    Formula<Pol> Formula<Pol>::toQF(QuantifiedVariables& variables, unsigned level, bool negated) const
    {
        switch (getType()) {
            case FormulaType::AND:
            case FormulaType::IFF:
            case FormulaType::OR:
            case FormulaType::XOR:
            {
                if (!negated) {
                    Formulas<Pol> subs;
                    for (auto& sub: subformulas()) {
                        subs.push_back(sub.toQF(variables, level, false));
                    }
                    return Formula<Pol>( getType(), std::move(subs) );
                } else if (getType() == FormulaType::AND || getType() == FormulaType::OR) {
                    Formulas<Pol> subs;
                    for (auto& sub: subformulas()) {
                        subs.push_back(sub.toQF(variables, level, true));
                    }
                    if (getType() == FormulaType::AND) return Formula<Pol>(FormulaType::OR, std::move(subs));
                    else return Formula<Pol>(FormulaType::AND, std::move(subs));
                } else if (getType() == FormulaType::IFF) {
                    Formulas<Pol> sub1;
                    Formulas<Pol> sub2;
                    for (auto& sub: subformulas()) {
                        sub1.push_back(sub.toQF(variables, level, true));
                        sub2.push_back(sub.toQF(variables, level, false));
                    }
                    return Formula<Pol>(FormulaType::AND, {Formula<Pol>(FormulaType::OR, std::move(sub1)), Formula<Pol>(FormulaType::OR, std::move(sub2))});
                } else if (getType() == FormulaType::XOR) {
                    auto lhs = back().toQF(variables, level, false);
                    auto rhs = connectPrecedingSubformulas().toQF(variables, level, true);
                    return Formula<Pol>(FormulaType::IFF, {lhs, rhs});
                }
                assert(false);
            }
            case FormulaType::BOOL:
            case FormulaType::CONSTRAINT:
            case FormulaType::FALSE:
            case FormulaType::UEQ:
            case FormulaType::BITVECTOR:
            case FormulaType::TRUE:
            {
                if (negated) return Formula<Pol>( NOT, *this );
                else return *this;
            }
            case FormulaType::EXISTS:
            case FormulaType::FORALL:
            {
                unsigned cur = 0;
                if ((level % 2 == (getType() == FormulaType::EXISTS ? (unsigned)0 : (unsigned)1)) ^ negated) cur = level;
                else cur = level+1;
                Variables vars(quantifiedVariables().begin(), quantifiedVariables().end());
                Formula<Pol> f = quantifiedFormula();
                for (auto it = vars.begin(); it != vars.end();) {
                    if (it->type() == VariableType::VT_BOOL) {
                        // Just leave boolean variables at the base level up to the SAT solver.
                        if (cur > 0) {
                            f = Formula<Pol>(
                                (getType() == FormulaType::EXISTS ? FormulaType::OR : FormulaType::AND),
                                {f.substitute({{*it, Formula<Pol>( FormulaType::TRUE )}}),
                                f.substitute({{*it, Formula<Pol>( FormulaType::FALSE )}})}
                            );
                        }
                        it = vars.erase(it);
                    }
                    else it++;
                }
                if (vars.size() > 0) {
                    while (variables.size() <= cur) variables.emplace_back();
                    variables[cur].insert(vars.begin(), vars.end());
                }
                return f.toQF(variables, cur, negated);
            }
            case FormulaType::IMPLIES:
                if (negated) return Formula<Pol>(FormulaType::AND, {premise().toQF(variables, level, false), conclusion().toQF(variables, level, true)});
                else return Formula<Pol>( FormulaType::IMPLIES, {premise().toQF(variables, level, false), conclusion().toQF(variables, level, false)});
            case FormulaType::ITE:
                return Formula<Pol>( FormulaType::ITE, {condition().toQF(variables, level, negated), firstCase().toQF(variables, level, negated), secondCase().toQF(variables, level, negated)});
            case FormulaType::NOT:
                return subformula().toQF(variables, level, !negated);
        }
        return Formula<Pol>( FormulaType::TRUE );
    }

    template<typename Pol>
    Formula<Pol> Formula<Pol>::toCNF( bool _keepConstraints, bool _simplifyConstraintCombinations, bool _tseitinWithEquivalence ) const
    {
        if( !_simplifyConstraintCombinations && propertyHolds( PROP_IS_IN_CNF ) )
        {
            if( _keepConstraints )
                return *this;
            else if( getType() == FormulaType::NOT )
            {
                assert( propertyHolds( PROP_IS_A_LITERAL ) );
                return resolveNegation( _keepConstraints );
            }
        }
        else if( isAtom() )
            return *this;
        Formulas<Pol> resultSubformulas;
        ConstraintBounds constraintBoundsAnd;
        std::vector<Formula<Pol>> subformulasToTransform;
        subformulasToTransform.push_back( *this );
        while( !subformulasToTransform.empty() )
        {
            Formula<Pol> currentFormula = subformulasToTransform.back();
//            cout << "To add:" << endl;
//            for( const auto& f : subformulasToTransform )
//                cout << "   " << f << endl;
//            cout << endl;
//            cout << "Conjunction:" << endl;
//            for( const auto& f : resultSubformulas )
//                cout << "   " << f << endl;
//            cout << endl;
            subformulasToTransform.pop_back();
            switch( currentFormula.getType() )
            {
                case FormulaType::BOOL:
                {
                    resultSubformulas.push_back( currentFormula );
                    break;
                }
                case FormulaType::UEQ:
                {
                    resultSubformulas.push_back( currentFormula );
                    break;
                }
                case FormulaType::CONSTRAINT:
                {   
                    if( _simplifyConstraintCombinations )
                    {
                        if( addConstraintBound( constraintBoundsAnd, currentFormula, true ).isFalse() )
                        {
                            goto ReturnFalse;
                        }
                    }
                    else
                        resultSubformulas.push_back( currentFormula );
                    break;
                }
				case FormulaType::VARASSIGN:
				{
					resultSubformulas.push_back( currentFormula );
					break;
				}
				case FormulaType::VARCOMPARE:
				{
					resultSubformulas.push_back( currentFormula );
					break;
				}
                case FormulaType::BITVECTOR:
                {
                    resultSubformulas.push_back( currentFormula );
                    break;
                }
                case FormulaType::TRUE: // Remove it.
                    break;
                case FormulaType::FALSE: // Return false.
                    goto ReturnFalse;
                case FormulaType::NOT: // Try to resolve this negation.
                {
                    Formula<Pol> resolvedFormula = currentFormula.resolveNegation( _keepConstraints );
                    if( resolvedFormula.propertyHolds( PROP_IS_A_LITERAL ) ) // It is a literal.
                    {
                        if( resolvedFormula.getType() == FormulaType::CONSTRAINT 
                                || (resolvedFormula.getType() == FormulaType::NOT && resolvedFormula.subformula().getType() == FormulaType::CONSTRAINT) )
                        {
                            if( _simplifyConstraintCombinations )
                            {
                                if( addConstraintBound( constraintBoundsAnd, resolvedFormula, true ).isFalse() )
                                {
                                    goto ReturnFalse;
                                }
                            }
                            else
                            {
                                resultSubformulas.push_back( resolvedFormula );
                            }
                        }
                        else
                        {
                            resultSubformulas.push_back( resolvedFormula );
                        }
                    }
                    else
                        subformulasToTransform.push_back( resolvedFormula );
                    break;
                }
                case FormulaType::AND: // (and phi_1 .. phi_n) -> psi_1 .. psi_m
                {
                    for( const Formula<Pol>& subFormula : currentFormula.subformulas() )
                        subformulasToTransform.push_back( subFormula );
                    break;
                }
                case FormulaType::IMPLIES: // (-> lhs rhs)  ->  (or (not lhs) rhs)
                {
                    Formulas<Pol> tmpSubformulas;
                    tmpSubformulas.emplace_back( NOT, currentFormula.premise() );
                    tmpSubformulas.push_back( currentFormula.conclusion() );
                    subformulasToTransform.emplace_back( FormulaType::OR, std::move(tmpSubformulas) );
                    break;
                }
                case FormulaType::ITE: // (ite cond then else)  ->  auxBool, where (or (not cond) then) and (or cond else) are added to the queue
                {
                    // Add: (or (not cond) then)
                    subformulasToTransform.push_back( Formula<Pol>(FormulaType::OR, {Formula<Pol>( FormulaType::NOT, currentFormula.condition() ), currentFormula.firstCase()}) );
                    // Add: (or cond else)
                    subformulasToTransform.push_back( Formula<Pol>(FormulaType::OR, {currentFormula.condition(), currentFormula.secondCase()}) );
                    break;
                }
                case FormulaType::IFF:
                {
                    if( currentFormula.subformulas().size() > 2 )
                    {
                        // (iff phi_1 .. phi_n) -> (or (and phi_1 .. phi_n) (and (not phi_1) .. (not phi_n))) is added to the queue
                        Formulas<Pol> subformulasA;
                        Formulas<Pol> subformulasB;
                        for( auto& subFormula : currentFormula.subformulas() )
                        {
                            subformulasA.push_back( subFormula );
                            subformulasB.emplace_back( FormulaType::NOT, subFormula );
                        }
                        subformulasToTransform.push_back( Formula<Pol>(FormulaType::OR, {Formula<Pol>( AND, move( subformulasA ) ), Formula<Pol>( FormulaType::AND, move( subformulasB ) )}) );
                    }
                    else
                    {
                        // (iff lhs rhs) -> (or lhs (not rhs)) and (or (not lhs) rhs) are added to the queue
                        assert( currentFormula.subformulas().size() == 2 );
                        // Get lhs and rhs.
                        const Formula<Pol>& lhs = *currentFormula.subformulas().begin();
                        const Formula<Pol>& rhs = currentFormula.back();
                        if( lhs.getType() == FormulaType::AND )
                        {
                            Formulas<Pol> subformulas;
                            for( auto& subFormula : lhs.subformulas() )
                            {
                                subformulas.emplace_back( FormulaType::NOT, subFormula );
                            }
                            subformulas.push_back( rhs );
                            subformulasToTransform.emplace_back( FormulaType::OR, std::move(subformulas) );
                            Formula<Pol> rhsNegated( FormulaType::NOT, rhs );
                            for( auto& subFormula : lhs.subformulas() )
                            {
                                subformulasToTransform.push_back( Formula<Pol>(FormulaType::OR, {subFormula, rhsNegated}) );
                            }
                        }
                        else if( rhs.getType() == FormulaType::AND )
                        {
                            Formulas<Pol> subformulas;
                            for( auto& subFormula : rhs.subformulas() )
                            {
                                subformulas.emplace_back( FormulaType::NOT, subFormula );
                            }
                            subformulas.push_back( lhs );
                            subformulasToTransform.emplace_back( FormulaType::OR, std::move(subformulas) );
                            Formula<Pol> lhsNegated( FormulaType::NOT, lhs );
                            for( auto& subFormula : rhs.subformulas() )
                            {
                                subformulasToTransform.push_back( Formula<Pol>(FormulaType::OR, {subFormula, lhsNegated}) );
                            }
                        }
                        else
                        {
                            // add (or lhs (not rhs)) to the queue
                            subformulasToTransform.push_back( Formula<Pol>(FormulaType::OR, {lhs, Formula<Pol>( FormulaType::NOT, rhs )}) );
                            // add (or (not lhs) rhs) to the queue
                            subformulasToTransform.push_back( Formula<Pol>(FormulaType::OR, {Formula<Pol>( FormulaType::NOT, lhs ), rhs}) );
                        }
                    }
                    break;
                }
                case FormulaType::XOR: // (xor lhs rhs) -> (or lhs rhs) and (or (not lhs) (not rhs)) are added to the queue
                {
                    // Get lhs and rhs.
                    Formula<Pol> lhs = currentFormula.connectPrecedingSubformulas();
                    const Formula<Pol>& rhs = currentFormula.back();
                    // add (or lhs rhs) to the queue
                    subformulasToTransform.push_back( Formula<Pol>(FormulaType::OR, {lhs, rhs}));
                    // add (or (not lhs) (not rhs)) to the queue
                    subformulasToTransform.push_back( Formula<Pol>(FormulaType::OR, {Formula<Pol>( FormulaType::NOT, lhs ), Formula<Pol>( FormulaType::NOT, rhs )}) );
                    break;
                }
                // Note, that the following case could be implemented using less code, but it would clearly
                // lead to a worse performance as we would then not benefit from the properties of a disjunction.
                case FormulaType::OR: // (or phi_1 .. phi_n) -> (or psi_1 .. psi_m),  where phi_i is transformed as follows:
                {
                    bool currentFormulaValid = false;
                    ConstraintBounds constraintBoundsOr;
                    Formulas<Pol> subsubformulas;
                    std::vector<Formula<Pol>> phis;
                    for( const Formula<Pol>& subFormula : currentFormula.subformulas() )
                        phis.push_back( subFormula );
                    std::vector<Formula<Pol>> subformulasToTransformTmp;
                    while( !currentFormulaValid && !phis.empty() )
                    {
                        Formula<Pol> currentSubformula = phis.back();
//                        cout << "    To add:" << endl;
//                        for( const auto& f : phis )
//                            cout << "       " << f << endl;
//                        cout << endl;
//                        cout << "    Disjunction:" << endl;
//                        for( const auto& f : subsubformulas )
//                            cout << "       " << f << endl;
//                        cout << endl;
                        phis.pop_back();
                        switch( currentSubformula.getType() )
                        {
                            case FormulaType::BOOL: // B -> B
                                subsubformulas.push_back( currentSubformula );
                                break;
                            case FormulaType::UEQ: // u -> u
                                subsubformulas.push_back( currentSubformula );
                                break;
                            case FormulaType::TRUE: // remove the entire considered disjunction and everything which has been created by considering it
                                currentFormulaValid = true;
                                break;
                            case FormulaType::FALSE: // remove it
                                break;
                            case FormulaType::OR: // (or phi_i1 .. phi_ik) -> phi_i1 .. phi_ik
                                for( const Formula<Pol>& subsubformula : currentSubformula.subformulas() )
                                    phis.push_back( subsubformula );
                                break;
                            case FormulaType::IMPLIES: // (-> lhs_i rhs_i) -> (not lhs_i) rhs_i
                                phis.emplace_back( NOT, currentSubformula.premise() );
                                phis.push_back( currentSubformula.conclusion() );
                                break;
                            case FormulaType::ITE: // (ite cond then else)  ->  (and (or (not cond) then) (or cond else))
                            {
                                Formulas<Pol> tmpSubformulas;
                                // Add: (or (not cond) then)
                                tmpSubformulas.push_back( Formula<Pol>(FormulaType::OR, {Formula<Pol>( FormulaType::NOT, currentSubformula.condition() ), currentSubformula.firstCase()}) );
                                // Add: (or cond else)
                                tmpSubformulas.push_back( Formula<Pol>(FormulaType::OR, {currentSubformula.condition(), currentSubformula.secondCase()}) );
                                phis.emplace_back( FormulaType::AND, std::move(tmpSubformulas) );
                                break;
                            }
                            case FormulaType::NOT: // resolve the negation
                            {
                                Formula<Pol> resolvedFormula = currentSubformula.resolveNegation( _keepConstraints );
                                if( resolvedFormula.propertyHolds( PROP_IS_A_LITERAL ) ) // It is a literal.
                                {
                                    if( resolvedFormula.getType() == FormulaType::CONSTRAINT 
                                            || (resolvedFormula.getType() == FormulaType::NOT && resolvedFormula.subformula().getType() == CONSTRAINT) )
                                    {
                                        if( _simplifyConstraintCombinations )
                                        {
                                            if( addConstraintBound( constraintBoundsOr, resolvedFormula, false ).isFalse() )
                                            {
                                                currentFormulaValid = true;
                                                break;
                                            }
                                        }
                                        else
                                        {
                                            subsubformulas.push_back( resolvedFormula );
                                        }
                                    }
                                    else
                                    {
                                        subsubformulas.push_back( resolvedFormula );
                                    }
                                }
                                else
                                    phis.push_back( resolvedFormula );
                                break;
                            }
                            case FormulaType::AND: // (and phi_i1 .. phi_ik) -> h_i, where (or (not h_i) phi_i1) .. (or (not h_i) phi_ik) 
                                      //                                and (or h_i (not phi_i1) .. (not phi_ik))  is added to the queue
                            {
                                bool conjunctionIsFalse = false;
                                ConstraintBounds constraintBoundsOrAnd;
                                Formulas<Pol> tmpSubSubformulas;
                                for( const Formula<Pol>& subsubformula : currentSubformula.subformulas() )
                                {
                                    if( subsubformula.getType() == FormulaType::CONSTRAINT 
                                            || (subsubformula.getType() == FormulaType::NOT && subsubformula.subformula().getType() == FormulaType::CONSTRAINT ) )
                                    {
                                        if( _simplifyConstraintCombinations )
                                        {
                                            if( addConstraintBound( constraintBoundsOrAnd, subsubformula, true ).isFalse() )
                                            {
                                                conjunctionIsFalse = true;
                                                break;
                                            }
                                        }
                                        else
                                        {
                                            tmpSubSubformulas.push_back( subsubformula );
                                        }
                                    }
                                    else
                                    {
                                        tmpSubSubformulas.push_back( subsubformula );
                                    }
                                }
                                if( conjunctionIsFalse )
                                    break;
                                if( _simplifyConstraintCombinations && swapConstraintBounds( constraintBoundsOrAnd, tmpSubSubformulas, true ) )
                                    break;
                                Formula<Pol> tseitinVar = FormulaPool<Pol>::getInstance().createTseitinVar( currentSubformula );
                                for( const Formula<Pol>& subsubformula : tmpSubSubformulas )
                                {
                                    assert( !subsubformula.isFalse() );
                                    if( !subsubformula.isTrue() )
                                    {
                                        Formula<Pol> notTVar( FormulaType::NOT, tseitinVar );
                                        Formula<Pol> tmpOr( OR, {notTVar, subsubformula} );
                                        subformulasToTransformTmp.push_back( std::move( tmpOr ) );
                                        subformulasToTransformTmp.back().mpContent->mTseitinClause = true;
                                    }
                                }
                                if( _tseitinWithEquivalence )
                                {
                                    Formulas<Pol> tmpSubformulas;
                                    tmpSubformulas.push_back( tseitinVar );
                                    for( const Formula<Pol>& subsubformula : tmpSubSubformulas )
                                    {
                                        if( !subsubformula.isTrue() )
                                            tmpSubformulas.emplace_back( NOT, subsubformula );
                                    }
                                    subformulasToTransformTmp.emplace_back( OR, std::move(tmpSubformulas) );
                                    subformulasToTransformTmp.back().mpContent->mTseitinClause = true;
                                }
                                subsubformulas.push_back( tseitinVar );
                                break;
                            }
                            case FormulaType::CONSTRAINT: // p~0 -> p~0
                            {
                                if( _simplifyConstraintCombinations )
                                {
                                    if( addConstraintBound( constraintBoundsOr, currentSubformula, false ).isFalse() )
                                    {
                                        currentFormulaValid = true;
                                        break;
                                    }
                                }
                                else
                                {
                                    subsubformulas.push_back( currentSubformula );
                                }
                                break;
                            }
							case FormulaType::VARASSIGN:
							{
								subsubformulas.push_back( currentSubformula );
								break;
							}
							case FormulaType::VARCOMPARE:
							{
								subsubformulas.push_back( currentSubformula );
								break;
							}
                            case FormulaType::BITVECTOR:
                            {
                                ///@todo Anything more to do here?
                                subsubformulas.push_back( currentSubformula );
                                break;
                            }
                            case FormulaType::IFF: // (iff phi_1 .. phi_n) -> (and phi_1 .. phi_n) and (and (not phi_1) .. (not phi_n)) are added to the queue
                            {
                                Formulas<Pol> subformulasA;
                                Formulas<Pol> subformulasB;
                                for( auto& subFormula : currentSubformula.subformulas() )
                                {
                                    subformulasA.push_back( subFormula );
                                    subformulasB.emplace_back( FormulaType::NOT, subFormula );
                                }
                                phis.emplace_back( FormulaType::AND, std::move( subformulasA ) );
                                phis.emplace_back( FormulaType::AND, std::move( subformulasB ) );
                                break;
                            }
                            case FormulaType::XOR: // (xor lhs rhs) -> (and lhs (not rhs)) and (and (not lhs) rhs) are added to the queue
                            {
                                // Get lhs and rhs.
                                Formula<Pol> lhs = currentSubformula.connectPrecedingSubformulas();
                                const Formula<Pol>& rhs = currentSubformula.back();
                                // add (and lhs (not rhs)) to the queue
                                phis.push_back( Formula<Pol>(FormulaType::AND, {lhs, Formula<Pol>( FormulaType::NOT, rhs )}) );
                                // add (and (not lhs) rhs) to the queue
                                phis.push_back( Formula<Pol>(FormulaType::AND, {Formula<Pol>( FormulaType::NOT, lhs ), rhs}) );
                                break;
                            }
                            case FormulaType::EXISTS:
                            {
                                assert(false);
                                std::cerr << "Formula must be quantifier-free!" << std::endl;
                                break;
                            }
                            case FormulaType::FORALL:
                            {
                                assert(false);
                                std::cerr << "Formula must be quantifier-free!" << std::endl;
                                break;
                            }
                            default:
                            {
                                assert( false );
                                cerr << "Unexpected type of formula!" << endl;
                            }
                        }
                    }
                    if( !currentFormulaValid && (!_simplifyConstraintCombinations || !swapConstraintBounds( constraintBoundsOr, subsubformulas, false )) )
                    {
                        subformulasToTransform.insert(subformulasToTransform.end(), subformulasToTransformTmp.begin(), subformulasToTransformTmp.end() );
                        if( subsubformulas.empty() ) // Empty clause = false, which, added to a conjunction, leads to false.
                        {
                            goto ReturnFalse;
                        }
                        else if( subsubformulas.size() == 1 )
                        {
                            resultSubformulas.push_back( *subsubformulas.begin() );
                        }
                        else if( subsubformulas.size() >= sizeof(uint32_t) )
                        {
                            int clauseBeginPos = 0;
                            int clauseEndPos = 0;
                            Formula<Pol> tseitinVar( TRUE );
                            while( true )
                            {
                                clauseEndPos = clauseBeginPos + (int)sizeof(uint32_t) - 2;
                                if( (size_t) clauseEndPos > subsubformulas.size() )
                                {
                                    std::vector<Formula<Pol>> partOfSubformulas;
                                    partOfSubformulas.reserve(subsubformulas.size()-(size_t)clauseBeginPos);
                                    assert( !tseitinVar.isTrue() );
                                    partOfSubformulas.emplace_back( NOT, tseitinVar );
                                    std::move( subsubformulas.begin()+clauseBeginPos, subsubformulas.end(), std::back_inserter(partOfSubformulas));
                                    resultSubformulas.emplace_back( OR, std::move( partOfSubformulas ) );
                                    break;
                                }
                                std::vector<Formula<Pol>> partOfSubformulas;
                                partOfSubformulas.reserve(sizeof(uint32_t));
                                if( !tseitinVar.isTrue() )
                                    partOfSubformulas.emplace_back( NOT, tseitinVar );
                                std::move( subsubformulas.begin()+clauseBeginPos, subsubformulas.begin()+clauseEndPos, std::back_inserter(partOfSubformulas));
                                tseitinVar = Formula<Pol>( freshBooleanVariable() );
                                partOfSubformulas.push_back( tseitinVar );
                                resultSubformulas.emplace_back( OR, std::move( partOfSubformulas ) );
                                clauseBeginPos = clauseEndPos;
                            }
                        }
                        else
                        {
                            resultSubformulas.emplace_back( OR, std::move( subsubformulas ) );
                        }
                    }
                    break;
                }
                case FormulaType::EXISTS:
                {
                    assert(false);
                    std::cerr << "Formula must be quantifier-free!" << std::endl;
                    break;
                }
                case FormulaType::FORALL:
                {
                    assert(false);
                    std::cerr << "Formula must be quantifier-free!" << std::endl;
                    break;
                }
                default:
                {
                    assert( false );
                    cerr << "Unexpected type of formula!" << endl;
                }
            }
        }
        if( _simplifyConstraintCombinations && swapConstraintBounds( constraintBoundsAnd, resultSubformulas, true ) )
            goto ReturnFalse;
        if( resultSubformulas.empty() )
            return Formula<Pol>( FormulaType::TRUE );
        else if( resultSubformulas.size() == 1 )
            return *resultSubformulas.begin();
        else
            return Formula<Pol>( FormulaType::AND, move( resultSubformulas ) );
        ReturnFalse:
            return Formula<Pol>( FormulaType::FALSE );
    }
            
    template<typename Pol>
    Formula<Pol> Formula<Pol>::substitute( const map<Variable, Formula<Pol>>& _booleanSubstitutions, const map<Variable, Pol>& _arithmeticSubstitutions ) const
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
                Pol lhsSubstituted = constraint().lhs().substitute( _arithmeticSubstitutions );
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
        cout << "add from a " << (_inConjunction ? "conjunction" : "disjunction") << " to " << &_constraintBounds << ":   " << _constraint << endl;
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
            relation = carl::turnAroundRelation( relation );
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
        cout << "try to add the bound  " << relation << boundValue << "  for the polynomial  " << poly << endl; 
        #endif
        auto resA = _constraintBounds.insert( make_pair( std::move(poly), std::move( map<typename Pol::NumberType, pair<Relation, Formula<Pol>>>() ) ) );
        auto resB = resA.first->second.insert( make_pair( boundValue, make_pair( relation, _constraint ) ) );
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
        cout << "swap from " << &_constraintBounds << " to a " << (_inConjunction ? "conjunction" : "disjunction") << endl;
        #endif
        while( !_constraintBounds.empty() )
        {
            #ifdef CONSTRAINT_BOUND_DEBUG
            cout << "for the bounds of  " << _constraintBounds.begin()->first << endl;
            #endif
            const map<typename Pol::NumberType, pair<Relation, Formula<Pol>>>& bounds = _constraintBounds.begin()->second;
            assert( !bounds.empty() );
            if( bounds.size() == 1 )
            {
                _intoFormulas.push_back( bounds.begin()->second.second );
                #ifdef CONSTRAINT_BOUND_DEBUG
                cout << "   just add the only bound" << endl;
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
                    cout << "   bound is  " << iter->second.first << iter->first << endl;
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
                                    cout << "      case: " << __LINE__ << endl;
                                    #endif
                                    _intoFormulas.push_back( iter->second.second );
                                }
                                else
                                {
                                    #ifdef CONSTRAINT_BOUND_DEBUG
                                    cout << "      case: " << __LINE__ << endl;
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
                            cout << "      case: " << __LINE__ << endl;
                            #endif
                            break;
                        }
                        else
                        {
                            #ifdef CONSTRAINT_BOUND_DEBUG
                            cout << "      case: " << __LINE__ << endl;
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
                            cout << "      case: " << __LINE__ << endl;
                            #endif
                            break;
                        }
                        // _inConjunction == true: found first equality
                        // _inConjunction == false: found first bound with !=
                        else 
                        {
                            #ifdef CONSTRAINT_BOUND_DEBUG
                            cout << "      case: " << __LINE__ << endl;
                            #endif
                            moreSignificantCase = iter;
                        }
                    }
                    // _inConjunction == true: found an upper bound
                    // _inConjunction == false: found a lower bound
                    else
                    {
                        #ifdef CONSTRAINT_BOUND_DEBUG
                        cout << "      case: " << __LINE__ << endl;
                        #endif
                        assert( !_inConjunction || iter->second.first == Relation::LEQ || iter->second.first == Relation::LESS );
                        assert( _inConjunction || iter->second.first == Relation::GEQ || iter->second.first == Relation::GREATER );
                        if( _inConjunction && mostSignificantUpperBound == bounds.end() ) // first upper bound found = strongest upper bound
                        {
                            #ifdef CONSTRAINT_BOUND_DEBUG
                            cout << "      case: " << __LINE__ << endl;
                            #endif
                            mostSignificantUpperBound = iter;
                        }
                        else if( !_inConjunction && mostSignificantLowerBound == bounds.end() ) // first lower bound found = weakest lower bound
                        {
                            #ifdef CONSTRAINT_BOUND_DEBUG
                            cout << "      case: " << __LINE__ << endl;
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
                        cout << "mostSignificantUpperBound:   " << mostSignificantUpperBound->first << "  [" << mostSignificantUpperBound->second.second << "]" << endl;
                        cout << "mostSignificantLowerBound:   " << mostSignificantLowerBound->first << "  [" << mostSignificantLowerBound->second.second << "]" << endl;
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
            cout << endl;
            #endif
            return false;
        }
        else
        {
            _constraintBounds.clear();
            #ifdef CONSTRAINT_BOUND_DEBUG
            cout << "is " << (_inConjunction ? "invalid" : "valid") << endl << endl;
            #endif
            return true;
        }
    }

	template<typename Formula>
	void FormulaVisitor<Formula>::visit(const Formula& formula, const std::function<void(Formula)>& func) {
		switch (formula.getType()) {
		case AND:
		case OR:
		case IFF:
		case XOR: 
		case IMPLIES:
        case ITE:
        {
            for (const auto& cur: formula.subformulas()) visit(cur, func);
			break;
		}
		case NOT: {
			visit(formula.subformula(), func);
			break;
		}
		case BOOL:
		case CONSTRAINT:
		case VARCOMPARE:
		case VARASSIGN:
		case BITVECTOR:
		case TRUE:
		case FALSE:
		case UEQ:
		case PBCONSTRAINT:
			break;
		case EXISTS:
		case FORALL: {
			visit(formula.quantifiedFormula(), func);
			break;
		}
		}
		func(formula);
	}

	template<typename Formula>
	Formula FormulaVisitor<Formula>::visitResult(const Formula& formula, const std::function<Formula(Formula)>& func) {
		Formula newFormula = formula;
		switch (formula.getType()) {
		case AND:
		case OR:
		case IFF:
		case XOR: {
			Formulas<typename Formula::PolynomialType> newSubformulas;
			bool changed = false;
			for (const auto& cur: formula.subformulas()) {
				Formula newCur = visitResult(cur, func);
				if (newCur != cur) changed = true;
				newSubformulas.push_back(newCur);
			}
			if (changed) {
				newFormula = Formula(formula.getType(), newSubformulas);
			}
			break;
		}
		case NOT: {
			Formula cur = visitResult(formula.subformula(), func);
			if (cur != formula.subformula()) {
				newFormula = Formula(NOT, cur);
			}
			break;
		}
		case IMPLIES: {
			Formula prem = visitResult(formula.premise(), func);
			Formula conc = visitResult(formula.conclusion(), func);
			if ((prem != formula.premise()) || (conc != formula.conclusion())) {
				newFormula = Formula(IMPLIES, {prem, conc});
			}
			break;
		}
		case ITE: {
			Formula cond = visitResult(formula.condition(), func);
			Formula fCase = visitResult(formula.firstCase(), func);
			Formula sCase = visitResult(formula.secondCase(), func);
			if ((cond != formula.condition()) || (fCase != formula.firstCase()) || (sCase != formula.secondCase())) {
				newFormula = Formula(ITE, {cond, fCase, sCase});
			}
			break;
		}
		case BOOL:
		case CONSTRAINT:
		case VARCOMPARE:
		case VARASSIGN:
		case BITVECTOR:
		case TRUE:
		case FALSE:
		case UEQ:
		case PBCONSTRAINT:
			break;
		case EXISTS:
		case FORALL: {
			Formula sub = visitResult(formula.quantifiedFormula(), func);
			if (sub != formula.quantifiedFormula()) {
				newFormula = Formula(formula.getType(), formula.quantifiedVariables(), sub);
			}
			break;
		}
		} 
		return func(newFormula);
	}
    
	template<typename Formula>
    Formula FormulaSubstitutor<Formula>::substitute(const Formula& formula, const std::map<Formula,Formula>& replacements) {
        Substitutor subs(replacements);
        return visitor.visitResult(formula, std::function<Formula(Formula)>(subs));
    }
    template<typename Formula>
    Formula FormulaSubstitutor<Formula>::substitute(const Formula& formula, const std::map<Variable,typename Formula::PolynomialType>& replacements) {
        PolynomialSubstitutor subs(replacements);
        return visitor.visitResult(formula, std::function<Formula(Formula)>(subs));
    }
    template<typename Formula>
    Formula FormulaSubstitutor<Formula>::substitute(const Formula& formula, const std::map<BVVariable,BVTerm>& replacements) {
        BitvectorSubstitutor subs(replacements);
        return visitor.visitResult(formula, std::function<Formula(Formula)>(subs));
    }
    template<typename Formula>
    Formula FormulaSubstitutor<Formula>::substitute(const Formula& formula, const std::map<UVariable,UFInstance>& replacements) {
        UninterpretedSubstitutor subs(replacements);
        return visitor.visitResult(formula, std::function<Formula(Formula)>(subs));
    }
}    // namespace carl
