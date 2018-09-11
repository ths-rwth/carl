#pragma once

namespace carl {
    // Forward declaration.
    template<typename Pol>
    class Formula;
    // Forward declaration.
    template<typename Pol>
    class FormulaPool;
	

    template<typename Poly>
    using Formulas = std::vector<Formula<Poly>>;
    template<typename Poly>
    using FormulaSet = std::set<Formula<Poly>>;
	template<typename Poly>
    using FormulasMulti = std::multiset<Formula<Poly>, carl::less<Formula<Poly>>>;
	
    
    /// 
    /**
     * Represent the type of a formula to allow faster/specialized processing.
     *
     * For each (supported) SMTLIB theory, we have
     * - Constants
     * - Variables
     * - Functions
     * - Additional functions (not specified, but used in the wild)
     */
    enum FormulaType {
        // Generic
        ITE, EXISTS, FORALL,
        
        // Core Theory
        TRUE, FALSE,
        BOOL,
        NOT, IMPLIES, AND, OR, XOR,
        IFF, 

		// Arithmetic Theory
		CONSTRAINT,
		VARCOMPARE,
		VARASSIGN,
		
		// Bitvector Theory
		BITVECTOR,
		
		// Uninterpreted Theory
		UEQ,
		
		// Pseudoboolean
		PBCONSTRAINT
    };
	
	
            
    /**
     * @param _type The formula type to get the string representation for.
     * @return The string representation of the given type.
     */
    inline std::string formulaTypeToString(FormulaType _type) {
        switch (_type) {
			case FormulaType::ITE: return "ite";
			case FormulaType::EXISTS: return "exists";
			case FormulaType::FORALL: return "forall";
            case FormulaType::TRUE: return "true";
            case FormulaType::FALSE: return "false";
			case FormulaType::BOOL: return "bool";
            case FormulaType::NOT: return "not";
            case FormulaType::IMPLIES: return "=>";
            case FormulaType::AND: return "and";
            case FormulaType::OR: return "or";
            case FormulaType::XOR: return "xor";
            case FormulaType::IFF: return "=";
			case FormulaType::CONSTRAINT: return "constraint";
			case FormulaType::VARCOMPARE: return "varcompare";
			case FormulaType::VARASSIGN: return "varassign";
			case FormulaType::BITVECTOR: return "bv";
			case FormulaType::UEQ: return "ueq";
			case FormulaType::PBCONSTRAINT: return "pbconstraint";
			default:
				CARL_LOG_ERROR("carl.formula", "Unknown formula type " << unsigned(_type));
				return "???";
        }
    }
    inline std::ostream& operator<<(std::ostream& os, FormulaType t) {
        return os << formulaTypeToString(t);
    }

    /**
     * Stores the variables and the formula bound by a quantifier.
     */
    template<typename Pol>
    struct QuantifierContent
    {
        /// The quantified variables.
        std::vector<carl::Variable> mVariables;
        /// The formula bound by this quantifier.
        Formula<Pol> mFormula;

        /**
         * Constructs the content of a quantified formula.
         * @param _vars The quantified variables.
         * @param _formula The formula bound by this quantifier.
         */
        QuantifierContent( std::vector<carl::Variable>&& _vars, Formula<Pol>&& _formula ):
            mVariables( std::move(_vars) ), 
            mFormula( std::move(_formula) )
        {}

        /**
         * Checks this content of a quantified formula and the given content of a quantified formula is equal.
         * @param _qc The content of a quantified formula to check for equality.
         * @return true, if this content of a quantified formula and the given content of a quantified formula is equal.
         */
        bool operator==(const QuantifierContent& _qc) const
        {
            return (mFormula == _qc.mFormula) && (mVariables == _qc.mVariables);
        }
    };
    
    template<typename Pol>
    struct ArithmeticConstraintContent
    {
        Formula<Pol> mLhs;
        Relation mRelation;
        
        ArithmeticConstraintContent(Formula<Pol>&& _lhs, Relation _rel):
            mLhs(std::move(_lhs)),
            mRelation(_rel)
        {}
        bool operator==(const ArithmeticConstraintContent& _acc) const {
            return (mLhs == _acc.mLhs) && (mRelation == _acc.mRelation);
        }
    };
	
	
    template<typename Pol>
    class FormulaContent
    {
            friend class Formula<Pol>;
            friend class FormulaPool<Pol>;
			template<typename P>
			friend std::ostream& operator<<(std::ostream& os, const FormulaContent<P>& f);
        
        private:
            
            // Member.
            /// The hash value.
            size_t mHash = 0;
            /// The unique id.
            size_t mId = 0;
            /// The activity for this formula, which means, how much is this formula involved in the solving procedure.
            mutable double mActivity = 0.0;
            /// Some value stating an expected difficulty of solving this formula for satisfiability.
            mutable double mDifficulty = 0.0;
            /// The number of formulas existing with this content.
            mutable size_t mUsages = 0;
            /// The type of this formula.
            FormulaType mType;
            /// The content of this formula.
            union
            {
#ifdef __VS
                /// The variable, in case this formula wraps a variable.
                carl::Variable* mpVariableVS;
                /// The polynomial, in case this formula wraps a polynomial.
                Pol* mpPolynomialVS;
                /// The arithmetic constraint over a formula.
                ArithmeticConstraintContent<Pol>* mpArithmeticVS;
                /// The constraint, in case this formula wraps a constraint.
                Constraint<Pol>* mpConstraintVS;
				/// A constraint comparing a single variable with a value. 
				VariableComparison<Pol>* mpVariableComparisonVS;
				/// A constraint assigning a single variable to a value. 
				VariableAssignment<Pol>* mpVariableAssignmentVS;
                /// The bitvector constraint.
                BVConstraint* mpBVConstraintVS;
                /// The uninterpreted equality, in case this formula wraps an uninterpreted equality.
                UEquality* mpUIEqualityVS;
				/// The pseudoboolean constraint.
				PBConstraint<Pol>* mpPBConstraintVS;
                /// The only sub-formula, in case this formula is an negation.
                Formula<Pol>* mpSubformulaVS;
                /// The subformulas, in case this formula is a n-nary operation as AND, OR, IFF or XOR.
                Formulas<Pol>* mpSubformulasVS;
                /// The quantifed variables and the bound formula, in case this formula is a quantified formula.
                QuantifierContent<Pol>* mpQuantifierContentVS;
#else
				/// The variable, in case this formula wraps a variable.
				carl::Variable mVariable;
				/// The polynomial, in case this formula wraps a polynomial.
				Pol mPolynomial;
				/// The arithmetic constraint over a formula.
				ArithmeticConstraintContent<Pol> mArithmetic;
				/// The constraint, in case this formula wraps a constraint.
				Constraint<Pol> mConstraint;
				/// A constraint comparing a single variable with a value. 
				VariableComparison<Pol> mVariableComparison;
				/// A constraint assigning a single variable to a value. 
				VariableAssignment<Pol> mVariableAssignment;
				/// The bitvector constraint.
				BVConstraint mBVConstraint;
				/// The uninterpreted equality, in case this formula wraps an uninterpreted equality.
				UEquality mUIEquality;
				/// The pseudoboolean constraint.
				PBConstraint<Pol> mPBConstraint;
				/// The only sub-formula, in case this formula is an negation.
				Formula<Pol> mSubformula;
				/// The subformulas, in case this formula is a n-nary operation as AND, OR, IFF or XOR.
				Formulas<Pol> mSubformulas;
				/// The quantifed variables and the bound formula, in case this formula is a quantified formula.
				QuantifierContent<Pol> mQuantifierContent;
#endif
            };
            /// The negation
            const FormulaContent<Pol> *mNegation = nullptr;
            /// The propositions of this formula.
            Condition mProperties;
            /// Mutex for access to activity.
            mutable std::mutex mActivityMutex;
            /// Mutex for access to difficulty.
            mutable std::mutex mDifficultyMutex;
            /// Mutex for collecting the variables within this formula.
            mutable std::mutex mCollectVariablesMutex;
            ///
            mutable bool mTseitinClause = false;
            /// Container collecting the variables which occur in this formula.
            mutable Variables* mpVariables = nullptr;
            
            FormulaContent() = delete;
            FormulaContent(const FormulaContent&) = delete;
            FormulaContent(FormulaContent&&) = delete;

            /**
             * Constructs the formula (true), if the given bool is true and the formula (false) otherwise.
             * @param _true Specifies whether to create the formula (true) or (false).
             * @param _id A unique id of the formula to create.
             */
            FormulaContent(FormulaType _type, std::size_t _id = 0);

            /**
             * Constructs a formula being a variable.
             * @param _variable 
             */
            FormulaContent(Variable _variable);
            
            FormulaContent(Formula<Pol>&& _lhs, Relation _rel);

            /**
             * Constructs a formula being a constraint.
             * @param _constraint The pointer to the constraint.
             */
            FormulaContent(Constraint<Pol>&& _constraint);
			
			FormulaContent(VariableComparison<Pol>&& _variableComparison);
			
			FormulaContent(VariableAssignment<Pol>&& _variableAssignment);
            
            /**
             * Constructs a formula being a bitvector constraint.
             * @param _constraint The pointer to the constraint.
             */
            FormulaContent(BVConstraint&& _constraint);

            /**
             * Constructs a formula being an uninterpreted equality.
             * @param _ueq The pointer to the constraint.
             */
            FormulaContent(UEquality&& _ueq);
			
			/**
             * Constructs a formula being an pseudoboolean constraint.
             * @param _pbc The pointer to the constraint.
             */
            FormulaContent(PBConstraint<Pol>&& _pbc);

            /**
             * Constructs a formula of the given type with a single subformula. This is usually a negation.
             */
            FormulaContent(FormulaType _type,  Formula<Pol>&& _subformula);
            
            FormulaContent(FormulaType _type,  const std::initializer_list<Formula<Pol>>& _subformula);
            FormulaContent(FormulaType _type,  Formulas<Pol>&& _subformula);

            
            /**
             * Constructs a quantifier expression: (exists (vars) term) or (forall (vars) term)
             * @param _type The type of the quantifier to construct.
             * @param _vars The variables that are bound.
             * @param _term The term in which the variables are bound.
             */
            FormulaContent(FormulaType _type, std::vector<carl::Variable>&& _vars, const Formula<Pol>& _term);

            
        public:

            /**
             * Destructor.
             */
            ~FormulaContent()
            {
                /*union
                {
                    /// The variable, in case this formula wraps a variable.
                    carl::Variable mVariable;
                    /// The polynomial, in case this formula wraps a polynomial.
                    Pol mPolynomial;
                    /// The arithmetic constraint over a formula.
                    ArithmeticConstraintContent<Pol> mArithmetic;
                    /// The constraint, in case this formula wraps a constraint.
                    Constraint<Pol> mConstraint;
                    /// The bitvector constraint.
                    BVConstraint mBVConstraint;
                    /// The uninterpreted equality, in case this formula wraps an uninterpreted equality.
                    UEquality mUIEquality;
                    /// The only sub-formula, in case this formula is an negation.
                    Formula<Pol> mSubformula;
                    /// The subformulas, in case this formula is a n-nary operation as AND, OR, IFF or XOR.
                    Formulas<Pol> mSubformulas;
                    /// The quantifed variables and the bound formula, in case this formula is a quantified formula.
                    QuantifierContent<Pol> mQuantifierContent;
                };*/
                switch (mType) {
                    case FormulaType::TRUE: break;
                    case FormulaType::FALSE: break;
                    case FormulaType::BOOL: break;
#ifdef __VS
					case FormulaType::NOT: { mpSubformulaVS->~Formula(); break; }
#else
					case FormulaType::NOT: { mSubformula.~Formula(); break; }
#endif
                    case FormulaType::IMPLIES: 
                    case FormulaType::AND: ;
                    case FormulaType::OR: ;
                    case FormulaType::XOR: ;
                    case FormulaType::IFF: ;
#ifdef __VS
					case FormulaType::ITE: { mpSubformulasVS->~vector(); break; }
                    case FormulaType::EXISTS: ;
					case FormulaType::FORALL: { mpQuantifierContentVS->~QuantifierContent(); break; }
					case FormulaType::CONSTRAINT: { mpConstraintVS->~Constraint(); break; }
					case FormulaType::VARCOMPARE: { mpVariableComparisonVS->~VariableComparison(); break; }
					case FormulaType::BITVECTOR: { mpBVConstraintVS->~BVConstraint(); break; }
					case FormulaType::UEQ: { mpUIEqualityVS->~UEquality(); break; }
#else
					case FormulaType::ITE: { mSubformulas.~vector(); break; }
					case FormulaType::EXISTS:;
					case FormulaType::FORALL: { mQuantifierContent.~QuantifierContent(); break; }
					case FormulaType::CONSTRAINT: { mConstraint.~Constraint(); break; }
					case FormulaType::VARCOMPARE: { mVariableComparison.~VariableComparison(); break; }
					case FormulaType::VARASSIGN: { mVariableAssignment.~VariableAssignment(); break; }
					case FormulaType::BITVECTOR: { mBVConstraint.~BVConstraint(); break; }
					case FormulaType::UEQ: { mUIEquality.~UEquality(); break; }
					case FormulaType::PBCONSTRAINT: { mPBConstraint.~PBConstraint(); break; }
#endif
                }

                if( mpVariables != nullptr )
                    delete mpVariables;
            }

            std::size_t hash() const {
                return mHash;
            }
			std::size_t id() const {
				return mId;
			}
            
            bool isNary() const
            {
                switch (mType) {
                    case FormulaType::TRUE: return false;
                    case FormulaType::FALSE: return false;
                    case FormulaType::BOOL: return false;
                    case FormulaType::NOT: return false;
                    case FormulaType::IMPLIES: return true;
                    case FormulaType::AND: return true;
                    case FormulaType::OR: return true;
                    case FormulaType::XOR: return true;
                    case FormulaType::IFF: return true;
                    case FormulaType::ITE: return true;
                    case FormulaType::EXISTS: return false;
                    case FormulaType::FORALL: return false;
                    case FormulaType::CONSTRAINT: return false;
					case FormulaType::VARCOMPARE: return false;
					case FormulaType::VARASSIGN: return false;
                    case FormulaType::BITVECTOR: return false;
                    case FormulaType::UEQ: return false;
					case FormulaType::PBCONSTRAINT: return false;
                }
                return false;
            }

            bool operator==(const FormulaContent& _content) const;
            
            /**
             * Gives the string representation of this formula content.
             * @param _withActivity A flag which indicates whether to add the formula's activity to the result.
             * @param _resolveUnequal A switch which indicates how to represent the relation symbol for unequal. 
             *                         (for further description see documentation of Constraint::toString( .. ))
             * @param _init The initial string of every row of the result.
             * @param _oneline A flag indicating whether the formula shall be printed on one line.
             * @param _infix A flag indicating whether to print the formula in infix or prefix notation.
             * @param _friendlyNames A flag that indicates whether to print the variables with their internal representation (false)
             *                        or with their dedicated names.
             * @return The resulting string representation of this formula.
             */
            std::string toString( bool _withActivity = false, unsigned _resolveUnequal = 0, const std::string _init = "", bool _oneline = true, bool _infix = false, bool _friendlyNames = true ) const; 
            
    };
/**
     * The output operator of a formula.
     * @param os The stream to print on.
     * @param f
     */
    template<typename P>
    std::ostream& operator<<(std::ostream& os, const FormulaContent<P>& f) {
		switch (f.mType) {
			case FormulaType::FALSE:
				return os << formulaTypeToString(f.mType);
			case FormulaType::TRUE:
				return os << formulaTypeToString(f.mType);
			case FormulaType::BOOL:
				return os << f.mVariable;
			case FormulaType::CONSTRAINT:
				return os << f.mConstraint;
			case FormulaType::VARASSIGN:
				return os << f.mVariableComparison;
			case FormulaType::VARCOMPARE:
				return os << f.mVariableAssignment;
			case FormulaType::BITVECTOR:
				return os << f.mBVConstraint;
			case FormulaType::UEQ:
				return os << f.mUIEquality;
			case FormulaType::PBCONSTRAINT:
				return os << f.mPBConstraint;
			case FormulaType::NOT:
				return os << "!(" << f.mSubformula << ")";
			case FormulaType::EXISTS:
				os << "(exists";
				for (auto v: f.mQuantifierContent.mVariables) os << " " << v;
				return os << ")(" << f.mQuantifierContent.mFormula << ")";
			case FormulaType::FORALL:
				os << "(forall";
				for (auto v: f.mQuantifierContent.mVariables) os << " " << v;
				return os << ")(" << f.mQuantifierContent.mFormula << ")";
			default:
				assert(f.isNary());
				return os << "(" << carl::stream_joined(" " + formulaTypeToString(f.mType) + " ", f.mSubformulas) << ")";
		}
    }
	template<typename P>
    std::ostream& operator<<( std::ostream& _out, const FormulaContent<P>* _formula )
    {
		assert(_formula != nullptr);
		return _out << *_formula;
    }
	
}

#include "FormulaContent.tpp"
