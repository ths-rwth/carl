
namespace carl {

	template<typename Pol>
    FormulaContent<Pol>::FormulaContent(FormulaType _type, size_t _id):
        mHash(((size_t)(Constraint<Pol>(_type == FormulaType::TRUE).id())) << (sizeof(size_t)*4)),
        mId( _id ),
        mType( _type ),
        mConstraint( Constraint<Pol>(_type == FormulaType::TRUE) )
    {
		assert(_type == FormulaType::TRUE || _type == FormulaType::FALSE);
	}

    template<typename Pol>
    FormulaContent<Pol>::FormulaContent(carl::Variable::Arg _variable):
        mHash((std::size_t)_variable.getId()), // TODO: subtract the id of the boolean variable with the smallest id
        mVariable( _variable )
    {
		switch (_variable.getType()) {
			case VariableType::VT_BOOL:
				mType = BOOL;
				break;
			default:
				assert(false);
		}
    }

	template<typename Pol>
	FormulaContent<Pol>::FormulaContent(Constraint<Pol>&& _constraint):
        mHash( ((size_t) _constraint.id()) << (sizeof(size_t)*4) ),
        mType( FormulaType::CONSTRAINT ),
        mConstraint(std::move(_constraint))
    {}

	template<typename Pol>
	FormulaContent<Pol>::FormulaContent(BVConstraint&& _constraint):
        mHash( ((size_t) _constraint.id()) << (sizeof(size_t)*4) ),
        mType( FormulaType::BITVECTOR ),
        mBVConstraint(std::move(_constraint))
    {}


	template<typename Pol>
	FormulaContent<Pol>::FormulaContent( UEquality&& _ueq ):
        mHash( std::hash<UEquality>()( _ueq ) ),
        mType( FormulaType::UEQ ),
        mUIEquality( std::move( _ueq ) )
    {}

    template<typename Pol>
    FormulaContent<Pol>::FormulaContent(FormulaType _type, Formula<Pol>&& _subformula):
        mHash( ((size_t)NOT << 5) ^ _subformula.getHash() ),
        mType( _type ),
        mSubformula( std::move(_subformula) )
    {
	}
	
	template<typename Pol>
	FormulaContent<Pol>::FormulaContent(FormulaType _type,  const std::initializer_list<Formula<Pol>>& _subformula):
		FormulaContent(_type, std::move(Formulas<Pol>(_subformula.begin(), _subformula.end())))
	{}

    template<typename Pol>
    FormulaContent<Pol>::FormulaContent(FormulaType _type, Formulas<Pol>&& _subformulas):
        mHash( (size_t)_type ),
        mType( _type ),
		mSubformulas( std::move(_subformulas) )
    {
        assert( isNary() );
        for (const auto& subformula: mSubformulas) {
            mHash = CIRCULAR_SHIFT(std::size_t, mHash, 5);
            mHash ^= subformula.getHash();
        }
    }

    template<typename Pol>
    FormulaContent<Pol>::FormulaContent(FormulaType _type, std::vector<carl::Variable>&& _vars, const Formula<Pol>& _term):
        ///@todo Construct reasonable hash
        mHash( _term.getHash() ),
        mType( _type ),
		mQuantifierContent(QuantifierContent<Pol>(std::move(_vars), _term))
    {
        assert(_type == FormulaType::EXISTS || _type == FormulaType::FORALL);
    }

    template<typename Pol>
    bool FormulaContent<Pol>::operator==(const FormulaContent& _content) const {
		if (mId != 0 && _content.mId != 0) return mId == _content.mId;
        if (mType != _content.mType) return false;
        switch (mType) {
            case FormulaType::TRUE:
                return true;
            case FormulaType::FALSE:
                return true;
            case FormulaType::BOOL:
                return mVariable == _content.mVariable;
            case FormulaType::NOT:
                return mSubformula == _content.mSubformula;
			case FormulaType::IMPLIES:
			case FormulaType::AND:
			case FormulaType::OR:
			case FormulaType::XOR:
			case FormulaType::IFF:
			case FormulaType::ITE:
				return mSubformulas == _content.mSubformulas;
            case FormulaType::CONSTRAINT:
                return mConstraint == _content.mConstraint;
            case FormulaType::BITVECTOR:
                return mBVConstraint == _content.mBVConstraint;
            case FormulaType::UEQ:
                return mUIEquality == _content.mUIEquality;
            case FormulaType::EXISTS:
                return (mQuantifierContent == _content.mQuantifierContent);
            case FormulaType::FORALL:
                return (mQuantifierContent == _content.mQuantifierContent);
        }
		assert(false);
		return false;
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
            return (_init + VariablePool::getInstance().getName( mVariable, _friendlyNames ) + activity);
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
        else if (mType == FormulaType::EXISTS)
        {
            string result = _init + "(exists ";
            for (auto v: mQuantifierContent.mVariables) {
                result += VariablePool::getInstance().getName(v, _friendlyNames) + " ";
            }
            result += mQuantifierContent.mFormula.toString(_withActivity, _resolveUnequal, _init, _oneline, _infix, _friendlyNames);
            result += ")";
            return result;
        }
        else if (mType == FormulaType::FORALL)
        {
            string result = _init + "(forall ";
            for (auto v: mQuantifierContent.mVariables) {
                result += VariablePool::getInstance().getName(v, _friendlyNames) + " ";
            }
            result += mQuantifierContent.mFormula.toString(_withActivity, _resolveUnequal, _init, _oneline, _infix, _friendlyNames);
            result += ")";
            return result;
        }
        assert( isNary() );
        string stringOfType = formulaTypeToString( mType );
        string result = _init + "(";
        if( _infix )
        {
            for( auto subFormula = mSubformulas.begin(); subFormula != mSubformulas.end(); ++subFormula )
            {
                if( subFormula != mSubformulas.begin() )
                    result += " " + stringOfType + " ";
                if( !_oneline ) 
                    result += "\n";
                result += subFormula->toString( _withActivity, _resolveUnequal, _oneline ? "" : (_init + "   "), _oneline, true, _friendlyNames );
            }
        }
        else
        {
            result += stringOfType;
            for( auto subFormula = mSubformulas.begin(); subFormula != mSubformulas.end(); ++subFormula )
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
	
}
