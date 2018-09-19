
namespace carl {

	template<typename Pol>
    FormulaContent<Pol>::FormulaContent(FormulaType _type, size_t _id):
        mHash(((size_t)(Constraint<Pol>(_type == FormulaType::TRUE).id())) << (sizeof(size_t)*4)),
        mId( _id ),
#ifdef __VS
        mType( _type )
    {
		mpConstraintVS = new Constraint<Pol>(Constraint<Pol>(_type == FormulaType::TRUE));
#else
		mType(_type),
		mConstraint(Constraint<Pol>(_type == FormulaType::TRUE))
	{
#endif
		assert(_type == FormulaType::TRUE || _type == FormulaType::FALSE);
		CARL_LOG_DEBUG("carl.formula", "Created " << *this << " from " << _type);
	}

    template<typename Pol>
    FormulaContent<Pol>::FormulaContent(Variable _variable):
		mHash(std::hash<Variable>()(_variable)), // TODO: subtract the id of the boolean variable with the smallest id
#ifdef __VS
		mpVariableVS(new Variable(_variable))
#else
		mVariable(_variable)
#endif
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
	FormulaContent<Pol>::FormulaContent(Formula<Pol>&& _lhs, Relation _rel):
	///@todo use sensible hash.
#ifdef __VS
		mHash(_lhs.mHash)
	{
		mpArithmeticVS = new ArithmeticConstraintContent<Pol>(std::move(_lhs), _rel);
#else
		mHash(_lhs.mHash),
		mArithmetic(std::move(_lhs), _rel)
	{
#endif
		CARL_LOG_DEBUG("carl.formula", "Created " << *this << " from " << mArithmetic.mLhs << " " << _rel);
	}

	template<typename Pol>
	FormulaContent<Pol>::FormulaContent(Constraint<Pol>&& _constraint):
        mHash( _constraint.id() << (sizeof(size_t)*4) ),
#ifdef __VS
        mType( FormulaType::CONSTRAINT )
    {
		mpConstraintVS = new Constraint<Pol>(std::move(_constraint));
		CARL_LOG_DEBUG("carl.formula", "Created " << *this << " from " << *mpConstraintVS);
#else
		mType(FormulaType::CONSTRAINT),
		mConstraint(std::move(_constraint))
	{
		CARL_LOG_DEBUG("carl.formula", "Created " << *this << " from " << mConstraint);
#endif
	}

	template<typename Pol>
	FormulaContent<Pol>::FormulaContent(VariableComparison<Pol>&& _variableComparison):
		mHash(std::hash<VariableComparison<Pol>>()(_variableComparison)),
#ifdef __VS
        mType( FormulaType::VARCOMPARE )
    {
		mpVariableComparisonVS = new VariableComparison<Pol>(std::move(_variableComparison));
		CARL_LOG_DEBUG("carl.formula", "Created " << *this << " from " << *mpVariableComparisonVS);
#else
		mType(FormulaType::VARCOMPARE),
		mVariableComparison(std::move(_variableComparison))
	{
		CARL_LOG_DEBUG("carl.formula", "Created " << *this << " from " << mVariableComparison);
#endif
	}

	template<typename Pol>
	FormulaContent<Pol>::FormulaContent(VariableAssignment<Pol>&& _variableAssignment):
		mHash(std::hash<VariableAssignment<Pol>>()(_variableAssignment)),
#ifdef __VS
        mType( FormulaType::VARASSIGN )
    {
		mpVariableAssignmentVS = new VariableAssignment<Pol>(std::move(_variableAssignment));
		CARL_LOG_DEBUG("carl.formula", "Created " << *this << " from " << *mpVariableAssignmentVS);
#else
		mType(FormulaType::VARASSIGN),
		mVariableAssignment(std::move(_variableAssignment))
	{
		CARL_LOG_DEBUG("carl.formula", "Created " << *this << " from " << mVariableAssignment);
#endif
	}

	template<typename Pol>
	FormulaContent<Pol>::FormulaContent(BVConstraint&& _constraint):
        mHash( ((size_t) _constraint.id()) << (sizeof(size_t)*4) ),
#ifdef __VS
        mType( FormulaType::BITVECTOR )
    {
		mpBVConstraintVS = new BVConstraint(std::move(_constraint));
		CARL_LOG_DEBUG("carl.formula", "Created " << *this << " from " << *mpBVConstraintVS);
#else
		mType(FormulaType::BITVECTOR),
		mBVConstraint(std::move(_constraint))
	{
		CARL_LOG_DEBUG("carl.formula", "Created " << *this << " from " << mBVConstraint);
#endif
	}


	template<typename Pol>
	FormulaContent<Pol>::FormulaContent( UEquality&& _ueq ):
        mHash( std::hash<UEquality>()( _ueq ) ),
#ifdef __VS
        mType( FormulaType::UEQ )
    {
		mpUIEqualityVS = new UEquality(std::move(_ueq));
		CARL_LOG_DEBUG("carl.formula", "Created " << *this << " from " << *mpUIEqualityVS);
#else
		mType(FormulaType::UEQ),
		mUIEquality(std::move(_ueq))
	{
		CARL_LOG_DEBUG("carl.formula", "Created " << *this << " from " << mUIEquality);
#endif
	}

    template<typename Pol>
    FormulaContent<Pol>::FormulaContent(FormulaType _type, Formula<Pol>&& _subformula):
        mHash( ((size_t)NOT << 5) ^ _subformula.getHash() ),
#ifdef __VS
        mType( _type )
    {
		mpSubformulaVS = new Formula<Pol>(std::move(_subformula));
		CARL_LOG_DEBUG("carl.formula", "Created " << *this << " from " << _type << " " << *mpSubformulaVS);
#else
		mType(_type),
		mSubformula(std::move(_subformula))
	{
		CARL_LOG_DEBUG("carl.formula", "Created " << *this << " from " << _type << " " << mSubformula);
#endif
	}
	
	template<typename Pol>
	FormulaContent<Pol>::FormulaContent(FormulaType _type,  const std::initializer_list<Formula<Pol>>& _subformula):
		FormulaContent(_type, std::move(Formulas<Pol>(_subformula.begin(), _subformula.end())))
	{}

    template<typename Pol>
    FormulaContent<Pol>::FormulaContent(FormulaType _type, Formulas<Pol>&& _subformulas):
        mHash( (size_t)_type ),
#ifdef __VS
        mType( _type )
    {
        mpSubformulasVS = new Formulas<Pol>( std::move(_subformulas) );
        assert( !mpSubformulasVS->empty() );
        assert( isNary() );
        for (const auto& subformula: *mpSubformulasVS) {
			carl::hash_add(mHash, subformula.getHash());
        }
	CARL_LOG_DEBUG("carl.formula", "Created " << *this << " from " << _type << " " << *mpSubformulasVS);
#else
        mType( _type ),
        mSubformulas( std::move(_subformulas) )
    {
        assert( !mSubformulas.empty() );
        assert( isNary() );
        for (const auto& subformula: mSubformulas) {
			carl::hash_add(mHash, subformula.getHash());
        }
	CARL_LOG_DEBUG("carl.formula", "Created " << *this << " from " << _type << " " << mSubformulas);
#endif
    }

    template<typename Pol>
    FormulaContent<Pol>::FormulaContent(FormulaType _type, std::vector<carl::Variable>&& _vars, const Formula<Pol>& _term):
        ///@todo Construct reasonable hash
        mHash( _term.getHash() ),
#ifdef __VS
        mType( _type )
    {
		mpQuantifierContentVS = new QuantifierContent<Pol>(std::move(_vars), std::move(Formula<Pol>(_term)));
#else
		mType(_type),
		mQuantifierContent(QuantifierContent<Pol>(std::move(_vars), std::move(Formula<Pol>(_term))))
	{
#endif
        assert(_type == FormulaType::EXISTS || _type == FormulaType::FORALL);
    }

    template<typename Pol>
    bool FormulaContent<Pol>::operator==(const FormulaContent& _content) const {
		CARL_LOG_TRACE("carl.formula", *this << " == " << _content << " (" << mId << " / " << _content.mId << ")");
		if (mId != 0 && _content.mId != 0) return mId == _content.mId;
        if (mType != _content.mType) return false;
		switch (mType) {
		case FormulaType::TRUE:
			return true;
		case FormulaType::FALSE:
			return true;
#ifdef __VS
		case FormulaType::BOOL:
			return *mpVariableVS == *_content.mpVariableVS;
		case FormulaType::NOT:
			return *mpSubformulaVS == *_content.mpSubformulaVS;
#else
		case FormulaType::BOOL:
			return mVariable == _content.mVariable;
		case FormulaType::NOT:
			return mSubformula == _content.mSubformula;
#endif
			case FormulaType::IMPLIES:
			case FormulaType::AND:
			case FormulaType::OR:
			case FormulaType::XOR:
			case FormulaType::IFF:
#ifdef __VS
			case FormulaType::ITE:
				return *mpSubformulasVS == *_content.mpSubformulasVS;
			case FormulaType::CONSTRAINT:
				return *mpConstraintVS == *_content.mpConstraintVS;
			case FormulaType::VARCOMPARE:
				return *mpVariableComparisonVS == *_content.mpVariableComparisonVS;
			case FormulaType::VARASSIGN:
				return *mpVariableAssignmentVS == *_content.mpVariableAssignmentVS;
			case FormulaType::BITVECTOR:
				return *mpBVConstraintVS == *_content.mpBVConstraintVS;
			case FormulaType::UEQ:
				return *mpUIEqualityVS == *_content.mpUIEqualityVS;
			case FormulaType::EXISTS:
				return (*mpQuantifierContentVS == *_content.mpQuantifierContentVS);
			case FormulaType::FORALL:
				return (*mpQuantifierContentVS == *_content.mpQuantifierContentVS);
#else
			case FormulaType::ITE:
				return mSubformulas == _content.mSubformulas;
			case FormulaType::CONSTRAINT:
				return mConstraint == _content.mConstraint;
			case FormulaType::VARCOMPARE:
				return mVariableComparison == _content.mVariableComparison;
			case FormulaType::VARASSIGN:
				return mVariableAssignment == _content.mVariableAssignment;
			case FormulaType::BITVECTOR:
				return mBVConstraint == _content.mBVConstraint;
			case FormulaType::UEQ:
				return mUIEquality == _content.mUIEquality;
			case FormulaType::EXISTS:
				return (mQuantifierContent == _content.mQuantifierContent);
			case FormulaType::FORALL:
				return (mQuantifierContent == _content.mQuantifierContent);
#endif
        }
		assert(false);
		return false;
    }
}
