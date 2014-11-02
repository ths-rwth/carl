/**
 * @file Ast.tpp
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 * @author Ulrich Loup
 * @author Sebastian Junges
 * @since 2012-02-09
 * @version 2014-10-30
 */


#include "Ast.h"
#include "AstPool.h"

using namespace std;

namespace carl
{
    template<typename Pol>
    Ast<Pol>::Ast( bool _true, size_t _id ):
        mDeducted( false ),
        mHash( ((size_t)(_true ? constraintPool<Pol>().consistentConstraint()->id() : constraintPool<Pol>().inconsistentConstraint()->id())) << (sizeof(size_t)*4) ),
        mId( _id ),
        mActivity( 0 ),
        mDifficulty( 0 ),
        mType( _true ? TRUE : FALSE ),
        mpConstraint( _true ? constraintPool<Pol>().consistentConstraint() : constraintPool<Pol>().inconsistentConstraint() ),
        mProperties()
    {}
    
    template<typename Pol>
    Ast<Pol>::Ast( Variable::Arg _boolean ):
        mDeducted( false ),
        mHash( (size_t)_boolean.getId() ), // TODO: subtract the id of the boolean variable with the smallest id
        mId( 0 ),
        mActivity( 0 ),
        mDifficulty( 0 ),
        mType( BOOL ),
        mBoolean( _boolean ),
        mProperties()
    {
        assert( _boolean.getType() == VariableType::VT_BOOL );
    }

    template<typename Pol>
    Ast<Pol>::Ast( const Constraint<Pol>* _constraint ):
        mDeducted( false ),
        mHash( ((size_t) _constraint->id()) << (sizeof(size_t)*4) ),
        mId( 0 ),
        mActivity( 0 ),
        mDifficulty( 0 ),
        mType( CONSTRAINT ),
        mpConstraint( _constraint ),
        mProperties()
    {
        switch( _constraint->isConsistent() )
        {
            case 0: 
                assert( mpConstraint == constraintPool<Pol>().inconsistentConstraint() );
                mType = FALSE;
                break;
            case 1: 
                assert( mpConstraint == constraintPool<Pol>().consistentConstraint() );
                mType = TRUE;
                break;
            default:
            {}
        }
    }

    template<typename Pol>
    Ast<Pol>::Ast( UEquality&& _ueq ):
        mDeducted( false ),
        mHash( std::hash<UEquality>()( _ueq ) ),
        mId( 0 ),
        mActivity( 0 ),
        mDifficulty( 0 ),
        mType( UEQ ),
        mUIEquality( std::move( _ueq ) ),
        mProperties()
    {}
         
    template<typename Pol>
    Ast<Pol>::Ast( const Ast<Pol>* _subast ):
        mDeducted( false ),
        mHash( ((size_t)NOT << 5) ^ _subast->getHash() ),
        mId( 0 ),
        mActivity( 0 ),
        mDifficulty( 0 ),
        mType( NOT ),
        mProperties()
    {
        mpSubast = _subast;
    }

    template<typename Pol>
    Ast<Pol>::Ast( const Ast<Pol>* _premise, const Ast<Pol>* _conclusion ):
        mDeducted( false ),
        mHash( CIRCULAR_SHIFT(size_t, (((size_t)IMPLIES << 5) ^ _premise->getHash()), 5) ^ _conclusion->getHash() ),
        mId( 0 ),
        mActivity( 0 ),
        mDifficulty( 0 ),
        mType( IMPLIES ),
        mProperties()
    {
        mpImpliesContent = new IMPLIESContent( _premise, _conclusion );
    }

    template<typename Pol>
    Ast<Pol>::Ast( const Ast<Pol>* _conditon, const Ast<Pol>* _then, const Ast<Pol>* _else ):
        mDeducted( false ),
        mHash( CIRCULAR_SHIFT(size_t, (CIRCULAR_SHIFT(size_t, (((size_t)ITE << 5) ^ _conditon->getHash()), 5) ^ _then->getHash()), 5) ^ _else->getHash() ),
        mId( 0 ),
        mActivity( 0 ),
        mDifficulty( 0 ),
        mType( ITE ),
        mProperties()
    {
        mpIteContent = new ITEContent( _conditon, _then, _else );
    }

    template<typename Pol>
    Ast<Pol>::Ast(const Type _type, const std::vector<Variable>&& _vars, const Ast<Pol>* _term):
        mDeducted( false ),
        ///@todo Construct reasonable hash
        mHash( _term->getHash() ),
        mId( 0 ),
        mActivity( 0 ),
        mDifficulty( 0 ),
        mType( _type ),
        mProperties()
    {
        assert(_type == EXISTS || _type == FORALL);
        mpQuantifierContent = new QuantifierContent(std::move(_vars), _term);
    }
    
    template<typename Pol>
    Ast<Pol>::Ast( Type _type, PointerSet<Ast>&& _subasts ):
        mDeducted( false ),
        mHash( (size_t)_type ),
        mId( 0 ),
        mActivity( 0 ),
        mDifficulty( 0 ),
        mType( _type ),
        mProperties()
    {
        assert( _subasts.size() > 1 );
        assert( mType == AND || mType == OR || mType == IFF || mType == XOR );
        mpSubasts = new PointerSet<Ast>( move( _subasts ) );
        for( const Ast<Pol>* subast : *mpSubasts )
        {
            mHash = CIRCULAR_SHIFT(size_t, mHash, 5);
            mHash ^= subast->getHash();
        }
    }

    template<typename Pol>
    Ast<Pol>::~Ast()
    {
        if( isNary() )
        {
            mpSubasts->clear();
            delete mpSubasts;
        }
        else if( mType == CONSTRAINT )
        {
            mpConstraint = NULL;
        }
        else if( mType == NOT )
        {
            mpSubast = NULL;
        }
        else if( mType == IMPLIES )
        {
            delete mpImpliesContent;
        }
        else if( mType == ITE )
        {
            delete mpIteContent;
        }
    }
    
    template<typename Pol>
    void Ast<Pol>::collectVariables( Variables& _vars, VariableType _type, bool _ofThisType ) const
    {
        switch( mType )
        {
            case BOOL:
                if( _ofThisType == (_type == VariableType::VT_BOOL) )
                {
                    _vars.insert( mBoolean );
                }
                break;
            case TRUE:
                break;
            case FALSE:
                break;
            case CONSTRAINT:
                if( !(_ofThisType && _type == VariableType::VT_BOOL) ) // NOTE: THIS ASSUMES THAT THE VARIABLES IN THE CONSTRAINT HAVE INFINTE DOMAINS
                {
                    for( auto var : mpConstraint->variables() )
                    {
                        if( _ofThisType == (var.getType() == VariableType::VT_INT) )
                            _vars.insert( var );
                        if( _ofThisType == (var.getType() == VariableType::VT_REAL) )
                            _vars.insert( var );
                    }
                }
                break;
            case NOT:
                mpSubast->collectVariables( _vars, _type, _ofThisType );
                break;
            case IMPLIES:
                premise().collectVariables( _vars, _type, _ofThisType );
                conclusion().collectVariables( _vars, _type, _ofThisType );
                break;
            case ITE:
                condition().collectVariables( _vars, _type, _ofThisType );
                firstCase().collectVariables( _vars, _type, _ofThisType );
                secondCase().collectVariables( _vars, _type, _ofThisType );
                break;
            case EXISTS:
            case FORALL:
                quantifiedAst().collectVariables(_vars, _type, _ofThisType);
                break;
            default:
            {
                for( const Ast<Pol>* subast : *mpSubasts )
                    subast->collectVariables( _vars, _type, _ofThisType );
            }
        }
    }
    
    template<typename Pol>
    bool Ast<Pol>::operator==( const Ast<Pol>& _ast ) const
    {
        if( mId == 0 || _ast.getId() == 0 )
        {
            if( mType != _ast.getType() )
                return false;
            switch( mType )
            {
                case BOOL:
                    return mBoolean == _ast.boolean();
                case TRUE:
                    return true;
                case FALSE:
                    return true;
                case CONSTRAINT:
                    return (*mpConstraint) == _ast.constraint();
                case NOT:
                    return (*mpSubast) == _ast.subast();
                case IMPLIES:
                    return premise() == _ast.premise() && conclusion() == _ast.conclusion();
                case ITE:
                    return condition() == _ast.condition() && firstCase() == _ast.firstCase() && secondCase() == _ast.secondCase();
                case Type::EXISTS:
                    return (*this->mpQuantifierContent == *_ast.mpQuantifierContent);
                case Type::FORALL:
                    return (*this->mpQuantifierContent == *_ast.mpQuantifierContent);
				case Type::UEQ:
					return (this->mUIEquality == _ast.mUIEquality);
                default:
                    return (*mpSubasts) == _ast.subasts();
            }
        }
        else
            return mId == _ast.getId();
    }
    
    template<typename Pol>
    unsigned Ast<Pol>::satisfiedBy( const EvaluationMap<typename Pol::NumberType>& _assignment ) const
    {
        switch( mType )
        {
            case TRUE:
            {
                return 1;
            }
            case FALSE:
            {
                return 0;
            }
            case BOOL:
            {
                auto ass = _assignment.find( mBoolean );
                return ass == _assignment.end() ? 2 : (ass->second == typename Pol::NumberType( 1 ) ? 1 : 0);
            }
            case CONSTRAINT:
            {
                return mpConstraint->satisfiedBy( _assignment );
            }
            case NOT:
            {
                switch( mpSubast->satisfiedBy( _assignment ) )
                {
                    case 0:
                        return 1;
                    case 1:
                        return 0;
                    default:
                        return 2;
                }   
            }
            case OR:
            {
                unsigned result = 0;
                for( auto subAst = mpSubasts->begin(); subAst != mpSubasts->end(); ++subAst )
                {
                    switch( (*subAst)->satisfiedBy( _assignment ) )
                    {
                        case 0:
                            break;
                        case 1:
                            return 1;
                        default:
                            if( result != 2 ) result = 2;
                    }
                }
                return result;
            }
            case AND:
            {
                unsigned result = 1;
                for( auto subAst = mpSubasts->begin(); subAst != mpSubasts->end(); ++subAst )
                {
                    switch( (*subAst)->satisfiedBy( _assignment ) )
                    {
                        case 0:
                            return 0;
                        case 1:
                            break;
                        default:
                            if( result != 2 ) result = 2;
                    }
                }
                return result;
            }
            case IMPLIES:
            {
                unsigned result = premise().satisfiedBy( _assignment );
                if( result == 0 ) return 1;
                switch( conclusion().satisfiedBy( _assignment ) )
                {
                    case 0:
                        return result == 1 ? 0 : 2;
                    case 1:
                        return 1;
                    default:
                        return 2;
                }
            }
            case ITE:
            {
                unsigned result = condition().satisfiedBy( _assignment );
                switch( result )
                {
                    case 0:
                        return secondCase().satisfiedBy( _assignment );
                    case 1:
                        return firstCase().satisfiedBy( _assignment );
                    default:
                        return 2;
                }
            }
            case IFF:
            {
                auto subAst = mpSubasts->begin();
                unsigned result = (*subAst)->satisfiedBy( _assignment );
                bool containsTrue = (result == 1 ? true : false);
                bool containsFalse = (result == 0 ? true : false);
                ++subAst;
                while( subAst != mpSubasts->end() )
                {
                    unsigned resultTmp = (*subAst)->satisfiedBy( _assignment );
                    switch( resultTmp )
                    {
                        case 0:
                            containsFalse = true;
                            break;
                        case 1:
                            containsTrue = true;
                        default:
                            result = 2;
                    }
                    if( containsFalse && containsTrue )
                        return 0;
                    ++subAst;
                }
                return (result == 2 ? 2 : 1);
            }
            case XOR:
            {
                auto subAst = mpSubasts->begin();
                unsigned result = (*subAst)->satisfiedBy( _assignment );
                if( result == 2 ) return 2;
                ++subAst;
                while( subAst != mpSubasts->end() )
                {
                    unsigned resultTmp = (*subAst)->satisfiedBy( _assignment );
                    if( resultTmp == 2 ) return 2;
                    result = resultTmp != result;
                    ++subAst;
                }
                return result;
            }
            case EXISTS:
            {
                ///@todo do something here
                return 2;
                break;
            }
            case FORALL:
            {
                ///@todo do something here
                return 2;
                break;
            }
            default:
            {
                assert( false );
                cerr << "Undefined operator!" << endl;
                return 2;
            }
        }
    }
    
    template<typename Pol>
    void Ast<Pol>::initProperties()
    {
        mProperties = Condition();
        switch( mType )
        {
            case TRUE:
            {
                mProperties |= STRONG_CONDITIONS;
                addConstraintProperties( *mpConstraint );
                break;
            }
            case FALSE:
            {
                mProperties |= STRONG_CONDITIONS;
                addConstraintProperties( *mpConstraint );
                break;
            }
            case BOOL:
            {
                mProperties |= STRONG_CONDITIONS | PROP_CONTAINS_BOOLEAN;
                break;
            }
            case CONSTRAINT:
            {
                mProperties |= STRONG_CONDITIONS;
                addConstraintProperties( *mpConstraint );
                break;
            }
            case NOT:
            {
                Condition subAstConds = mpSubast->properties();
                if( PROP_IS_AN_ATOM <= subAstConds )
                    mProperties |= PROP_IS_A_CLAUSE | PROP_IS_A_LITERAL | PROP_IS_IN_CNF | PROP_IS_PURE_CONJUNCTION;
                mProperties |= (subAstConds & PROP_VARIABLE_DEGREE_LESS_THAN_THREE);
                mProperties |= (subAstConds & PROP_VARIABLE_DEGREE_LESS_THAN_FOUR);
                mProperties |= (subAstConds & PROP_VARIABLE_DEGREE_LESS_THAN_FIVE);
                mProperties |= (subAstConds & WEAK_CONDITIONS);
                break;
            }
            case OR:
            {
                mProperties |= PROP_IS_A_CLAUSE | PROP_IS_IN_CNF | PROP_IS_IN_NNF;
                mProperties |= PROP_VARIABLE_DEGREE_LESS_THAN_THREE | PROP_VARIABLE_DEGREE_LESS_THAN_FOUR | PROP_VARIABLE_DEGREE_LESS_THAN_FIVE;
                for( auto subAst = mpSubasts->begin(); subAst != mpSubasts->end(); ++subAst )
                {
                    Condition subAstConds = (*subAst)->properties();
                    if( !(PROP_IS_A_LITERAL<=subAstConds) )
                    {
                        mProperties &= ~PROP_IS_A_CLAUSE;
                        mProperties &= ~PROP_IS_IN_CNF;
                    }
                    if( !(PROP_IS_IN_NNF<=subAstConds) )
                        mProperties &= ~PROP_IS_IN_NNF;
                    mProperties |= (subAstConds & WEAK_CONDITIONS);
                }
                break;
            }
            case AND:
            {
                mProperties |= PROP_IS_PURE_CONJUNCTION | PROP_IS_IN_CNF | PROP_IS_IN_NNF;
                mProperties |= PROP_VARIABLE_DEGREE_LESS_THAN_THREE | PROP_VARIABLE_DEGREE_LESS_THAN_FOUR | PROP_VARIABLE_DEGREE_LESS_THAN_FIVE;
                for( auto subAst = mpSubasts->begin(); subAst != mpSubasts->end(); ++subAst )
                {
                    Condition subAstConds = (*subAst)->properties();
                    if( !(PROP_IS_A_CLAUSE<=subAstConds) )
                    {
                        mProperties &= ~PROP_IS_PURE_CONJUNCTION;
                        mProperties &= ~PROP_IS_IN_CNF;
                    }
                    else if( !(PROP_IS_A_LITERAL<=subAstConds) )
                        mProperties &= ~PROP_IS_PURE_CONJUNCTION;
                    if( !(PROP_IS_IN_NNF<=subAstConds) )
                        mProperties &= ~PROP_IS_IN_NNF;
                    mProperties |= (subAstConds & WEAK_CONDITIONS);
                }
                break;
            }
            case IMPLIES:
            {
                mProperties |= PROP_IS_IN_NNF;
                mProperties |= PROP_VARIABLE_DEGREE_LESS_THAN_THREE | PROP_VARIABLE_DEGREE_LESS_THAN_FOUR | PROP_VARIABLE_DEGREE_LESS_THAN_FIVE;
                Condition subAstCondsA = premise().properties();
                if( !(PROP_IS_IN_NNF<=subAstCondsA) )
                    mProperties &= ~PROP_IS_IN_NNF;
                mProperties |= (subAstCondsA & WEAK_CONDITIONS);
                Condition subAstCondsB = conclusion().properties();
                if( !(PROP_IS_IN_NNF<=subAstCondsB) )
                    mProperties &= ~PROP_IS_IN_NNF;
                mProperties |= (subAstCondsB & WEAK_CONDITIONS);
                break;
            }
            case ITE:
            {
                mProperties |= PROP_VARIABLE_DEGREE_LESS_THAN_THREE | PROP_VARIABLE_DEGREE_LESS_THAN_FOUR | PROP_VARIABLE_DEGREE_LESS_THAN_FIVE;
                mProperties |= (condition().properties() & WEAK_CONDITIONS);
                mProperties |= (firstCase().properties() & WEAK_CONDITIONS);
                mProperties |= (secondCase().properties() & WEAK_CONDITIONS);
                break;
            }
            case IFF:
            {
                mProperties |= PROP_IS_IN_NNF;
                mProperties |= PROP_VARIABLE_DEGREE_LESS_THAN_THREE | PROP_VARIABLE_DEGREE_LESS_THAN_FOUR | PROP_VARIABLE_DEGREE_LESS_THAN_FIVE;
                for( auto subAst = mpSubasts->begin(); subAst != mpSubasts->end(); ++subAst )
                {
                    Condition subAstConds = (*subAst)->properties();
                    if( !(PROP_IS_IN_NNF<=subAstConds) )
                        mProperties &= ~PROP_IS_IN_NNF;
                    mProperties |= (subAstConds & WEAK_CONDITIONS);
                }
                break;
            }
            case XOR:
            {
                mProperties |= PROP_IS_IN_NNF;
                mProperties |= PROP_VARIABLE_DEGREE_LESS_THAN_THREE | PROP_VARIABLE_DEGREE_LESS_THAN_FOUR | PROP_VARIABLE_DEGREE_LESS_THAN_FIVE;
                for( auto subAst = mpSubasts->begin(); subAst != mpSubasts->end(); ++subAst )
                {
                    Condition subAstConds = (*subAst)->properties();
                    if( !(PROP_IS_IN_NNF<=subAstConds) )
                        mProperties &= ~PROP_IS_IN_NNF;
                    mProperties |= (subAstConds & WEAK_CONDITIONS);
                }
                break;
            }
            case EXISTS:
            {
                ///@todo do something here
                break;
            }
            case FORALL:
            {
                ///@todo do something here
                break;
            }
            case UEQ:
            {
                mProperties |= STRONG_CONDITIONS | PROP_CONTAINS_UNINTERPRETED_EQUATIONS;
                break;
            }
            default:
            {
                assert( false );
                cerr << "Undefined operator!" << endl;
            }
        }
    }
    
    template<typename Pol>
    void Ast<Pol>::addConstraintProperties( const Constraint<Pol>& _constraint )
    {
        if( _constraint.lhs().isZero() )
        {
            mProperties |= PROP_CONTAINS_LINEAR_POLYNOMIAL;
        }
        else
        {
            switch( _constraint.lhs().totalDegree() )
            {
                case 0:
                    mProperties |= PROP_CONTAINS_LINEAR_POLYNOMIAL;
                    break;
                case 1:
                    mProperties |= PROP_CONTAINS_LINEAR_POLYNOMIAL;
                    break;
                case 2:
                    mProperties |= PROP_CONTAINS_NONLINEAR_POLYNOMIAL;
                    break;
                case 3:
                    mProperties |= PROP_CONTAINS_NONLINEAR_POLYNOMIAL;
                    mProperties &= ~PROP_VARIABLE_DEGREE_LESS_THAN_THREE;
                    break;
                case 4:
                    mProperties |= PROP_CONTAINS_NONLINEAR_POLYNOMIAL;
                    mProperties &= ~PROP_VARIABLE_DEGREE_LESS_THAN_FOUR;
                    break;
                case 5:
                    mProperties |= PROP_CONTAINS_NONLINEAR_POLYNOMIAL;
                    mProperties &= ~PROP_VARIABLE_DEGREE_LESS_THAN_FIVE;
                    break;
                default:
                {
                }
            }
        }
        switch( _constraint.relation() )
        {
            case Relation::EQ:
                mProperties |= PROP_CONTAINS_EQUATION;
                break;
            case Relation::NEQ:
                mProperties |= PROP_CONTAINS_STRICT_INEQUALITY;
                break;
            case Relation::LEQ:
                mProperties |= PROP_CONTAINS_INEQUALITY;
                break;
            case Relation::GEQ:
                mProperties |= PROP_CONTAINS_INEQUALITY;
                break;
            case Relation::LESS:
                mProperties |= PROP_CONTAINS_STRICT_INEQUALITY;
                break;
            case Relation::GREATER:
                mProperties |= PROP_CONTAINS_STRICT_INEQUALITY;
                break;
            default:
            {
            }
        }
        if( _constraint.hasIntegerValuedVariable() )
            mProperties |= PROP_CONTAINS_INTEGER_VALUED_VARS;
        if( _constraint.hasRealValuedVariable() )
            mProperties |= PROP_CONTAINS_REAL_VALUED_VARS;
    }

    template<typename Pol>
    string Ast<Pol>::toString( bool _withActivity, unsigned _resolveUnequal, const string _init, bool _oneline, bool _infix, bool _friendlyNames ) const
    {
        string activity = "";
        if( _withActivity )
        {
            stringstream s;
            s << " [" << mDifficulty << ":" << mActivity << "]";
            activity += s.str();
        }
        if( mType == BOOL )
        {
            return (_init + variablePool().getVariableName( mBoolean, _friendlyNames ) + activity);
        }
        else if( mType == CONSTRAINT )
            return (_init + mpConstraint->toString( _resolveUnequal, _infix, _friendlyNames ) + activity);
        else if( isAtom() )
            return (_init + AstTypeToString( mType ) + activity);
        else if( mType == NOT )
        {
            string result = _init;
            if( _infix )
            {
                result += "not(";
                if( !_oneline ) result += "\n";
            }
            else
            {
                result += "(not";
                result += (_oneline ? " " : "\n");
            }
            result += mpSubast->toString( _withActivity, _resolveUnequal, _oneline ? "" : (_init + "   "), _oneline, _infix, _friendlyNames );
            result += (_oneline ? "" : "\n") + _init + ")";
            return result;
        }
        else if( mType == IMPLIES )
        {
            string result = _init + "(";
            if( _infix )
            {
                if( !_oneline ) 
                    result += "\n";
                result += premise().toString( _withActivity, _resolveUnequal, _oneline ? "" : (_init + "   "), _oneline, true, _friendlyNames );
                result += " " + AstTypeToString( IMPLIES ) + " ";
                if( !_oneline ) 
                    result += "\n";
                result += conclusion().toString( _withActivity, _resolveUnequal, _oneline ? "" : (_init + "   "), _oneline, true, _friendlyNames );
            }
            else
            {
                result += AstTypeToString( IMPLIES );
                result += (_oneline ? " " : "\n");
                result += premise().toString( _withActivity, _resolveUnequal, _oneline ? "" : (_init + "   "), _oneline, false, _friendlyNames );
                result += (_oneline ? " " : "\n");
                result += conclusion().toString( _withActivity, _resolveUnequal, _oneline ? "" : (_init + "   "), _oneline, false, _friendlyNames );
            }
            result += ")";
            if( _withActivity )
                result += activity;
            return result;
        }
        else if( mType == ITE )
        {
            string result = _init + "(";
            if( _infix )
            {
                if( !_oneline ) 
                    result += "\n";
                result += condition().toString( _withActivity, _resolveUnequal, _oneline ? "" : (_init + "   "), _oneline, true, _friendlyNames );
                result += " " + AstTypeToString( ITE ) + " ";
                if( !_oneline ) 
                    result += "\n";
                result += firstCase().toString( _withActivity, _resolveUnequal, _oneline ? "" : (_init + "   "), _oneline, true, _friendlyNames );
                if( !_oneline ) 
                    result += "\n";
                result += secondCase().toString( _withActivity, _resolveUnequal, _oneline ? "" : (_init + "   "), _oneline, true, _friendlyNames );
            }
            else
            {
                result += AstTypeToString( ITE );
                result += (_oneline ? " " : "\n");
                result += condition().toString( _withActivity, _resolveUnequal, _oneline ? "" : (_init + "   "), _oneline, false, _friendlyNames );
                result += (_oneline ? " " : "\n");
                result += firstCase().toString( _withActivity, _resolveUnequal, _oneline ? "" : (_init + "   "), _oneline, false, _friendlyNames );
                result += (_oneline ? " " : "\n");
                result += secondCase().toString( _withActivity, _resolveUnequal, _oneline ? "" : (_init + "   "), _oneline, false, _friendlyNames );
            }
            result += ")";
            if( _withActivity )
                result += activity;
            return result;
        }
        else if (mType == EXISTS)
        {
            string result = _init + "(exists ";
            for (auto v: this->mpQuantifierContent->mVariables) {
                result += variablePool().getVariableName(v, _friendlyNames) + " ";
            }
            result += this->mpQuantifierContent->mpAst->toString(_withActivity, _resolveUnequal, _init, _oneline, _infix, _friendlyNames);
            result += ")";
            return result;
        }
        else if (mType == FORALL)
        {
            string result = _init + "(forall ";
            for (auto v: this->mpQuantifierContent->mVariables) {
                result += variablePool().getVariableName(v, _friendlyNames) + " ";
            }
            result += this->mpQuantifierContent->mpAst->toString(_withActivity, _resolveUnequal, _init, _oneline, _infix, _friendlyNames);
            result += ")";
            return result;
        }
		else if( mType == UEQ )
        {
            std::stringstream ss;
			ss << this->mUIEquality;
			return ss.str();
        }
        assert( mType == AND || mType == OR || mType == IFF || mType == XOR );
        string stringOfType = AstTypeToString( mType );
        string result = _init + "(";
        if( _infix )
        {
            for( auto subast = mpSubasts->begin(); subast != mpSubasts->end(); ++subast )
            {
                if( subast != mpSubasts->begin() )
                    result += " " + stringOfType + " ";
                if( !_oneline ) 
                    result += "\n";
                result += (*subast)->toString( _withActivity, _resolveUnequal, _oneline ? "" : (_init + "   "), _oneline, true, _friendlyNames );
            }
        }
        else
        {
            result += stringOfType;
            for( auto subast = mpSubasts->begin(); subast != mpSubasts->end(); ++subast )
            {
                result += (_oneline ? " " : "\n");
                result += (*subast)->toString( _withActivity, _resolveUnequal, _oneline ? "" : (_init + "   "), _oneline, false, _friendlyNames );
            }
        }
        result += ")";
        if( _withActivity )
            result += activity;
        return result;
    }
    
    template<typename Pol>
    ostream& operator<<( ostream& _ostream, const Ast<Pol>& _ast )
    {
        return (_ostream << _ast.toString( false, 0, "", true, false, true ));
    }

    template<typename Pol>
    void Ast<Pol>::printProposition( ostream& _out, const string _init ) const
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
    string Ast<Pol>::toRedlogFormat( bool _withVariables ) const
    {
        string result = "";
        string oper = Ast<Pol>::AstTypeToString( mType );
        switch( mType )
        {
            // unary cases
            case TRUE:
                result += " " + oper + " ";
                break;
            case FALSE:
                result += " " + oper + " ";
                break;
            case NOT:
                result += " " + oper + "( " + mpSubast->toRedlogFormat( _withVariables ) + " )";
                break;
            case CONSTRAINT:
                result += constraint().toString( 1 );
                break;
            case BOOL:
                result += variablePool().getVariableName( mBoolean, true ) + " = 1";
                break;
            case IMPLIES:
                result += "( " + premise().toRedlogFormat( _withVariables ) + " " + oper + " " + premise().toRedlogFormat( _withVariables ) + " )";
                break;
            default:
            {
                // recursive print of the subasts
                if( _withVariables )
                { // add the variables
                    result += "( ex( {";
                    result += variableListToString( "," );
                    result += "}, (";
                    // Make pseudo Booleans.
                    set<Variable> boolVars = set<Variable>();
                    booleanVars( boolVars );
                    for( auto j = boolVars.begin(); j != boolVars.end(); ++j )
                    {
                        string boolName = variablePool().getVariableName( *j, true );
                        result += "(" + boolName + " = 0 or " + boolName + " = 1) and ";
                    }
                }
                else
                    result += "( ";
                typename PointerSet<Ast>::const_iterator it = mpSubasts->begin();
                // do not quantify variables again.
                result += (*it)->toRedlogFormat( false );
                for( ++it; it != mpSubasts->end(); ++it ) // do not quantify variables again.
                    result += " " + oper + " " + (*it)->toRedlogFormat( false );
                if( _withVariables )
                    result += " ) )";
                result += " )";
            }
        }
        return result;
    }

    template<typename Pol>
    string Ast<Pol>::variableListToString( string _separator, const unordered_map<string, string>& _variableIds ) const
    {
        Variables realVars = Variables();
        realValuedVars( realVars );
        set<Variable> boolVars = set<Variable>();
        booleanVars( boolVars );
        auto i = realVars.begin();
        auto j = boolVars.begin();
        string result = "";
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
            string boolName = variablePool().getVariableName( *j, true );
            unordered_map<string, string>::const_iterator vId = _variableIds.find(boolName);
            result += vId == _variableIds.end() ? boolName : vId->second;
            for( ++j; j != boolVars.end(); ++j )
            {
                boolName = variablePool().getVariableName( *j, true );
                result += _separator;
                vId = _variableIds.find(boolName);
                result += vId == _variableIds.end() ? boolName : vId->second;
            }
        }
        return result;
    }

    template<typename Pol>
    string Ast<Pol>::AstTypeToString( Type _type )
    {
        switch( _type )
        {
            case AND:
                return "and";
            case OR:
                return "or";
            case NOT:
                return "not";
            case IFF:
                return "=";
            case XOR:
                return "xor";
            case IMPLIES:
                return "=>";
            case ITE:
                return "ite";
            case TRUE:
                return "true";
            case FALSE:
                return "false";
            default:
                return "";
        }
    }

    template<typename Pol>
    const Ast<Pol>* Ast<Pol>::resolveNegation( bool _keepConstraint ) const
    {
        if( mType != NOT ) return this;
        Type newType = mType;
        switch( mpSubast->getType() )
        {
            case BOOL:
                return this;
            case UEQ:
                if( _keepConstraint )
                    return this;
                else
                {
                    const UEquality& ueq = mpSubast->uequality();
                    return newEquality<Pol>( ueq.lhs(), ueq.rhs(), !ueq.negated() );
                }
            case CONSTRAINT:
            {
                const Constraint<Pol>* constraint = mpSubast->pConstraint();
                if( _keepConstraint )
                    return this;
                else
                {
                    switch( constraint->relation() )
                    {
                        case Relation::EQ:
                        {
                            return newAst( newConstraint( constraint->lhs(), Relation::NEQ ) );
                        }
                        case Relation::LEQ:
                        {
                            return newAst( newConstraint( -constraint->lhs(), Relation::LESS ) );
                        }
                        case Relation::LESS:
                        {
                            return newAst( newConstraint( -constraint->lhs(), Relation::LEQ ) );
                        }
                        case Relation::GEQ:
                        {
                            return newAst( newConstraint( constraint->lhs(), Relation::LESS ) );
                        }
                        case Relation::GREATER:
                        {
                            return newAst( newConstraint( constraint->lhs(), Relation::LEQ ) );
                        }
                        case Relation::NEQ:
                        {
                            return newAst( newConstraint( constraint->lhs(), Relation::EQ ) );
                        }
                        default:
                        {
                            assert( false );
                            cerr << "Unexpected relation symbol!" << endl;
                            return this;
                        }
                    }
                }
            }
            case TRUE: // (not true)  ->  false
                return falseAst<Pol>();
            case FALSE: // (not false)  ->  true
                return trueAst<Pol>();
            case NOT: // (not (not phi))  ->  phi
                return mpSubast->pSubast();
            case IMPLIES:
            {
                assert( mpSubast->size() == 2 );
                // (not (implies lhs rhs))  ->  (and lhs (not rhs))
                PointerSet<Ast> subasts;
                subasts.insert( mpSubast->pPremise() );
                subasts.insert( newNegation( mpSubast->pConclusion() ) );
                return newAst( AND, move( subasts ) );
            }
            case ITE: // (not (ite cond then else))  ->  (ite cond (not then) (not else))
            {
                return newIte( mpSubast->pCondition(), newNegation( mpSubast->pFirstCase() ), newNegation( mpSubast->pSecondCase() ) );
            }
            case IFF: // (not (iff phi_1 .. phi_n))  ->  (and (or phi_1 .. phi_n) (or (not phi_1) .. (not phi_n)))
            {
                PointerSet<Ast> subastsA;
                PointerSet<Ast> subastsB;
                for( auto subast : mpSubast->subasts() )
                {
                    subastsA.insert( subast );
                    subastsB.insert( newNegation( subast ) );
                }
                return newAst( AND, newAst( OR, move( subastsA ) ), newAst( OR, move( subastsB ) ) );
            }
            case XOR: // (not (xor phi_1 .. phi_n))  ->  (xor (not phi_1) phi_2 .. phi_n)
            {
                auto subast = mpSubast->subasts().begin();
                PointerSet<Ast> subasts;
                subasts.insert( newNegation( *subast ) );
                ++subast;
                for( ; subast != mpSubast->subasts().end(); ++subast )
                    subasts.insert( *subast );
                return newAst( XOR, move( subasts ) );
            }
            case AND: // (not (and phi_1 .. phi_n))  ->  (or (not phi_1) .. (not phi_n))
                newType = OR;
                break;
            case OR: // (not (or phi_1 .. phi_n))  ->  (and (not phi_1) .. (not phi_n))
                newType = AND;
                break;
            case EXISTS: // (not (exists (vars) phi)) -> (forall (vars) (not phi))
                break;
                newType = FORALL;
            case FORALL: // (not (forall (vars) phi)) -> (exists (vars) (not phi))
                break;
                newType = EXISTS;
            default:
                assert( false );
                cerr << "Unexpected type of ast!" << endl;
                return this;
        }
        assert( newType != mpSubast->getType() );
        assert( mpSubast->getType() == AND || mpSubast->getType() == OR );
        PointerSet<Ast> subasts;
        for( const Ast<Pol>* subsubast : mpSubast->subasts() )
            subasts.insert( newNegation( subsubast ) );
        return newAst( newType, move( subasts ) );
    }
    
    template<typename Pol>
    const Ast<Pol>* Ast<Pol>::connectPrecedingSubasts() const
    {
        assert( isNary() );
        if( mpSubasts->size() > 2 )
        {
            PointerSet<Ast> tmpSubasts;
            auto iter = mpSubasts->rbegin();
            ++iter;
            for( ; iter != mpSubasts->rend(); ++iter )
                tmpSubasts.insert( *iter );
            return newAst( mType, tmpSubasts );
        }
        else
        {
            assert( mpSubasts->size() == 2 );
            return *(mpSubasts->begin());
        }
    }

    template<typename Pol>
    const Ast<Pol>* Ast<Pol>::toQF(QuantifiedVariables& variables, unsigned level, bool negated) const
    {
        const Ast<Pol>* res;
        switch (this->mType) {
            case Type::AND:
            case Type::IFF:
            case Type::OR:
            case Type::XOR:
            {
                if (!negated) {
                    PointerSet<Ast> subs;
                    for (auto sub: *this->mpSubasts) {
                        subs.insert(sub->toQF(variables, level, false));
                    }
                    res = newAst(this->mType, std::move(subs));
                } else if (this->mType == Type::AND || this->mType == Type::OR) {
                    PointerSet<Ast> subs;
                    for (auto sub: *this->mpSubasts) {
                        subs.insert(sub->toQF(variables, level, true));
                    }
                    if (this->mType == Type::AND) res = newAst(Type::OR, std::move(subs));
                    else res = newAst(Type::AND, std::move(subs));
                } else if (this->mType == Type::IFF) {
                    PointerSet<Ast> sub1;
                    PointerSet<Ast> sub2;
                    for (auto sub: *this->mpSubasts) {
                        sub1.insert(sub->toQF(variables, level, true));
                        sub2.insert(sub->toQF(variables, level, false));
                    }
                    res = newAst(Type::AND, newAst(Type::OR, std::move(sub1)), newAst(Type::OR, std::move(sub2)));
                } else if (this->mType == Type::XOR) {
                    auto lhs = this->back()->toQF(variables, level, false);
                    auto rhs = this->connectPrecedingSubasts()->toQF(variables, level, true);
                    res = newAst(Type::IFF, lhs, rhs);
                } else assert(false);
                break;
            }
            case Type::BOOL:
            case Type::CONSTRAINT:
            case Type::FALSE:
            case Type::UEQ:
            case Type::TRUE:
            {
                if (negated) res = newNegation(this);
                else res = this;
                break;
            }
            case Type::EXISTS:
            case Type::FORALL:
            {
                unsigned cur = 0;
                if ((level % 2 == (mType == Type::EXISTS ? (unsigned)0 : (unsigned)1)) xor negated) cur = level;
                else cur = level+1;
                Variables vars(this->quantifiedVariables().begin(), this->quantifiedVariables().end());
                const Ast<Pol>* f = this->pQuantifiedAst();
                for (auto it = vars.begin(); it != vars.end();) {
                    if (it->getType() == VariableType::VT_BOOL) {
                        // Just leave boolean variables at the base level up to the SAT solver.
                        if (cur > 0) {
                            f = newAst(
                                (mType == Type::EXISTS ? Type::OR : Type::AND),
                                f->substitute({{*it, trueAst<Pol>()}}),
                                f->substitute({{*it, falseAst<Pol>()}})
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
                res = f->toQF(variables, cur, negated);
                break;
            }
            case Type::IMPLIES:
                if (negated) res = newAst(Type::AND, pPremise()->toQF(variables, level, false), pConclusion()->toQF(variables, level, true));
                else res = newImplication(pPremise()->toQF(variables, level, false), pConclusion()->toQF(variables, level, false));
                break;
            case Type::ITE:
                res = newIte(pCondition()->toQF(variables, level, negated), pFirstCase()->toQF(variables, level, negated), pSecondCase()->toQF(variables, level, negated));
                break;
            case Type::NOT:
                res = this->pSubast()->toQF(variables, level, !negated);
                break;
        }
        return res;
    }

    template<typename Pol>
    const Ast<Pol>* Ast<Pol>::toCNF( bool _keepConstraints, bool _simplifyConstraintCombinations ) const
    {
        if( !_simplifyConstraintCombinations && propertyHolds( PROP_IS_IN_CNF ) )
        {
            if( _keepConstraints )
                return this;
            else if( mType == NOT )
            {
                assert( propertyHolds( PROP_IS_A_LITERAL ) );
                return resolveNegation( _keepConstraints );
            }
        }
        else if( isAtom() )
            return this;
        PointerMap<Ast,pair<const Ast<Pol>*,const Ast<Pol>*>*> tseitinVars;
        PointerSet<Ast> resultSubasts;
        ConstraintBounds constraintBoundsAnd;
        vector<const Ast<Pol>*> subastsToTransform;
        subastsToTransform.push_back( this );
        while( !subastsToTransform.empty() )
        {
            const Ast<Pol>* currentAst = subastsToTransform.back();
//            cout << "To add:" << endl;
//            for( auto f : subastsToTransform )
//                cout << "   " << *f << endl;
//            cout << endl;
//            cout << "Conjunction:" << endl;
//            for( auto f : resultSubasts )
//                cout << "   " << *f << endl;
//            cout << endl;
            subastsToTransform.pop_back();
            switch( currentAst->getType() )
            {
                case BOOL:
                {
                    resultSubasts.insert( currentAst );
                    break;
                }
                case UEQ:
                {
                    resultSubasts.insert( currentAst );
                    break;
                }
                case CONSTRAINT:
                {   
                    if( _simplifyConstraintCombinations )
                    {
                        if( addConstraintBound( constraintBoundsAnd, currentAst, true ) == NULL )
                        {
                            goto ReturnFalse;
                        }
                    }
                    else
                        resultSubasts.insert( currentAst );
                    break;
                }
                case TRUE: // Remove it.
                    break;
                case FALSE: // Return false.
                    goto ReturnFalse;
                case NOT: // Try to resolve this negation.
                {
                    const Ast<Pol>* resolvedAst = currentAst->resolveNegation( _keepConstraints );
                    if( resolvedAst->propertyHolds( PROP_IS_A_LITERAL ) ) // It is a literal.
                    {
                        if( resolvedAst->getType() == CONSTRAINT || (resolvedAst->getType() == NOT && resolvedAst->subast().getType() == CONSTRAINT) )
                        {
                            if( _simplifyConstraintCombinations )
                            {
                                if( addConstraintBound( constraintBoundsAnd, resolvedAst, true ) == NULL )
                                {
                                    goto ReturnFalse;
                                }
                            }
                            else
                            {
                                resultSubasts.insert( resolvedAst );
                            }
                        }
                        else
                        {
                            resultSubasts.insert( resolvedAst );
                        }
                    }
                    else
                        subastsToTransform.push_back( resolvedAst );
                    break;
                }
                case AND: // (and phi_1 .. phi_n) -> psi_1 .. psi_m
                {
                    for( const Ast<Pol>* subast : currentAst->subasts() )
                        subastsToTransform.push_back( subast );
                    break;
                }
                case IMPLIES: // (-> lhs rhs)  ->  (or (not lhs) rhs)
                {
                    PointerSet<Ast> tmpSubasts;
                    tmpSubasts.insert( newNegation( currentAst->pPremise() ) );
                    tmpSubasts.insert( currentAst->pConclusion() );
                    subastsToTransform.push_back( newAst( OR, tmpSubasts ) );
                    break;
                }
                case ITE: // (ite cond then else)  ->  auxBool, where (or (not cond) then) and (or cond else) are added to the queue
                {
                    // Add: (or (not cond) then)
                    subastsToTransform.push_back( newAst( OR, newNegation( currentAst->pCondition() ), currentAst->pFirstCase() ) );
                    // Add: (or cond else)
                    subastsToTransform.push_back( newAst( OR, currentAst->pCondition(), currentAst->pSecondCase() ) );
                    break;
                }
                case IFF: 
                {
                    if( currentAst->subasts().size() > 2 )
                    {
                        // (iff phi_1 .. phi_n) -> (or (and phi_1 .. phi_n) (and (not phi_1) .. (not phi_n))) is added to the queue
                        PointerSet<Ast> subastsA;
                        PointerSet<Ast> subastsB;
                        for( auto subast : currentAst->subasts() )
                        {
                            subastsA.insert( subast );
                            subastsB.insert( newNegation( subast ) );
                        }
                        subastsToTransform.push_back( newAst( OR, newAst( AND, move( subastsA ) ), newAst( AND, move( subastsB ) ) ) );
                    }
                    else
                    {
                        // (iff lhs rhs) -> (or lhs (not rhs)) and (or (not lhs) rhs) are added to the queue
                        assert( currentAst->subasts().size() == 2 );
                        // Get lhs and rhs.
                        const Ast<Pol>* lhs = *currentAst->subasts().begin();
                        const Ast<Pol>* rhs = currentAst->back();
                        // add (or lhs (not rhs)) to the queue
                        subastsToTransform.push_back( newAst( OR, lhs, newNegation( rhs ) ) );
                        // add (or (not lhs) rhs) to the queue
                        subastsToTransform.push_back( newAst( OR, newNegation( lhs ), rhs ) );
                    }
                    break;
                }
                case XOR: // (xor lhs rhs) -> (or lhs rhs) and (or (not lhs) (not rhs)) are added to the queue
                {
                    // Get lhs and rhs.
                    const Ast<Pol>* lhs = currentAst->connectPrecedingSubasts();
                    const Ast<Pol>* rhs = currentAst->back();
                    // add (or lhs rhs) to the queue
                    subastsToTransform.push_back( newAst( OR, lhs, rhs) );
                    // add (or (not lhs) (not rhs)) to the queue
                    subastsToTransform.push_back( newAst( OR, newNegation( lhs ), newNegation( rhs ) ) );
                    break;
                }
                // Note, that the following case could be implemented using less code, but it would clearly
                // lead to a worse performance as we would then not benefit from the properties of a disjunction.
                case OR: // (or phi_1 .. phi_n) -> (or psi_1 .. psi_m),  where phi_i is transformed as follows:
                {
                    bool currentAstValid = false;
                    ConstraintBounds constraintBoundsOr;
                    PointerSet<Ast> subsubasts;
                    vector<const Ast<Pol>*> phis;
                    for( const Ast<Pol>* subast : currentAst->subasts() )
                        phis.push_back( subast );
                    vector<const Ast<Pol>*> subastsToTransformTmp;
                    while( !currentAstValid && !phis.empty() )
                    {
                        const Ast<Pol>* currentSubast = phis.back();
//                        cout << "    To add:" << endl;
//                        for( auto f : phis )
//                            cout << "       " << *f << endl;
//                        cout << endl;
//                        cout << "    Disjunction:" << endl;
//                        for( auto f : subsubasts )
//                            cout << "       " << *f << endl;
//                        cout << endl;
                        phis.pop_back();
                        switch( currentSubast->getType() )
                        {
                            case BOOL: // B -> B
                                subsubasts.insert( currentSubast );
                                break;
                            case UEQ: // u -> u
                                subsubasts.insert( currentSubast );
                                break;
                            case TRUE: // remove the entire considered disjunction and everything which has been created by considering it
                                currentAstValid = true;
                                break;
                            case FALSE: // remove it
                                break;
                            case OR: // (or phi_i1 .. phi_ik) -> phi_i1 .. phi_ik
                                for( const Ast<Pol>* subsubast : currentSubast->subasts() )
                                    phis.push_back( subsubast );
                                break;
                            case IMPLIES: // (-> lhs_i rhs_i) -> (not lhs_i) rhs_i
                                phis.push_back( newNegation( currentSubast->pPremise() ) );
                                phis.push_back( currentSubast->pConclusion() );
                                break;
                            case ITE: // (ite cond then else)  ->  (and (or (not cond) then) (or cond else))
                            {   
                                PointerSet<Ast> tmpSubasts;
                                // Add: (or (not cond) then)
                                tmpSubasts.insert( newAst( OR, newNegation( currentSubast->pCondition() ), currentSubast->pFirstCase() ) );
                                // Add: (or cond else)
                                tmpSubasts.insert( newAst( OR, currentSubast->pCondition(), currentSubast->pSecondCase() ) );
                                phis.push_back( newAst( AND, tmpSubasts ) );
                                break;
                            }
                            case NOT: // resolve the negation
                            {
                                const Ast<Pol>* resolvedAst = currentSubast->resolveNegation( _keepConstraints );
                                if( resolvedAst->propertyHolds( PROP_IS_A_LITERAL ) ) // It is a literal.
                                {
                                    if( resolvedAst->getType() == CONSTRAINT || (resolvedAst->getType() == NOT && resolvedAst->subast().getType() == CONSTRAINT) )
                                    {
                                        if( _simplifyConstraintCombinations )
                                        {
                                            if( addConstraintBound( constraintBoundsOr, resolvedAst, false ) == NULL )
                                            {
                                                currentAstValid = true;
                                                break;
                                            }
                                        }
                                        else
                                        {
                                            subsubasts.insert( resolvedAst );
                                        }
                                    }
                                    else
                                    {
                                        subsubasts.insert( resolvedAst );
                                    }
                                }
                                else
                                    phis.push_back( resolvedAst );
                                break;
                            }
                            case AND: // (and phi_i1 .. phi_ik) -> h_i, where (or (not h_i) phi_i1) .. (or (not h_i) phi_ik) 
                                      //                                and (or h_i (not phi_i1) .. (not phi_ik))  is added to the queue
                            {
                                bool conjunctionIsFalse = false;
                                ConstraintBounds constraintBoundsOrAnd;
                                PointerSet<Ast> tmpSubSubasts;
                                for( const Ast<Pol>* subsubast : currentSubast->subasts() )
                                {
                                    if( subsubast->getType() == CONSTRAINT || (subsubast->getType() == NOT && subsubast->subast().getType() == CONSTRAINT ) )
                                    {
                                        if( _simplifyConstraintCombinations )
                                        {
                                            if( addConstraintBound( constraintBoundsOrAnd, subsubast, true ) == NULL )
                                            {
                                                conjunctionIsFalse = true;
                                                break;
                                            }
                                        }
                                        else
                                        {
                                            tmpSubSubasts.insert( subsubast );
                                        }
                                    }
                                    else
                                    {
                                        tmpSubSubasts.insert( subsubast );
                                    }
                                }
                                if( conjunctionIsFalse )
                                    break;
                                if( _simplifyConstraintCombinations && swapConstraintBounds( constraintBoundsOrAnd, tmpSubSubasts, true ) )
                                    break;
                                auto iter = tseitinVars.insert( pair<const Ast<Pol>*,pair<const Ast<Pol>*,const Ast<Pol>*>*>( currentSubast, NULL ) );
                                if( iter.second )
                                {
                                    Variable auxVar = newAuxiliaryBooleanVariable();
                                    const Ast<Pol>* hi = newBoolean<Pol>( auxVar );
                                    hi->setDifficulty( currentSubast->difficulty() );
                                    iter.first->second = new pair<const Ast<Pol>*,const Ast<Pol>*>( hi, newNegation( hi ) );
                                }
                                for( const Ast<Pol>* subsubast : tmpSubSubasts )
                                    subastsToTransformTmp.push_back( newAst( OR, iter.first->second->second, subsubast ) );
                                PointerSet<Ast> tmpSubasts;
                                tmpSubasts.insert( iter.first->second->first );
                                for( const Ast<Pol>* subsubast : tmpSubSubasts )
                                    tmpSubasts.insert( newNegation( subsubast ) );
                                subastsToTransformTmp.push_back( newAst( OR, tmpSubasts ) );
                                subsubasts.insert( iter.first->second->first );
                                break;
                            }
                            case CONSTRAINT: // p~0 -> p~0
                            {
                                if( _simplifyConstraintCombinations )
                                {
                                    if( addConstraintBound( constraintBoundsOr, currentSubast, false ) == NULL )
                                    {
                                        currentAstValid = true;
                                        break;
                                    }
                                }
                                else
                                {
                                    subsubasts.insert( currentSubast );
                                }
                                break;
                            }
                            case IFF: // (iff phi_1 .. phi_n) -> (and phi_1 .. phi_n) and (and (not phi_1) .. (not phi_n)) are added to the queue
                            {
                                PointerSet<Ast> subastsA;
                                PointerSet<Ast> subastsB;
                                for( auto subast : currentSubast->subasts() )
                                {
                                    subastsA.insert( subast );
                                    subastsB.insert( newNegation( subast ) );
                                }
                                phis.push_back( newAst( AND, move( subastsA ) ) );
                                phis.push_back( newAst( AND, move( subastsB ) ) );
                                break;
                            }
                            case XOR: // (xor lhs rhs) -> (and lhs (not rhs)) and (and (not lhs) rhs) are added to the queue
                            {
                                // Get lhs and rhs.
                                const Ast<Pol>* lhs = currentSubast->connectPrecedingSubasts();
                                const Ast<Pol>* rhs = currentSubast->back();
                                // add (and lhs (not rhs)) to the queue
                                phis.push_back( newAst( AND, lhs, newNegation( rhs )) );
                                // add (and (not lhs) rhs) to the queue
                                phis.push_back( newAst( AND, newNegation( lhs ), rhs ) );
                                break;
                            }
                            case EXISTS:
                            {
                                assert(false);
                                std::cerr << "Ast must be quantifier-free!" << std::endl;
                                break;
                            }
                            case FORALL:
                            {
                                assert(false);
                                std::cerr << "Ast must be quantifier-free!" << std::endl;
                                break;
                            }
                            default:
                            {
                                assert( false );
                                cerr << "Unexpected type of ast!" << endl;
                            }
                        }
                    }
                    if( !currentAstValid && (!_simplifyConstraintCombinations || !swapConstraintBounds( constraintBoundsOr, subsubasts, false )) )
                    {
                        subastsToTransform.insert( subastsToTransform.end(), subastsToTransformTmp.begin(), subastsToTransformTmp.end() );
                        if( subsubasts.empty() ) // Empty clause = false, which, added to a conjunction, leads to false.
                        {
                            goto ReturnFalse;
                        }
                        else if( subsubasts.size() == 1 )
                        {
                            resultSubasts.insert( *subsubasts.begin() );
                        }
                        else
                        {
                            resultSubasts.insert( newAst( OR, move( subsubasts ) ) );
                        }
                    }
                    break;
                }
                case EXISTS:
                {
                    assert(false);
                    std::cerr << "Ast must be quantifier-free!" << std::endl;
                    break;
                }
                case FORALL:
                {
                    assert(false);
                    std::cerr << "Ast must be quantifier-free!" << std::endl;
                    break;
                }
                default:
                {
                    assert( false );
                    cerr << "Unexpected type of ast!" << endl;
                }
            }
        }
        if( _simplifyConstraintCombinations && swapConstraintBounds( constraintBoundsAnd, resultSubasts, true ) )
            goto ReturnFalse;
        if( resultSubasts.empty() )
            return trueAst<Pol>();
        else if( resultSubasts.size() == 1 )
            return *resultSubasts.begin();
        else
            return newAst( AND, move( resultSubasts ) );
        ReturnFalse:
            while( !tseitinVars.empty() ) // TODO: why only here?
            {
                auto toDel = tseitinVars.begin()->second;
                tseitinVars.erase( tseitinVars.begin() );
                delete toDel;
            }
            return falseAst<Pol>();
    }
            
    template<typename Pol>
    const Ast<Pol>* Ast<Pol>::substitute( const map<Variable, const Ast<Pol>*>& _booleanSubstitutions, const map<Variable, Pol>& _arithmeticSubstitutions ) const
    {
        switch( mType )
        {
            case TRUE:
            {
                return this;
            }
            case FALSE:
            {
                return this;
            }
            case BOOL:
            {
                auto iter = _booleanSubstitutions.find( mBoolean );
                if( iter != _booleanSubstitutions.end() )
                {
                    return iter->second;
                }
                return this;
            }
            case CONSTRAINT:
            {
                Pol lhsSubstituted = mpConstraint->lhs().substitute( _arithmeticSubstitutions );
                return newAst( newConstraint( lhsSubstituted, mpConstraint->relation() ) );
            }
            case NOT:
            {
                return newNegation( mpSubast->substitute( _booleanSubstitutions, _arithmeticSubstitutions ) );
            }
            case IMPLIES:
            {
                const Ast<Pol>* premiseSubstituted = premise().substitute( _booleanSubstitutions, _arithmeticSubstitutions );
                const Ast<Pol>* conclusionSubstituted = conclusion().substitute( _booleanSubstitutions, _arithmeticSubstitutions );
                return newImplication( premiseSubstituted, conclusionSubstituted );
            }
            case ITE:
            {
                const Ast<Pol>* conditionSubstituted = condition().substitute( _booleanSubstitutions, _arithmeticSubstitutions );
                const Ast<Pol>* thenSubstituted = firstCase().substitute( _booleanSubstitutions, _arithmeticSubstitutions );
                const Ast<Pol>* elseSubstituted = secondCase().substitute( _booleanSubstitutions, _arithmeticSubstitutions );
                return newIte( conditionSubstituted, thenSubstituted, elseSubstituted );
            }
            case Type::EXISTS:
            case Type::FORALL:
            {
                return newQuantifier(mType, quantifiedVariables(), quantifiedAst().substitute(_booleanSubstitutions, _arithmeticSubstitutions));
            }
            default:
            {
                assert( isNary() );
                PointerSet<Ast> subastsSubstituted;
                for( const Ast<Pol>* subast : subasts() )
                    subastsSubstituted.insert( subast->substitute( _booleanSubstitutions, _arithmeticSubstitutions ) );
                return newAst( mType, subastsSubstituted );
            }
        }
    }
    
//    #define CONSTRAINT_BOUND_DEBUG

    template<typename Pol>
    const Ast<Pol>* Ast<Pol>::addConstraintBound( ConstraintBounds& _constraintBounds, const Ast<Pol>* _constraint, bool _inConjunction )
    {
        #ifdef CONSTRAINT_BOUND_DEBUG
        cout << "add from a " << (_inConjunction ? "conjunction" : "disjunction") << " to " << &_constraintBounds << ":   " << *_constraint << endl;
        #endif
        bool negated = _constraint->getType() == NOT;
        assert( _constraint->getType() == CONSTRAINT || (negated && _constraint->subast().getType() == CONSTRAINT ) );
        const Constraint<Pol>& constraint = negated ? _constraint->subast().constraint() : _constraint->constraint();
        assert( constraint.isConsistent() == 2 );
        typename Pol::NumberType boundValue;
        Relation relation = negated ? Constraint<Pol>::invertRelation( constraint.relation() ) : constraint.relation();
        const Pol& lhs = constraint.lhs();
        Pol* poly = NULL;
        bool multipliedByMinusOne = lhs.lterm()->coeff() < typename Pol::NumberType( 0 );
        if( multipliedByMinusOne )
        {
            boundValue = constraint.constantPart();
            relation = Constraint<Pol>::turnAroundRelation( relation );
            poly = new Pol( -lhs + boundValue );
        }
        else
        {
            boundValue = -constraint.constantPart();
            poly = new Pol( lhs + boundValue );
        }
        typename Pol::NumberType cf( poly->coprimeFactor() );
        assert( cf > 0 );
        boundValue *= cf;
        (*poly) *= cf;
        #ifdef CONSTRAINT_BOUND_DEBUG
        cout << "try to add the bound  " << Constraint<Pol>::relationToString( relation ) << boundValue << "  for the polynomial  " << *poly << endl; 
        #endif
        auto resA = _constraintBounds.insert( make_pair( poly, std::move( map<typename Pol::NumberType, pair<Relation, const Ast<Pol>*>>() ) ) );
        if( !resA.second )
        {
            delete poly;
        }
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
                        return NULL;
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
                            resB.first->second.second = newAst( newConstraint( lhs, multipliedByMinusOne ? Relation::GEQ : Relation::LEQ ) );
                            return resB.first->second.second;
                        case Relation::GREATER:
                            resB.first->second.first = Relation::GEQ;
                            resB.first->second.second = newAst( newConstraint( lhs, multipliedByMinusOne ? Relation::LEQ : Relation::GEQ ) );
                            return resB.first->second.second;
                        default:
                            assert( resB.first->second.first == Relation::NEQ );
                            return NULL;
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
                            resB.first->second.second = newAst( newConstraint( lhs, Relation::EQ ) );
                            return resB.first->second.second;
                        case Relation::LESS:
                            return resB.first->second.second;
                        case Relation::GREATER:
                            return NULL;
                        default:
                            assert( resB.first->second.first == Relation::NEQ );
                            resB.first->second.first = Relation::LESS;
                            resB.first->second.second = newAst( newConstraint( lhs, multipliedByMinusOne ? Relation::GREATER : Relation::LESS ) );
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
                            return NULL;
                        case Relation::LESS:
                            resB.first->second.first = Relation::LEQ;
                            resB.first->second.second = _constraint;
                            return resB.first->second.second;
                        case Relation::GREATER:
                            return NULL;
                        default:
                            assert( resB.first->second.first == Relation::NEQ );
                            return NULL;
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
                            resB.first->second.second = newAst( newConstraint( lhs, Relation::EQ ) );
                            return resB.first->second.second;
                        case Relation::LESS:
                            return NULL;
                        case Relation::GREATER:
                            return resB.first->second.second;
                        default:
                            assert( resB.first->second.first == Relation::NEQ );
                            resB.first->second.first = Relation::GREATER;
                            resB.first->second.second = newAst( newConstraint( lhs, multipliedByMinusOne ? Relation::LESS : Relation::GREATER ) );
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
                            return NULL;
                        case Relation::LESS:
                            return NULL;
                        case Relation::GREATER:
                            resB.first->second.first = Relation::GEQ;
                            resB.first->second.second = _constraint;
                            return NULL;
                        default:
                            assert( resB.first->second.first == Relation::NEQ );
                            return NULL;
                    }
                }
            case Relation::LESS:
                if( _inConjunction )
                {
                    switch( resB.first->second.first )
                    {
                        case Relation::EQ:
                            return NULL;
                        case Relation::LEQ:
                            resB.first->second.first = Relation::LESS;
                            resB.first->second.second = _constraint;
                            return resB.first->second.second;
                        case Relation::GEQ:
                            return NULL;
                        case Relation::GREATER:
                            return NULL;
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
                            resB.first->second.second = newAst( newConstraint( lhs, multipliedByMinusOne ? Relation::GEQ : Relation::LEQ ) );
                            return resB.first->second.second;
                        case Relation::LEQ:
                            return resB.first->second.second;
                        case Relation::GEQ:
                            return NULL;
                        case Relation::GREATER:
                            resB.first->second.first = Relation::NEQ;
                            resB.first->second.second = newAst( newConstraint( lhs, Relation::NEQ ) );
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
                            return NULL;
                        case Relation::LEQ:
                            return NULL;
                        case Relation::GEQ:
                            resB.first->second.first = Relation::GREATER;
                            resB.first->second.second = _constraint;
                            return resB.first->second.second;
                        case Relation::LESS:
                            return NULL;
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
                            resB.first->second.second = newAst( newConstraint( lhs, multipliedByMinusOne ? Relation::LEQ : Relation::GEQ ) );
                            return resB.first->second.second;
                        case Relation::LEQ:
                            return NULL;
                        case Relation::GEQ:
                            return resB.first->second.second;
                        case Relation::LESS:
                            resB.first->second.first = Relation::NEQ;
                            resB.first->second.second = newAst( newConstraint( lhs, Relation::NEQ ) );
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
                            return NULL;
                        case Relation::LEQ:
                            resB.first->second.first = Relation::LESS;
                            resB.first->second.second = newAst( newConstraint( lhs, multipliedByMinusOne ? Relation::GREATER : Relation::LESS ) );
                            return resB.first->second.second;
                        case Relation::GEQ:
                            resB.first->second.first = Relation::GREATER;
                            resB.first->second.second = newAst( newConstraint( lhs, multipliedByMinusOne ? Relation::LESS : Relation::GREATER ) );
                            return resB.first->second.second;
                        case Relation::LESS:
                            resB.first->second.first = Relation::LESS;
                            resB.first->second.second = newAst( newConstraint( lhs, multipliedByMinusOne ? Relation::GREATER : Relation::LESS ) );
                            return resB.first->second.second;
                        default:
                            assert( resB.first->second.first == Relation::GREATER );
                            resB.first->second.first = Relation::GREATER;
                            resB.first->second.second = newAst( newConstraint( lhs, multipliedByMinusOne ? Relation::LESS : Relation::GREATER ) );
                            return resB.first->second.second;
                    }
                }
                else
                {
                    switch( resB.first->second.first )
                    {
                        case Relation::EQ:
                            return NULL;
                        case Relation::LEQ:
                            return NULL;
                        case Relation::GEQ:
                            return NULL;
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
    bool Ast<Pol>::swapConstraintBounds( ConstraintBounds& _constraintBounds, PointerSet<Ast>& _intoAsts, bool _inConjunction )
    {
        #ifdef CONSTRAINT_BOUND_DEBUG
        cout << "swap from " << &_constraintBounds << " to a " << (_inConjunction ? "conjunction" : "disjunction") << endl;
        #endif
        while( !_constraintBounds.empty() )
        {
            #ifdef CONSTRAINT_BOUND_DEBUG
            cout << "for the bounds of  " << *_constraintBounds.begin()->first << endl;
            #endif
            const map<typename Pol::NumberType, pair<Relation, const Ast<Pol>*>>& bounds = _constraintBounds.begin()->second;
            assert( !bounds.empty() );
            if( bounds.size() == 1 )
            {
                _intoAsts.insert( bounds.begin()->second.second );
                #ifdef CONSTRAINT_BOUND_DEBUG
                cout << "   just add the only bound" << endl;
                #endif
            }
            else
            {
                auto mostSignificantLowerBound = bounds.end();
                auto mostSignificantUpperBound = bounds.end();
                auto moreSignificantCase = bounds.end();
                PointerSet<Ast> lessSignificantCases;
                auto iter = bounds.begin();
                for( ; iter != bounds.end(); ++iter )
                {
                    #ifdef CONSTRAINT_BOUND_DEBUG
                    cout << "   bound is  " << Constraint::relationToString( iter->second.first ) << iter->first << endl;
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
                                    _intoAsts.insert( iter->second.second );
                                }
                                else
                                {
                                    #ifdef CONSTRAINT_BOUND_DEBUG
                                    cout << "      case: " << __LINE__ << endl;
                                    #endif
                                    lessSignificantCases.insert( iter->second.second );
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
                        if( moreSignificantCase != bounds.end() || mostSignificantUpperBound != bounds.end() )
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
                    _intoAsts.insert( moreSignificantCase->second.second );
                }
                else
                {
                    #ifdef CONSTRAINT_BOUND_DEBUG
                    if( !(_inConjunction || mostSignificantUpperBound == bounds.end() || mostSignificantLowerBound == bounds.end() 
                            || mostSignificantUpperBound->first > mostSignificantLowerBound->first) 
                        || !( !_inConjunction || mostSignificantUpperBound == bounds.end() || mostSignificantLowerBound == bounds.end() 
                             || mostSignificantLowerBound->first > mostSignificantUpperBound->first ) )
                    {
                        cout << "mostSignificantUpperBound:   " << mostSignificantUpperBound->first << "  [" << *mostSignificantUpperBound->second.second << "]" << endl;
                        cout << "mostSignificantLowerBound:   " << mostSignificantLowerBound->first << "  [" << *mostSignificantLowerBound->second.second << "]" << endl;
                    }
                    #endif
                    assert( !_inConjunction || mostSignificantUpperBound == bounds.end() || mostSignificantLowerBound == bounds.end() 
                            || mostSignificantUpperBound->first > mostSignificantLowerBound->first );
                    assert( _inConjunction || mostSignificantUpperBound == bounds.end() || mostSignificantLowerBound == bounds.end() 
                             || mostSignificantLowerBound->first > mostSignificantUpperBound->first );
                    if( mostSignificantUpperBound != bounds.end() )
                        _intoAsts.insert( mostSignificantUpperBound->second.second );
                    if( mostSignificantLowerBound != bounds.end() )
                        _intoAsts.insert( mostSignificantLowerBound->second.second );
                    _intoAsts.insert( lessSignificantCases.begin(), lessSignificantCases.end() );
                }
            }
            const Pol* poly = _constraintBounds.begin()->first;
            _constraintBounds.erase( _constraintBounds.begin() );
            delete poly;
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
            while( !_constraintBounds.empty() )
            {
                const Pol* poly = _constraintBounds.begin()->first;
                _constraintBounds.erase( _constraintBounds.begin() );
                delete poly;
            }
            #ifdef CONSTRAINT_BOUND_DEBUG
            cout << "is " << (_inConjunction ? "invalid" : "valid") << endl << endl;
            #endif
            return true;
        }
    }
}    // namespace carl

