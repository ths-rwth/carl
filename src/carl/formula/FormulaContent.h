#pragma once

#include "../core/logging.h"

#include <iostream>
#include <variant>

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
    using FormulasMulti = std::multiset<Formula<Poly>>;
	
    
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
		UEQ
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
    class FormulaContent : public boost::intrusive::unordered_set_base_hook<>
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
            std::variant<carl::Variable, // The variable, in case this formula wraps a variable.
                        Constraint<Pol>, // The constraint, in case this formula wraps a constraint.
                        VariableComparison<Pol>, // A constraint comparing a single variable with a value. 
                        VariableAssignment<Pol>, // A constraint assigning a single variable to a value. 
                        BVConstraint, // The bitvector constraint.
                        UEquality, // The uninterpreted equality, in case this formula wraps an uninterpreted equality.
                        Formula<Pol>, // The only sub-formula, in case this formula is an negation.
                        Formulas<Pol>, // The subformulas, in case this formula is a n-nary operation as AND, OR, IFF or XOR.
                        QuantifierContent<Pol>> mContent; // The quantifed variables and the bound formula, in case this formula is a quantified formula.
            /// The negation
            const FormulaContent<Pol> *mNegation = nullptr;
            /// The propositions of this formula.
            Condition mProperties;
            /// Mutex for access to activity.
            mutable std::mutex mActivityMutex;
            /// Mutex for access to difficulty.
            mutable std::mutex mDifficultyMutex;
            /// Mutex for collecting the variables within this formula.
            mutable std::mutex mVariablesMutex;
            /// Container collecting the variables which occur in this formula.
            mutable Variables* mpVariables = nullptr;
            
            FormulaContent() = delete;
            FormulaContent(const FormulaContent&) = delete;
            FormulaContent(FormulaContent&& o) : mHash(o.mHash), mId(o.mId), mType(o.mType), mContent(std::move(o.mContent)) {};

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
            ~FormulaContent() {
                // TODO NOTE: in case of true, false, bool: mContent was not destroyed ...
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
                }
                return false;
            }

            bool operator==(const FormulaContent& _content) const;
    };
	/**
	 * The output operator of a formula.
	 * @param os The stream to print on.
	 * @param f
	 */
	template<typename Pol>
	std::ostream& operator<<(std::ostream& os, const FormulaContent<Pol>& f) {
		switch (f.mType) {
			case FormulaType::FALSE:
				return os << formulaTypeToString(f.mType);
			case FormulaType::TRUE:
				return os << formulaTypeToString(f.mType);
			case FormulaType::BOOL:
				return os << std::get<carl::Variable>(f.mContent);
			case FormulaType::CONSTRAINT:
				return os << std::get<Constraint<Pol>>(f.mContent);
			case FormulaType::VARASSIGN:
				return os << std::get<VariableAssignment<Pol>>(f.mContent);
			case FormulaType::VARCOMPARE:
				return os << std::get<VariableComparison<Pol>>(f.mContent);
			case FormulaType::BITVECTOR:
				return os << std::get<BVConstraint>(f.mContent);
			case FormulaType::UEQ:
				return os << std::get<UEquality>(f.mContent);
			case FormulaType::NOT:
				return os << "!(" << std::get<Formula<Pol>>(f.mContent) << ")";
			case FormulaType::EXISTS:
				os << "(exists";
				for (auto v: std::get<QuantifierContent<Pol>>(f.mContent).mVariables) os << " " << v;
				return os << ")(" << std::get<QuantifierContent<Pol>>(f.mContent).mFormula << ")";
			case FormulaType::FORALL:
				os << "(forall";
				for (auto v: std::get<QuantifierContent<Pol>>(f.mContent).mVariables) os << " " << v;
				return os << ")(" << std::get<QuantifierContent<Pol>>(f.mContent).mFormula << ")";
			default:
				assert(f.isNary());
				return os << "(" << carl::stream_joined(" " + formulaTypeToString(f.mType) + " ", std::get<Formulas<Pol>>(f.mContent)) << ")";
		}
	}

	template<typename Pol>
	std::ostream& operator<<(std::ostream& os, const FormulaContent<Pol>* fc) {
		assert(fc != nullptr);
		return os << *fc;
	}
	
}

#include "FormulaContent.tpp"
