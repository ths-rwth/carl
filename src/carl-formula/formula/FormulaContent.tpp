
namespace carl {

	template<typename Pol>
    FormulaContent<Pol>::FormulaContent(FormulaType _type, size_t _id):
        mHash(std::hash<Constraint<Pol>>()(Constraint<Pol>(_type == FormulaType::TRUE))),
        mId( _id ),
		mType(_type),
		mContent(Constraint<Pol>(_type == FormulaType::TRUE))
	{
		assert(_type == FormulaType::TRUE || _type == FormulaType::FALSE);
		CARL_LOG_DEBUG("carl.formula", "Created " << *this << " from " << _type);
	}

    template<typename Pol>
    FormulaContent<Pol>::FormulaContent(Variable _variable):
		mHash(std::hash<Variable>()(_variable)),
		mContent(_variable)
	{
		switch (_variable.type()) {
			case VariableType::VT_BOOL:
				mType = BOOL;
				break;
			default:
				assert(false);
		}
		CARL_LOG_DEBUG("carl.formula", "Created " << *this << " from " << _variable);
    }
	
	template<typename Pol>
	FormulaContent<Pol>::FormulaContent(Constraint<Pol>&& _constraint):
        mHash( std::hash<Constraint<Pol>>()(_constraint) ),
		mType(FormulaType::CONSTRAINT),
		mContent(std::move(_constraint))
	{
		CARL_LOG_DEBUG("carl.formula", "Created " << *this << " from " << std::get<Constraint<Pol>>(mContent));
	}

	template<typename Pol>
	FormulaContent<Pol>::FormulaContent(VariableComparison<Pol>&& _variableComparison):
		mHash(std::hash<VariableComparison<Pol>>()(_variableComparison)),
		mType(FormulaType::VARCOMPARE),
		mContent(std::move(_variableComparison))
	{
		CARL_LOG_DEBUG("carl.formula", "Created " << *this << " from " << std::get<VariableComparison<Pol>>(mContent));
	}

	template<typename Pol>
	FormulaContent<Pol>::FormulaContent(VariableAssignment<Pol>&& _variableAssignment):
		mHash(std::hash<VariableAssignment<Pol>>()(_variableAssignment)),
		mType(FormulaType::VARASSIGN),
		mContent(std::move(_variableAssignment))
	{
		CARL_LOG_DEBUG("carl.formula", "Created " << *this << " from " << std::get<VariableAssignment<Pol>>(mContent));
	}

	template<typename Pol>
	FormulaContent<Pol>::FormulaContent(BVConstraint&& _constraint):
        mHash( ((size_t) _constraint.id()) << (sizeof(size_t)*4) ),
		mType(FormulaType::BITVECTOR),
		mContent(std::move(_constraint))
	{
		CARL_LOG_DEBUG("carl.formula", "Created " << *this << " from " << std::get<BVConstraint>(mContent));
	}


	template<typename Pol>
	FormulaContent<Pol>::FormulaContent( UEquality&& _ueq ):
        mHash( std::hash<UEquality>()( _ueq ) ),
		mType(FormulaType::UEQ),
		mContent(std::move(_ueq))
	{
		CARL_LOG_DEBUG("carl.formula", "Created " << *this << " from " << std::get<UEquality>(mContent));
	}

    template<typename Pol>
    FormulaContent<Pol>::FormulaContent(FormulaType _type, Formula<Pol>&& _subformula):
        mHash( ((size_t)NOT << 5) ^ _subformula.hash() ),
		mType(_type),
		mContent(std::move(_subformula))
	{
		CARL_LOG_DEBUG("carl.formula", "Created " << *this << " from " << _type << " " << std::get<Formula<Pol>>(mContent));
	}
	
	template<typename Pol>
	FormulaContent<Pol>::FormulaContent(FormulaType _type,  const std::initializer_list<Formula<Pol>>& _subformula):
		FormulaContent(_type, std::move(Formulas<Pol>(_subformula.begin(), _subformula.end())))
	{}

    template<typename Pol>
    FormulaContent<Pol>::FormulaContent(FormulaType _type, Formulas<Pol>&& _subformulas):
        mHash( (size_t)_type ),
        mType( _type ),
        mContent( std::move(_subformulas) )
    {
        assert( !std::get<Formulas<Pol>>(mContent).empty() );
        assert( is_nary() );
		std::get<Formulas<Pol>>(mContent).shrink_to_fit();
        for (const auto& subformula: std::get<Formulas<Pol>>(mContent)) {
			carl::hash_add(mHash, subformula.hash());
        }
		CARL_LOG_DEBUG("carl.formula", "Created " << *this << " from " << _type << " " << std::get<Formulas<Pol>>(mContent));
    }

    template<typename Pol>
    FormulaContent<Pol>::FormulaContent(FormulaType _type, std::vector<carl::Variable>&& _vars, const Formula<Pol>& _term):
        ///@todo Construct reasonable hash
        mHash( _term.hash() ),
		mType(_type),
		mContent(QuantifierContent<Pol>(std::move(_vars), std::move(Formula<Pol>(_term))))
	{
        assert(_type == FormulaType::EXISTS || _type == FormulaType::FORALL);
    }

	template<typename Pol>
    FormulaContent<Pol>::FormulaContent(FormulaType _type, std::vector<carl::Variable>&& _vars, const Formula<Pol>& _aux_term, const Formula<Pol>& _term):
        ///@todo Construct reasonable hash
        mHash( _term.hash() ),
		mType(_type),
		mContent(AuxQuantifierContent<Pol>(std::move(_vars), std::move(Formula<Pol>(_aux_term)), std::move(Formula<Pol>(_term))))
	{
        assert(_type == FormulaType::AUX_EXISTS);
    }

    template<typename Pol>
    bool FormulaContent<Pol>::operator==(const FormulaContent& _content) const {
		CARL_LOG_TRACE("carl.formula", *this << " == " << _content << " (" << mId << " / " << _content.mId << ")");
		if (mId != 0 && _content.mId != 0) return mId == _content.mId;
        if (mType != _content.mType) return false;
		if (mType == FormulaType::TRUE || mType == FormulaType::FALSE) {
			return true;
		} else {
			return mContent == _content.mContent;
		}
    }
}
