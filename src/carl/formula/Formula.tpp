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
    //string formulaTypeToString( FormulaType _type )2
    
    template<typename Pol>
    FormulaContent<Pol>::FormulaContent( bool _true, size_t _id ):
        mHash( ((size_t)(Constraint<Pol>( _true ).id())) << (sizeof(size_t)*4) ),
        mId( _id ),
        mActivity( 0.0 ),
        mDifficulty( 0.0 ),
        mUsages( 0 ),
        mType( _true ? FormulaType::TRUE : FormulaType::FALSE ),
        mConstraint( Constraint<Pol>( _true ) ),
        mProperties(),
        mTseitinClause( false )
    {}

    template<typename Pol>
    FormulaContent<Pol>::FormulaContent( carl::Variable::Arg _boolean ):
        mHash( (size_t)_boolean.getId() ), // TODO: subtract the id of the boolean variable with the smallest id
        mId( 0 ),
        mActivity( 0.0 ),
        mDifficulty( 0.0 ),
        mUsages( 0 ),
        mType( FormulaType::BOOL ),
        mBoolean( _boolean ),
        mProperties(),
        mTseitinClause( false )
    {
        assert( _boolean.getType() == VariableType::VT_BOOL );
    }

    template<typename Pol>
    FormulaContent<Pol>::FormulaContent( const Constraint<Pol>& _constraint ):
        mHash( ((size_t) _constraint.id()) << (sizeof(size_t)*4) ),
        mId( 0 ),
        mActivity( 0.0 ),
        mDifficulty( 0.0 ),
        mUsages( 0 ),
        mType( FormulaType::CONSTRAINT ),
        mConstraint( _constraint ),
        mProperties(),
        mTseitinClause( false )
    {
        switch( _constraint.isConsistent() )
        {
            case 0: 
                assert( mConstraint == Constraint<Pol>( false ) );
                mType = FormulaType::FALSE;
                break;
            case 1: 
                assert( mConstraint == Constraint<Pol>( true ) );
                mType = FormulaType::TRUE;
                break;
            default:
            {}
        }
    }
    
    template<typename Pol>
    FormulaContent<Pol>::FormulaContent( const BVConstraint& _constraint ):
        mHash( ((size_t) _constraint.id()) << (sizeof(size_t)*4) ),
        mId( 0 ),
        mActivity( 0.0 ),
        mDifficulty( 0.0 ),
        mUsages( 0 ),
        mType( FormulaType::BITVECTOR ),
        mBVConstraint( _constraint ),
        mProperties(),
        mTseitinClause( false )
    {
    }

    template<typename Pol>
    FormulaContent<Pol>::FormulaContent( UEquality&& _ueq ):
        mHash( std::hash<UEquality>()( _ueq ) ),
        mId( 0 ),
        mActivity( 0.0 ),
        mDifficulty( 0.0 ),
        mUsages( 0 ),
        mType( FormulaType::UEQ ),
        mUIEquality( std::move( _ueq ) ),
        mProperties(),
        mTseitinClause( false )
    {}

    template<typename Pol>
    FormulaContent<Pol>::FormulaContent( Formula<Pol>&& _subformula ):
        mHash( ((size_t)NOT << 5) ^ _subformula.getHash() ),
        mId( 0 ),
        mActivity( 0.0 ),
        mDifficulty( 0.0 ),
        mUsages( 0 ),
        mType( FormulaType::NOT ),
        mSubformula( std::move( _subformula ) ),
        mNegation( mSubformula.mpContent ),
        mProperties(),
        mTseitinClause( false )
    {}

    template<typename Pol>
    FormulaContent<Pol>::FormulaContent( const Formula<Pol>& _premise, const Formula<Pol>& _conclusion ):
        mHash( CIRCULAR_SHIFT(size_t, (((size_t)IMPLIES << 5) ^ _premise.getHash()), 5) ^ _conclusion.getHash() ),
        mId( 0 ),
        mActivity( 0.0 ),
        mDifficulty( 0.0 ),
        mUsages( 0 ),
        mType( FormulaType::IMPLIES ),
        mProperties(),
        mTseitinClause( false )
    {
        mpImpliesContent = new IMPLIESContent<Pol>( _premise, _conclusion );
    }

    template<typename Pol>
    FormulaContent<Pol>::FormulaContent( const Formula<Pol>& _condition, const Formula<Pol>& _then, const Formula<Pol>& _else ):
        mHash( CIRCULAR_SHIFT(size_t, (CIRCULAR_SHIFT(size_t, (((size_t)ITE << 5) ^ _condition.getHash()), 5) ^ _then.getHash()), 5) ^ _else.getHash() ),
        mId( 0 ),
        mActivity( 0.0 ),
        mDifficulty( 0.0 ),
        mUsages( 0 ),
        mType( FormulaType::ITE ),
        mProperties(),
        mTseitinClause( false )
    {
        mpIteContent = new ITEContent<Pol>( _condition, _then, _else );
    }

    template<typename Pol>
    FormulaContent<Pol>::FormulaContent(const FormulaType _type, const std::vector<carl::Variable>&& _vars, const Formula<Pol>& _term):
        ///@todo Construct reasonable hash
        mHash( _term.getHash() ),
        mId( 0 ),
        mActivity( 0.0 ),
        mDifficulty( 0.0 ),
        mUsages( 0 ),
        mType( _type ),
        mProperties(),
        mTseitinClause( false )
    {
        assert(_type == FormulaType::EXISTS || _type == FormulaType::FORALL);
        mpQuantifierContent = new QuantifierContent<Pol>(std::move(_vars), _term);
    }

    template<typename Pol>
    FormulaContent<Pol>::FormulaContent( const FormulaType _type, Formulas<Pol>&& _subformulas ):
        mHash( (size_t)_type ),
        mId( 0 ),
        mActivity( 0.0 ),
        mDifficulty( 0.0 ),
        mUsages( 0 ),
        mType( _type ),
        mProperties(),
        mTseitinClause( false )
    {
        assert( _subformulas.size() > 1 );
        assert( mType == FormulaType::AND || mType == FormulaType::OR || mType == FormulaType::IFF || mType == FormulaType::XOR );
        mpSubformulas = new Formulas<Pol>( move( _subformulas ) );
        for( const Formula<Pol>& subformula : *mpSubformulas )
        {
            mHash = CIRCULAR_SHIFT(size_t, mHash, 5);
            mHash ^= subformula.getHash();
        }
    }
    
    template<typename Pol>
    FormulaContent<Pol>::~FormulaContent()
    {
        if( (mType == FormulaType::AND || mType == FormulaType::OR || mType == FormulaType::IFF || mType == FormulaType::XOR) )
        {
            mpSubformulas->clear();
            delete mpSubformulas;
        }
        else if( mType == FormulaType::CONSTRAINT )
        {
            mConstraint = Constraint<Pol>();
        }
        else if( mType == FormulaType::IMPLIES )
        {
            delete mpImpliesContent;
        }
        else if( mType == FormulaType::ITE )
        {
            delete mpIteContent;
        }
    }

    template<typename Pol>
    bool FormulaContent<Pol>::operator==( const FormulaContent& _content ) const
    {
        if( mId == 0 || _content.mId == 0 )
        {
            if( mType != _content.mType )
            {
                return false;
            }
            switch( mType )
            {
                case FormulaType::BOOL:
                    return mBoolean == _content.mBoolean;
                case FormulaType::TRUE:
                    return true;
                case FormulaType::FALSE:
                    return true;
                case FormulaType::CONSTRAINT:
                    return mConstraint == _content.mConstraint;
                case FormulaType::BITVECTOR:
                    return mBVConstraint == _content.mBVConstraint;
                case FormulaType::NOT:
                    return mSubformula == _content.mSubformula;
                case FormulaType::IMPLIES:
                    return mpImpliesContent->mPremise == _content.mpImpliesContent->mPremise 
                            && mpImpliesContent->mConclusion == _content.mpImpliesContent->mConclusion;
                case FormulaType::ITE:
                    return mpIteContent->mCondition == _content.mpIteContent->mCondition
                            && mpIteContent->mThen == _content.mpIteContent->mThen
                            && mpIteContent->mElse == _content.mpIteContent->mElse;
                case FormulaType::EXISTS:
                    return (*mpQuantifierContent == *_content.mpQuantifierContent);
                case FormulaType::FORALL:
                    return (*mpQuantifierContent == *_content.mpQuantifierContent);
                case FormulaType::UEQ:
                    return mUIEquality == _content.mUIEquality;
                default:
                    return (*mpSubformulas) == (*_content.mpSubformulas);
            }
        }
        else
            return mId == _content.mId;
    }
    
    template<typename Pol>
    string FormulaContent<Pol>::toString( bool _withActivity, unsigned _resolveUnequal, const string _init, bool _oneline, bool _infix, bool _friendlyNames ) const
    {
        string activity = "";
        if( _withActivity )
        {
            stringstream s;
            s << " [" << mDifficulty << ":" << mActivity << "]";
            activity += s.str();
        }
        if( mType == FormulaType::BOOL )
        {
            return (_init + VariablePool::getInstance().getName( mBoolean, _friendlyNames ) + activity);
        }
        else if( mType == FormulaType::CONSTRAINT )
            return (_init + mConstraint.toString( _resolveUnequal, _infix, _friendlyNames ) + activity);
        else if (mType == FormulaType::BITVECTOR) {
            return (_init + mBVConstraint.toString(_init, _oneline, _infix, _friendlyNames) + activity);
        }
        else if( mType == FormulaType::UEQ )
        {
            return (_init + mUIEquality.toString( _resolveUnequal, _infix, _friendlyNames ) + activity);
        }
        else if( mType == FormulaType::FALSE || mType == FormulaType::TRUE )
            return (_init + formulaTypeToString( mType ) + activity);
        else if( mType == FormulaType::NOT )
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
            result += mSubformula.toString( _withActivity, _resolveUnequal, _oneline ? "" : (_init + "   "), _oneline, _infix, _friendlyNames );
            result += (_oneline ? "" : "\n") + _init + ")";
            return result;
        }
        else if( mType == FormulaType::IMPLIES )
        {
            string result = _init + "(";
            if( _infix )
            {
                if( !_oneline ) 
                    result += "\n";
                result += mpImpliesContent->mPremise.toString( _withActivity, _resolveUnequal, _oneline ? "" : (_init + "   "), _oneline, true, _friendlyNames );
                result += " " + formulaTypeToString( FormulaType::IMPLIES ) + " ";
                if( !_oneline ) 
                    result += "\n";
                result += mpImpliesContent->mConclusion.toString( _withActivity, _resolveUnequal, _oneline ? "" : (_init + "   "), _oneline, true, _friendlyNames );
            }
            else
            {
                result += formulaTypeToString( FormulaType::IMPLIES );
                result += (_oneline ? " " : "\n");
                result += mpImpliesContent->mPremise.toString( _withActivity, _resolveUnequal, _oneline ? "" : (_init + "   "), _oneline, false, _friendlyNames );
                result += (_oneline ? " " : "\n");
                result += mpImpliesContent->mConclusion.toString( _withActivity, _resolveUnequal, _oneline ? "" : (_init + "   "), _oneline, false, _friendlyNames );
            }
            result += ")";
            if( _withActivity )
                result += activity;
            return result;
        }
        else if( mType == FormulaType::ITE )
        {
            string result = _init + "(";
            if( _infix )
            {
                if( !_oneline ) 
                    result += "\n";
                result += mpIteContent->mCondition.toString( _withActivity, _resolveUnequal, _oneline ? "" : (_init + "   "), _oneline, true, _friendlyNames );
                result += " " + formulaTypeToString( FormulaType::ITE ) + " ";
                if( !_oneline ) 
                    result += "\n";
                result += mpIteContent->mThen.toString( _withActivity, _resolveUnequal, _oneline ? "" : (_init + "   "), _oneline, true, _friendlyNames );
                if( !_oneline ) 
                    result += "\n";
                result += mpIteContent->mElse.toString( _withActivity, _resolveUnequal, _oneline ? "" : (_init + "   "), _oneline, true, _friendlyNames );
            }
            else
            {
                result += formulaTypeToString( FormulaType::ITE );
                result += (_oneline ? " " : "\n");
                result += mpIteContent->mCondition.toString( _withActivity, _resolveUnequal, _oneline ? "" : (_init + "   "), _oneline, false, _friendlyNames );
                result += (_oneline ? " " : "\n");
                result += mpIteContent->mThen.toString( _withActivity, _resolveUnequal, _oneline ? "" : (_init + "   "), _oneline, false, _friendlyNames );
                result += (_oneline ? " " : "\n");
                result += mpIteContent->mElse.toString( _withActivity, _resolveUnequal, _oneline ? "" : (_init + "   "), _oneline, false, _friendlyNames );
            }
            result += ")";
            if( _withActivity )
                result += activity;
            return result;
        }
        else if (mType == FormulaType::EXISTS)
        {
            string result = _init + "(exists ";
            for (auto v: mpQuantifierContent->mVariables) {
                result += VariablePool::getInstance().getName(v, _friendlyNames) + " ";
            }
            result += mpQuantifierContent->mFormula.toString(_withActivity, _resolveUnequal, _init, _oneline, _infix, _friendlyNames);
            result += ")";
            return result;
        }
        else if (mType == FormulaType::FORALL)
        {
            string result = _init + "(forall ";
            for (auto v: mpQuantifierContent->mVariables) {
                result += VariablePool::getInstance().getName(v, _friendlyNames) + " ";
            }
            result += mpQuantifierContent->mFormula.toString(_withActivity, _resolveUnequal, _init, _oneline, _infix, _friendlyNames);
            result += ")";
            return result;
        }
        assert( mType == FormulaType::AND || mType == FormulaType::OR || mType == FormulaType::IFF || mType == FormulaType::XOR );
        string stringOfType = formulaTypeToString( mType );
        string result = _init + "(";
        if( _infix )
        {
            for( auto subFormula = mpSubformulas->begin(); subFormula != mpSubformulas->end(); ++subFormula )
            {
                if( subFormula != mpSubformulas->begin() )
                    result += " " + stringOfType + " ";
                if( !_oneline ) 
                    result += "\n";
                result += subFormula->toString( _withActivity, _resolveUnequal, _oneline ? "" : (_init + "   "), _oneline, true, _friendlyNames );
            }
        }
        else
        {
            result += stringOfType;
            for( auto subFormula = mpSubformulas->begin(); subFormula != mpSubformulas->end(); ++subFormula )
            {
                result += (_oneline ? " " : "\n");
                result += subFormula->toString( _withActivity, _resolveUnequal, _oneline ? "" : (_init + "   "), _oneline, false, _friendlyNames );
            }
        }
        result += ")";
        if( _withActivity )
            result += activity;
        return result;
    }
        
    template<typename Pol>
    void Formula<Pol>::collectVariables( Variables& _vars, VariableType _type, bool _ofThisType ) const
    {
        switch( getType() )
        {
            case FormulaType::BOOL:
                if( _ofThisType == (_type == VariableType::VT_BOOL) )
                {
                    _vars.insert( boolean() );
                }
                break;
            case FormulaType::TRUE:
                break;
            case FormulaType::FALSE:
                break;
            case FormulaType::CONSTRAINT:
                if( !(_ofThisType && _type == VariableType::VT_BOOL) ) // NOTE: THIS ASSUMES THAT THE VARIABLES IN THE CONSTRAINT HAVE INFINTE DOMAINS
                {
                    for( auto var : constraint().variables() )
                    {
                        if( _ofThisType == (var.getType() == VariableType::VT_INT) )
                            _vars.insert( var );
                        if( _ofThisType == (var.getType() == VariableType::VT_REAL) )
                            _vars.insert( var );
                    }
                }
                break;
            case FormulaType::NOT:
                subformula().collectVariables( _vars, _type, _ofThisType );
                break;
            case FormulaType::IMPLIES:
                premise().collectVariables( _vars, _type, _ofThisType );
                conclusion().collectVariables( _vars, _type, _ofThisType );
                break;
            case FormulaType::ITE:
                condition().collectVariables( _vars, _type, _ofThisType );
                firstCase().collectVariables( _vars, _type, _ofThisType );
                secondCase().collectVariables( _vars, _type, _ofThisType );
                break;
            case FormulaType::EXISTS:
            case FormulaType::FORALL:
                quantifiedFormula().collectVariables(_vars, _type, _ofThisType);
                break;
            default:
            {
                for( const Formula<Pol>& subFormula : subformulas() )
                    subFormula.collectVariables( _vars, _type, _ofThisType );
            }
        }
    }
    
    template<typename Pol>
    unsigned Formula<Pol>::satisfiedBy( const EvaluationMap<typename Pol::NumberType>& _assignment ) const
    {
        switch( getType() )
        {
            case FormulaType::TRUE:
            {
                return 1;
            }
            case FormulaType::FALSE:
            {
                return 0;
            }
            case FormulaType::BOOL:
            {
                auto ass = _assignment.find( boolean() );
                return ass == _assignment.end() ? 2 : (ass->second == typename Pol::NumberType( 1 ) ? 1 : 0);
            }
            case FormulaType::CONSTRAINT:
            {
                return constraint().satisfiedBy( _assignment );
            }
            case FormulaType::BITVECTOR:
            {
                assert(false);
                std::cerr << "Implement BVConstraint::satisfiedBy()" << std::endl;
                //return bvConstraint().satisfiedBy( _assignment );
            }
            case FormulaType::NOT:
            {
                switch( subformula().satisfiedBy( _assignment ) )
                {
                    case 0:
                        return 1;
                    case 1:
                        return 0;
                    default:
                        return 2;
                }   
            }
            case FormulaType::OR:
            {
                unsigned result = 0;
                for( auto subFormula = subformulas().begin(); subFormula != subformulas().end(); ++subFormula )
                {
                    switch( subFormula->satisfiedBy( _assignment ) )
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
            case FormulaType::AND:
            {
                unsigned result = 1;
                for( auto subFormula = subformulas().begin(); subFormula != subformulas().end(); ++subFormula )
                {
                    switch( subFormula->satisfiedBy( _assignment ) )
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
            case FormulaType::IMPLIES:
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
            case FormulaType::ITE:
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
            case FormulaType::IFF:
            {
                auto subFormula = subformulas().begin();
                unsigned result = subFormula->satisfiedBy( _assignment );
                bool containsTrue = (result == 1 ? true : false);
                bool containsFalse = (result == 0 ? true : false);
                ++subFormula;
                while( subFormula != subformulas().end() )
                {
                    unsigned resultTmp = subFormula->satisfiedBy( _assignment );
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
                    ++subFormula;
                }
                return (result == 2 ? 2 : 1);
            }
            case FormulaType::XOR:
            {
                auto subFormula = subformulas().begin();
                unsigned result = subFormula->satisfiedBy( _assignment );
                if( result == 2 ) return 2;
                ++subFormula;
                while( subFormula != subformulas().end() )
                {
                    unsigned resultTmp = subFormula->satisfiedBy( _assignment );
                    if( resultTmp == 2 ) return 2;
                    result = resultTmp != result;
                    ++subFormula;
                }
                return result;
            }
            case FormulaType::EXISTS:
            {
                ///@todo do something here
                return 2;
                break;
            }
            case FormulaType::FORALL:
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
    void Formula<Pol>::init( FormulaContent<Pol>& _content )
    {
        _content.mProperties = Condition();
        switch( _content.mType )
        {
            case FormulaType::TRUE:
            {
                _content.mProperties |= STRONG_CONDITIONS;
                addConstraintProperties( _content.mConstraint, _content.mProperties );
                break;
            }
            case FormulaType::FALSE:
            {
                _content.mProperties |= STRONG_CONDITIONS;
                addConstraintProperties( _content.mConstraint, _content.mProperties );
                break;
            }
            case FormulaType::BOOL:
            {
                _content.mProperties |= STRONG_CONDITIONS | PROP_CONTAINS_BOOLEAN;
                break;
            }
            case FormulaType::CONSTRAINT:
            {
                _content.mProperties |= STRONG_CONDITIONS;
                addConstraintProperties( _content.mConstraint, _content.mProperties );
                break;
            }
            case FormulaType::BITVECTOR:
            {
                _content.mProperties |= STRONG_CONDITIONS | PROP_CONTAINS_BITVECTOR;
                break;
            }
            case FormulaType::NOT:
            {
                Condition subFormulaConds = _content.mSubformula.mpContent->mProperties;
                if( PROP_IS_AN_ATOM <= subFormulaConds )
                    _content.mProperties |= PROP_IS_A_CLAUSE | PROP_IS_A_LITERAL | PROP_IS_IN_CNF | PROP_IS_PURE_CONJUNCTION;
                _content.mProperties |= (subFormulaConds & PROP_VARIABLE_DEGREE_LESS_THAN_THREE);
                _content.mProperties |= (subFormulaConds & PROP_VARIABLE_DEGREE_LESS_THAN_FOUR);
                _content.mProperties |= (subFormulaConds & PROP_VARIABLE_DEGREE_LESS_THAN_FIVE);
                _content.mProperties |= (subFormulaConds & WEAK_CONDITIONS);
                break;
            }
            case FormulaType::OR:
            {
                _content.mProperties |= PROP_IS_A_CLAUSE | PROP_IS_IN_CNF | PROP_IS_IN_NNF;
                _content.mProperties |= PROP_VARIABLE_DEGREE_LESS_THAN_THREE | PROP_VARIABLE_DEGREE_LESS_THAN_FOUR | PROP_VARIABLE_DEGREE_LESS_THAN_FIVE;
                for( auto subFormula = _content.mpSubformulas->begin(); subFormula != _content.mpSubformulas->end(); ++subFormula )
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
                _content.mProperties |= PROP_IS_PURE_CONJUNCTION | PROP_IS_IN_CNF | PROP_IS_IN_NNF;
                _content.mProperties |= PROP_VARIABLE_DEGREE_LESS_THAN_THREE | PROP_VARIABLE_DEGREE_LESS_THAN_FOUR | PROP_VARIABLE_DEGREE_LESS_THAN_FIVE;
                for( auto subFormula = _content.mpSubformulas->begin(); subFormula != _content.mpSubformulas->end(); ++subFormula )
                {
                    Condition subFormulaConds = subFormula->properties();
                    if( !(PROP_IS_A_CLAUSE<=subFormulaConds) )
                    {
                        _content.mProperties &= ~PROP_IS_PURE_CONJUNCTION;
                        _content.mProperties &= ~PROP_IS_IN_CNF;
                    }
                    else if( !(PROP_IS_A_LITERAL<=subFormulaConds) )
                        _content.mProperties &= ~PROP_IS_PURE_CONJUNCTION;
                    if( !(PROP_IS_IN_NNF<=subFormulaConds) )
                        _content.mProperties &= ~PROP_IS_IN_NNF;
                    _content.mProperties |= (subFormulaConds & WEAK_CONDITIONS);
                }
                break;
            }
            case FormulaType::IMPLIES:
            {
                _content.mProperties |= PROP_IS_IN_NNF;
                _content.mProperties |= PROP_VARIABLE_DEGREE_LESS_THAN_THREE | PROP_VARIABLE_DEGREE_LESS_THAN_FOUR | PROP_VARIABLE_DEGREE_LESS_THAN_FIVE;
                Condition subFormulaCondsA = _content.mpImpliesContent->mPremise.properties();
                if( !(PROP_IS_IN_NNF<=subFormulaCondsA) )
                    _content.mProperties &= ~PROP_IS_IN_NNF;
                _content.mProperties |= (subFormulaCondsA & WEAK_CONDITIONS);
                Condition subFormulaCondsB = _content.mpImpliesContent->mConclusion.properties();
                if( !(PROP_IS_IN_NNF<=subFormulaCondsB) )
                    _content.mProperties &= ~PROP_IS_IN_NNF;
                _content.mProperties |= (subFormulaCondsB & WEAK_CONDITIONS);
                break;
            }
            case FormulaType::ITE:
            {
                _content.mProperties |= PROP_VARIABLE_DEGREE_LESS_THAN_THREE | PROP_VARIABLE_DEGREE_LESS_THAN_FOUR | PROP_VARIABLE_DEGREE_LESS_THAN_FIVE;
                _content.mProperties |= (_content.mpIteContent->mCondition.properties() & WEAK_CONDITIONS);
                _content.mProperties |= (_content.mpIteContent->mThen.properties() & WEAK_CONDITIONS);
                _content.mProperties |= (_content.mpIteContent->mElse.properties() & WEAK_CONDITIONS);
                break;
            }
            case FormulaType::IFF:
            {
                _content.mProperties |= PROP_IS_IN_NNF;
                _content.mProperties |= PROP_VARIABLE_DEGREE_LESS_THAN_THREE | PROP_VARIABLE_DEGREE_LESS_THAN_FOUR | PROP_VARIABLE_DEGREE_LESS_THAN_FIVE;
                for( auto subFormula = _content.mpSubformulas->begin(); subFormula != _content.mpSubformulas->end(); ++subFormula )
                {
                    Condition subFormulaConds = subFormula->properties();
                    if( !(PROP_IS_IN_NNF<=subFormulaConds) )
                        _content.mProperties &= ~PROP_IS_IN_NNF;
                    _content.mProperties |= (subFormulaConds & WEAK_CONDITIONS);
                }
                break;
            }
            case FormulaType::XOR:
            {
                _content.mProperties |= PROP_IS_IN_NNF;
                _content.mProperties |= PROP_VARIABLE_DEGREE_LESS_THAN_THREE | PROP_VARIABLE_DEGREE_LESS_THAN_FOUR | PROP_VARIABLE_DEGREE_LESS_THAN_FIVE;
                for( auto subFormula = _content.mpSubformulas->begin(); subFormula != _content.mpSubformulas->end(); ++subFormula )
                {
                    Condition subFormulaConds = subFormula->properties();
                    if( !(PROP_IS_IN_NNF<=subFormulaConds) )
                        _content.mProperties &= ~PROP_IS_IN_NNF;
                    _content.mProperties |= (subFormulaConds & WEAK_CONDITIONS);
                }
                break;
            }
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
            case FormulaType::UEQ:
            {
                _content.mProperties |= STRONG_CONDITIONS | PROP_CONTAINS_UNINTERPRETED_EQUATIONS;
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
                    _properties &= ~PROP_VARIABLE_DEGREE_LESS_THAN_THREE;
                    break;
                case 4:
                    _properties |= PROP_CONTAINS_NONLINEAR_POLYNOMIAL;
                    _properties &= ~PROP_VARIABLE_DEGREE_LESS_THAN_FOUR;
                    break;
                case 5:
                    _properties |= PROP_CONTAINS_NONLINEAR_POLYNOMIAL;
                    _properties &= ~PROP_VARIABLE_DEGREE_LESS_THAN_FIVE;
                    break;
                default:
                {
                }
            }
        }
        switch( _constraint.relation() )
        {
            case Relation::EQ:
                _properties |= PROP_CONTAINS_EQUATION;
                break;
            case Relation::NEQ:
                _properties |= PROP_CONTAINS_STRICT_INEQUALITY;
                break;
            case Relation::LEQ:
                _properties |= PROP_CONTAINS_INEQUALITY;
                break;
            case Relation::GEQ:
                _properties |= PROP_CONTAINS_INEQUALITY;
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
                            default:
                                break;
                        }
                    });
            for( auto var : vars )
                os << "(declare-fun " << var << " () " << var.getType() << ")\n";
            for( const auto& uvar : uvars )
                os << "(declare-fun " << uvar() << " () " << uvar.domain() << ")\n";
            for( const auto& bvvar : bvvars )
                os << "(declare-fun " << bvvar() << " () " << bvvar.sort() << ")\n";
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
                    set<Variable> boolVars = set<Variable>();
                    booleanVars( boolVars );
                    for( auto j = boolVars.begin(); j != boolVars.end(); ++j )
                    {
                        string boolName = VariablePool::getInstance().getName( *j, true );
                        result += "(" + boolName + " = 0 or " + boolName + " = 1) and ";
                    }
                }
                else
                    result += "( ";
                typename Formulas<Pol>::const_iterator it = subformulas().begin();
                // do not quantify variables again.
                result += (*it)->toRedlogFormat( false );
                for( ++it; it != subformulas().end(); ++it ) // do not quantify variables again.
                    result += " " + oper + " " + (*it)->toRedlogFormat( false );
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
                subFormulas.insert( subformula().premise() );
                subFormulas.insert( Formula<Pol>( NOT, subformula().conclusion() ) );
                return Formula<Pol>( AND, move( subFormulas ) );
            }
            case FormulaType::ITE: // (not (ite cond then else))  ->  (ite cond (not then) (not else))
            {
                return Formula<Pol>( ITE, subformula().condition(), Formula<Pol>( NOT, subformula().firstCase() ), Formula<Pol>( NOT, subformula().secondCase() ) );
            }
            case FormulaType::IFF: // (not (iff phi_1 .. phi_n))  ->  (and (or phi_1 .. phi_n) (or (not phi_1) .. (not phi_n)))
            {
                Formulas<Pol> subFormulasA;
                Formulas<Pol> subFormulasB;
                for( auto& subFormula : subformula().subformulas() )
                {
                    subFormulasA.insert( subFormula );
                    subFormulasB.insert( Formula<Pol>( NOT, subFormula ) );
                }
                return Formula<Pol>( AND, Formula<Pol>( OR, move( subFormulasA ) ), Formula<Pol>( OR, move( subFormulasB ) ) );
            }
            case FormulaType::XOR: // (not (xor phi_1 .. phi_n))  ->  (xor (not phi_1) phi_2 .. phi_n)
            {
                auto subFormula = subformula().subformulas().begin();
                Formulas<Pol> subFormulas;
                subFormulas.insert( Formula<Pol>( NOT, *subFormula ) );
                ++subFormula;
                for( ; subFormula != subformula().subformulas().end(); ++subFormula )
                    subFormulas.insert( *subFormula );
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
            subFormulas.insert( Formula<Pol>( FormulaType::NOT, subsubformula ) );
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
                tmpSubformulas.insert( *iter );
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
                        subs.insert(sub->toQF(variables, level, false));
                    }
                    return Formula<Pol>( getType(), std::move(subs) );
                } else if (getType() == FormulaType::AND || getType() == FormulaType::OR) {
                    Formulas<Pol> subs;
                    for (auto& sub: subformulas()) {
                        subs.insert(sub->toQF(variables, level, true));
                    }
                    if (getType() == FormulaType::AND) return Formula<Pol>(FormulaType::OR, std::move(subs));
                    else return Formula<Pol>(FormulaType::AND, std::move(subs));
                } else if (getType() == FormulaType::IFF) {
                    Formulas<Pol> sub1;
                    Formulas<Pol> sub2;
                    for (auto& sub: subformulas()) {
                        sub1.insert(sub->toQF(variables, level, true));
                        sub2.insert(sub->toQF(variables, level, false));
                    }
                    return Formula<Pol>(FormulaType::AND, Formula<Pol>(FormulaType::OR, std::move(sub1)), Formula<Pol>(FormulaType::OR, std::move(sub2)));
                } else if (getType() == FormulaType::XOR) {
                    auto lhs = back()->toQF(variables, level, false);
                    auto rhs = connectPrecedingSubformulas()->toQF(variables, level, true);
                    return Formula<Pol>(FormulaType::IFF, lhs, rhs);
                }
                assert(false);
            }
            case FormulaType::BOOL:
            case FormulaType::CONSTRAINT:
            case FormulaType::FALSE:
            case FormulaType::UEQ:
            case FormulaType::TRUE:
            {
                if (negated) return Formula<Pol>( NOT, *this );
                else return *this;
            }
            case FormulaType::EXISTS:
            case FormulaType::FORALL:
            {
                unsigned cur = 0;
                if ((level % 2 == (getType() == FormulaType::EXISTS ? (unsigned)0 : (unsigned)1)) xor negated) cur = level;
                else cur = level+1;
                Variables vars(quantifiedVariables().begin(), quantifiedVariables().end());
                const Formula<Pol>& f = quantifiedFormula();
                for (auto it = vars.begin(); it != vars.end();) {
                    if (it->getType() == VariableType::VT_BOOL) {
                        // Just leave boolean variables at the base level up to the SAT solver.
                        if (cur > 0) {
                            f = Formula<Pol>(
                                (getType() == FormulaType::EXISTS ? FormulaType::OR : FormulaType::AND),
                                f->substitute({{*it, Formula<Pol>( FormulaType::TRUE )}}),
                                f->substitute({{*it, Formula<Pol>( FormulaType::FALSE )}})
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
                return f->toQF(variables, cur, negated);
            }
            case FormulaType::IMPLIES:
                if (negated) return Formula<Pol>(FormulaType::AND, premise().toQF(variables, level, false), conclusion().toQF(variables, level, true));
                else return Formula<Pol>( FormulaType::IMPLIES, premise().toQF(variables, level, false), conclusion().toQF(variables, level, false));
            case FormulaType::ITE:
                return Formula<Pol>( FormulaType::ITE, condition().toQF(variables, level, negated), firstCase().toQF(variables, level, negated), secondCase().toQF(variables, level, negated));
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
                    resultSubformulas.insert( currentFormula );
                    break;
                }
                case FormulaType::UEQ:
                {
                    resultSubformulas.insert( currentFormula );
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
                        resultSubformulas.insert( currentFormula );
                    break;
                }
                case FormulaType::BITVECTOR:
                {
                    resultSubformulas.insert( currentFormula );
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
                                resultSubformulas.insert( resolvedFormula );
                            }
                        }
                        else
                        {
                            resultSubformulas.insert( resolvedFormula );
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
                    tmpSubformulas.insert( Formula<Pol>( NOT, currentFormula.premise() ) );
                    tmpSubformulas.insert( currentFormula.conclusion() );
                    subformulasToTransform.push_back( Formula<Pol>( FormulaType::OR, tmpSubformulas ) );
                    break;
                }
                case FormulaType::ITE: // (ite cond then else)  ->  auxBool, where (or (not cond) then) and (or cond else) are added to the queue
                {
                    // Add: (or (not cond) then)
                    subformulasToTransform.push_back( Formula<Pol>( FormulaType::OR, Formula<Pol>( FormulaType::NOT, currentFormula.condition() ), currentFormula.firstCase() ) );
                    // Add: (or cond else)
                    subformulasToTransform.push_back( Formula<Pol>( FormulaType::OR, currentFormula.condition(), currentFormula.secondCase() ) );
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
                            subformulasA.insert( subFormula );
                            subformulasB.insert( Formula<Pol>( FormulaType::NOT, subFormula ) );
                        }
                        subformulasToTransform.push_back( Formula<Pol>( FormulaType::OR, Formula<Pol>( AND, move( subformulasA ) ), Formula<Pol>( FormulaType::AND, move( subformulasB ) ) ) );
                    }
                    else
                    {
                        // (iff lhs rhs) -> (or lhs (not rhs)) and (or (not lhs) rhs) are added to the queue
                        assert( currentFormula.subformulas().size() == 2 );
                        // Get lhs and rhs.
                        const Formula<Pol>& lhs = *currentFormula.subformulas().begin();
                        const Formula<Pol>& rhs = currentFormula.back();
                        // add (or lhs (not rhs)) to the queue
                        subformulasToTransform.push_back( Formula<Pol>( FormulaType::OR, lhs, Formula<Pol>( FormulaType::NOT, rhs ) ) );
                        // add (or (not lhs) rhs) to the queue
                        subformulasToTransform.push_back( Formula<Pol>( FormulaType::OR, Formula<Pol>( FormulaType::NOT, lhs ), rhs ) );
                    }
                    break;
                }
                case FormulaType::XOR: // (xor lhs rhs) -> (or lhs rhs) and (or (not lhs) (not rhs)) are added to the queue
                {
                    // Get lhs and rhs.
                    Formula<Pol> lhs = currentFormula.connectPrecedingSubformulas();
                    const Formula<Pol>& rhs = currentFormula.back();
                    // add (or lhs rhs) to the queue
                    subformulasToTransform.push_back( Formula<Pol>( FormulaType::OR, lhs, rhs) );
                    // add (or (not lhs) (not rhs)) to the queue
                    subformulasToTransform.push_back( Formula<Pol>( FormulaType::OR, Formula<Pol>( FormulaType::NOT, lhs ), Formula<Pol>( FormulaType::NOT, rhs ) ) );
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
                                subsubformulas.insert( currentSubformula );
                                break;
                            case FormulaType::UEQ: // u -> u
                                subsubformulas.insert( currentSubformula );
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
                                phis.push_back( Formula<Pol>( NOT, currentSubformula.premise() ) );
                                phis.push_back( currentSubformula.conclusion() );
                                break;
                            case FormulaType::ITE: // (ite cond then else)  ->  (and (or (not cond) then) (or cond else))
                            {   
                                Formulas<Pol> tmpSubformulas;
                                // Add: (or (not cond) then)
                                tmpSubformulas.insert( Formula<Pol>( FormulaType::OR, Formula<Pol>( FormulaType::NOT, currentSubformula.condition() ), currentSubformula.firstCase() ) );
                                // Add: (or cond else)
                                tmpSubformulas.insert( Formula<Pol>( FormulaType::OR, currentSubformula.condition(), currentSubformula.secondCase() ) );
                                phis.push_back( Formula<Pol>( FormulaType::AND, tmpSubformulas ) );
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
                                            subsubformulas.insert( resolvedFormula );
                                        }
                                    }
                                    else
                                    {
                                        subsubformulas.insert( resolvedFormula );
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
                                            tmpSubSubformulas.insert( subsubformula );
                                        }
                                    }
                                    else
                                    {
                                        tmpSubSubformulas.insert( subsubformula );
                                    }
                                }
                                if( conjunctionIsFalse )
                                    break;
                                if( _simplifyConstraintCombinations && swapConstraintBounds( constraintBoundsOrAnd, tmpSubSubformulas, true ) )
                                    break;
                                Formula<Pol> tseitinVar = FormulaPool<Pol>::getInstance().createTseitinVar( currentSubformula );
                                for( const Formula<Pol>& subsubformula : tmpSubSubformulas )
                                {
                                    subformulasToTransformTmp.push_back( Formula<Pol>( OR, Formula<Pol>( FormulaType::NOT, tseitinVar ), subsubformula ) );
                                    assert( *(--(subformulasToTransformTmp.back().subformulas().end())) == Formula<Pol>( FormulaType::NOT, tseitinVar ) );
                                    subformulasToTransformTmp.back().mpContent->mTseitinClause = true;
                                }
                                if( _tseitinWithEquivalence )
                                {
                                    Formulas<Pol> tmpSubformulas;
                                    tmpSubformulas.insert( tseitinVar );
                                    for( const Formula<Pol>& subsubformula : tmpSubSubformulas )
                                        tmpSubformulas.insert( Formula<Pol>( NOT, subsubformula ) );
                                    subformulasToTransformTmp.push_back( Formula<Pol>( OR, tmpSubformulas ) );
                                    subformulasToTransformTmp.back().mpContent->mTseitinClause = true;
                                    assert( *(--(subformulasToTransformTmp.back().subformulas().end())) == tseitinVar );
                                }
                                subsubformulas.insert( tseitinVar );
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
                                    subsubformulas.insert( currentSubformula );
                                }
                                break;
                            }
                            case FormulaType::BITVECTOR:
                            {
                                ///@todo Anything more to do here?
                                subsubformulas.insert( currentSubformula );
                                break;
                            }
                            case FormulaType::IFF: // (iff phi_1 .. phi_n) -> (and phi_1 .. phi_n) and (and (not phi_1) .. (not phi_n)) are added to the queue
                            {
                                Formulas<Pol> subformulasA;
                                Formulas<Pol> subformulasB;
                                for( auto& subFormula : currentSubformula.subformulas() )
                                {
                                    subformulasA.insert( subFormula );
                                    subformulasB.insert( Formula<Pol>( FormulaType::NOT, subFormula ) );
                                }
                                phis.push_back( Formula<Pol>( FormulaType::AND, move( subformulasA ) ) );
                                phis.push_back( Formula<Pol>( FormulaType::AND, move( subformulasB ) ) );
                                break;
                            }
                            case FormulaType::XOR: // (xor lhs rhs) -> (and lhs (not rhs)) and (and (not lhs) rhs) are added to the queue
                            {
                                // Get lhs and rhs.
                                Formula<Pol> lhs = currentSubformula.connectPrecedingSubformulas();
                                const Formula<Pol>& rhs = currentSubformula.back();
                                // add (and lhs (not rhs)) to the queue
                                phis.push_back( Formula<Pol>( FormulaType::AND, lhs, Formula<Pol>( FormulaType::NOT, rhs )) );
                                // add (and (not lhs) rhs) to the queue
                                phis.push_back( Formula<Pol>( FormulaType::AND, Formula<Pol>( FormulaType::NOT, lhs ), rhs ) );
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
                        subformulasToTransform.insert( subformulasToTransform.end(), subformulasToTransformTmp.begin(), subformulasToTransformTmp.end() );
                        if( subsubformulas.empty() ) // Empty clause = false, which, added to a conjunction, leads to false.
                        {
                            goto ReturnFalse;
                        }
                        else if( subsubformulas.size() == 1 )
                        {
                            resultSubformulas.insert( *subsubformulas.begin() );
                        }
                        else
                        {
                            resultSubformulas.insert( Formula<Pol>( OR, std::move( subsubformulas ) ) );
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
                return Formula<Pol>( FormulaType::IMPLIES, premiseSubstituted, conclusionSubstituted );
            }
            case FormulaType::ITE:
            {
                Formula<Pol> conditionSubstituted = condition().substitute( _booleanSubstitutions, _arithmeticSubstitutions );
                Formula<Pol> thenSubstituted = firstCase().substitute( _booleanSubstitutions, _arithmeticSubstitutions );
                Formula<Pol> elseSubstituted = secondCase().substitute( _booleanSubstitutions, _arithmeticSubstitutions );
                return Formula<Pol>( FormulaType::ITE, conditionSubstituted, thenSubstituted, elseSubstituted );
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
                    subformulasSubstituted.insert( subFormula.substitute( _booleanSubstitutions, _arithmeticSubstitutions ) );
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
        Relation relation = negated ? carl::invertRelation( constraint.relation() ) : constraint.relation();
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
        cout << "try to add the bound  " << Constraint<Pol>::relationToString( relation ) << boundValue << "  for the polynomial  " << poly << endl; 
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
                _intoFormulas.insert( bounds.begin()->second.second );
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
                    cout << "   bound is  " << Constraint<Pol>::relationToString( iter->second.first ) << iter->first << endl;
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
                                    _intoFormulas.insert( iter->second.second );
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
                    _intoFormulas.insert( moreSignificantCase->second.second );
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
                        _intoFormulas.insert( mostSignificantUpperBound->second.second );
                    if( mostSignificantLowerBound != bounds.end() )
                        _intoFormulas.insert( mostSignificantLowerBound->second.second );
                    _intoFormulas.insert( lessSignificantCases.begin(), lessSignificantCases.end() );
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
		func(formula);
		switch (formula.getType()) {
		case AND:
		case OR:
		case IFF:
		case XOR: {
			for (const auto& cur: formula.subformulas()) visit(cur, func);
			break;
		}
		case NOT: {
			visit(formula.subformula(), func);
			break;
		}
		case IMPLIES: {
			visit(formula.premise(), func);
			visit(formula.conclusion(), func);
			break;
		}
		case ITE: {
			visit(formula.condition(), func);
			visit(formula.firstCase(), func);
			visit(formula.secondCase(), func);
			break;
		}
		case BOOL:
		case CONSTRAINT:
		case BITVECTOR:
		case TRUE:
		case FALSE:
		case UEQ:
			break;
		case EXISTS:
		case FORALL: {
			visit(formula.quantifiedFormula(), func);
			break;
		}
		}
	}

	template<typename Formula>
	Formula FormulaVisitor<Formula>::visit(const Formula& formula, const std::function<Formula(Formula)>& func) {
		Formula newFormula = func(formula);
		switch (newFormula.getType()) {
		case AND:
		case OR:
		case IFF:
		case XOR: {
			Formulas<typename Formula::PolynomialType> newSubformulas;
			bool changed = false;
			for (const auto& cur: newFormula.subformulas()) {
				Formula newCur = visit(cur, func);
				if (newCur != cur) changed = true;
				newSubformulas.insert(newCur);
			}
			if (changed) {
				return Formula(newFormula.getType(), newSubformulas);
			}
			break;
		}
		case NOT: {
			Formula cur = visit(newFormula.subformula(), func);
			if (cur != newFormula.subformula()) {
				return Formula(NOT, cur);
			}
			break;
		}
		case IMPLIES: {
			Formula prem = visit(newFormula.premise(), func);
			Formula conc = visit(newFormula.conclusion(), func);
			if ((prem != newFormula.premise()) || (conc != newFormula.conclusion())) {
				return Formula(IMPLIES, prem, conc);
			}
			break;
		}
		case ITE: {
			Formula cond = visit(newFormula.condition(), func);
			Formula fCase = visit(newFormula.firstCase(), func);
			Formula sCase = visit(newFormula.secondCase(), func);
			if ((cond != newFormula.condition()) || (fCase != newFormula.firstCase()) || (sCase != newFormula.secondCase())) {
				return Formula(ITE, cond, fCase, sCase);
			}
			break;
		}
		case BOOL:
		case CONSTRAINT:
		case BITVECTOR:
		case TRUE:
		case FALSE:
		case UEQ:
			break;
		case EXISTS:
		case FORALL: {
			Formula sub = visit(newFormula.quantifiedFormula(), func);
			if (sub != newFormula.quantifiedFormula()) {
				return Formula(newFormula.getType(), newFormula.quantifiedVariables(), sub);
			}
			break;
		}
		}
		return newFormula;
	}
    
    template<typename Formula>
    Formula FormulaSubstitutor<Formula>::substitute(const Formula& formula, const std::map<Variable,typename Formula::PolynomialType>& replacements) {
        PolynomialSubstitutor subs(replacements);
        return visitor.visit(formula, std::function<Formula(Formula)>(subs));
    }
    template<typename Formula>
    Formula FormulaSubstitutor<Formula>::substitute(const Formula& formula, const std::map<BVVariable,BVTerm>& replacements) {
        BitvectorSubstitutor subs(replacements);
        return visitor.visit(formula, std::function<Formula(Formula)>(subs));
    }
    template<typename Formula>
    Formula FormulaSubstitutor<Formula>::substitute(const Formula& formula, const std::map<UVariable,UFInstance>& replacements) {
        UninterpretedSubstitutor subs(replacements);
        return visitor.visit(formula, std::function<Formula(Formula)>(subs));
    }
}    // namespace carl
