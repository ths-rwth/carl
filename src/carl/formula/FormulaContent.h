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
     * The possible types of a formula.
     *
     * For each (supported) SMTLIB theory, we have
     * - Constants
     * - Variables
     * - Functions
     * - Additional functions (not specified, but used in the wild)
     */
    enum FormulaType {
        // Core Theory
        TRUE, FALSE,
        BOOL,
        NOT, IMPLIES, AND, OR, XOR,
        IFF, 
        
        ITE, EXISTS, FORALL,
		// Arithmetic Theory
		CONSTRAINT,
		
		// Bitvector Theory
		BITVECTOR,
		
		// Uninterpreted Theory
		UEQ
    };
	
	
            
    /**
     * @param _type The formula type to get the string representation for.
     * @return The string representation of the given type.
     */
    inline std::string formulaTypeToString(FormulaType _type) {
        switch (_type) {
            case FormulaType::TRUE: return "true";
            case FormulaType::FALSE: return "false";
            case FormulaType::NOT: return "not";
            case FormulaType::IMPLIES: return "=>";
            case FormulaType::AND: return "and";
            case FormulaType::OR: return "or";
            case FormulaType::XOR: return "xor";
            case FormulaType::IFF: return "=";
            case FormulaType::ITE: return "ite";
            default:
                return "";
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
        ~QuantifierContent() {}

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
    class FormulaContent
    {
            friend class Formula<Pol>;
            friend class FormulaPool<Pol>;
        
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
            /// The unique id.
            mutable size_t mUsages = 0;
            /// The type of this formula.
            FormulaType mType;
            /// The content of this formula.
            union
            {
                /// The variable, in case this formula wraps a variable.
                carl::Variable mVariable;
                /// The constraint, in case this formulas wraps a constraint.
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
            };
            /// The negation
            const FormulaContent<Pol> *mNegation = nullptr;
            /// The propositions of this formula.
            Condition mProperties;
            /// Mutex for access to activity.
            mutable std::mutex mActivityMutex;
            /// Mutex for access to difficulty.
            mutable std::mutex mDifficultyMutex;
            ///
            mutable bool mTseitinClause = false;
            
            FormulaContent() = delete;
            FormulaContent(const FormulaContent&) = delete;
            FormulaContent(FormulaContent&&) = delete;

            /**
             * Constructs the formula (true), if the given bool is true and the formula (false) otherwise.
             * @param _true Specifies whether to create the formula (true) or (false).
             * @param _id A unique id of the formula to create.
             */
            FormulaContent(FormulaType _type, size_t _id = 0);

            /**
             * Constructs a formula being a variable.
             * @param _variable 
             */
            FormulaContent(carl::Variable::Arg _variable);

            /**
             * Constructs a formula being a constraint.
             * @param _constraint The pointer to the constraint.
             */
            FormulaContent(Constraint<Pol>&& _constraint);
            
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
            ~FormulaContent() {}

            std::size_t hash() const {
                return mHash;
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
                    case FormulaType::BITVECTOR: return false;
                    case FormulaType::UEQ: return false;
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
            
            /**
             * The output operator of a formula.
             * @param _out The stream to print on.
             * @param _formula
             */
            template<typename P>
            friend std::ostream& operator<<( std::ostream& _out, const FormulaContent<P>& _formula )
            {
                return (_out << _formula.toString());
            }
    };
	
}

#include "FormulaContent.tpp"
