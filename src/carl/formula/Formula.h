/**
 * @file Formula.h
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 * @author Ulrich Loup
 * @author Sebastian Junges
 * @since 2012-02-09
 * @version 2014-10-30
 */

#pragma once

#include <string.h>
#include <string>
#include <set>
#include <boost/dynamic_bitset.hpp>
#include "Condition.h"
#include "Constraint.h"
#include "UEquality.h"

namespace carl
{
    // Forward definition.
    template<typename Pol>
    class Formula;

    template<typename Poly>
    using Formulas = std::set<Formula<Poly>, carl::less<Formula<Poly>, false>>;
	template<typename Poly>
    using FormulasMulti = std::multiset<Formula<Poly>, carl::less<Formula<Poly>, false>>;
    
    /**
     * Stores the sub-formulas of a formula being an implication.
     */
    template<typename Pol>
    struct IMPLIESContent
    {
        /// The premise of the implication.
        Formula<Pol> mPremise;
        /// The conclusion of the implication.
        Formula<Pol> mConclusion;

        /**
         * Constructs the content of a formula being an implication.
         * @param _premise The premise of the implication.
         * @param _conclusion The conclusion of the implication.
         */
        IMPLIESContent( const Formula<Pol>& _premise, const Formula<Pol>& _conclusion): 
            mPremise( _premise ), mConclusion( _conclusion ) {}
    };

    /**
     * Stores the sub-formulas of a formulas being an if-then-else expression of formulas.
     */
    template<typename Pol>
    struct ITEContent
    {
        /// The condition of the if-then-else expression.
        Formula<Pol> mCondition;
        /// The then-case of the if-then-else expression.
        Formula<Pol> mThen;
        /// The else-case of if-then-else expression.
        Formula<Pol> mElse;

        /**
         * Constructs the content of a formula being an implication.
         * @param _condition The condition of the if-then-else expression.
         * @param _then The then-case of the if-then-else expression.
         * @param _else The else-case of if-then-else expression.
         */
        ITEContent( const Formula<Pol>& _condition, const Formula<Pol>& _then, const Formula<Pol>& _else ): 
            mCondition( _condition ), mThen( _then ), mElse( _else ) {}
    };

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
        QuantifierContent( const std::vector<carl::Variable>&& _vars, const Formula<Pol>& _formula ):
            mVariables( _vars ), 
            mFormula( _formula )
        {}

        /**
         * Checks this content of a quantified formula and the given content of a quantified formula is equal.
         * @param _qc The content of a quantified formula to check for equality.
         * @return true, if this content of a quantified formula and the given content of a quantified formula is equal.
         */
        bool operator==( const QuantifierContent& _qc )
        {
            return (mFormula == _qc.mFormula) && (mVariables == _qc.mVariables);
        }
    };
    
    // Forward declaration
    template<typename Pol>
    class FormulaPool;
    
    
    /// The possible types of a formula.
    enum FormulaType { AND, OR, NOT, IFF, XOR, IMPLIES, ITE, BOOL, CONSTRAINT, TRUE, FALSE, EXISTS, FORALL, UEQ };
            
    /**
     * @param _type The formula type to get the string representation for.
     * @return The string representation of the given type.
     */
    inline std::string formulaTypeToString( FormulaType _type )
    {
        switch( _type )
        {
            case FormulaType::AND:
                return "and";
            case FormulaType::OR:
                return "or";
            case FormulaType::NOT:
                return "not";
            case FormulaType::IFF:
                return "=";
            case FormulaType::XOR:
                return "xor";
            case FormulaType::IMPLIES:
                return "=>";
            case FormulaType::ITE:
                return "ite";
            case FormulaType::TRUE:
                return "true";
            case FormulaType::FALSE:
                return "false";
            default:
                return "";
        }
    }
    
    template<typename Pol>
    class FormulaContent
    {
            friend class Formula<Pol>;
            friend class FormulaPool<Pol>;
        
        private:
            
            // Member.
            
            /// The negation
            FormulaContent<Pol> *mNegation;
            /// The hash value.
            size_t mHash;
            /// The unique id.
            size_t mId;
            /// The activity for this formula, which means, how much is this formula involved in the solving procedure.
            mutable double mActivity;
            /// Some value stating an expected difficulty of solving this formula for satisfiability.
            mutable double mDifficulty;
            /// The type of this formula.
            FormulaType mType;
            /// The content of this formula.
            union
            {
                /// The only sub-formula, in case this formula is an negation.
                Formula<Pol> mSubformula;
                /// The premise and conclusion, in case this formula is an implication.
                IMPLIESContent<Pol>* mpImpliesContent;
                /// The condition, then- and else-case, in case this formula is an ite-expression of formulas.
                ITEContent<Pol>* mpIteContent;
                /// The quantifed variables and the bound formula, in case this formula is a quantified formula.
                QuantifierContent<Pol>* mpQuantifierContent;
                /// The subformulas, in case this formula is a n-nary operation as AND, OR, IFF or XOR.
                Formulas<Pol>* mpSubformulas;
                /// The constraint, in case this formulas wraps a constraint.
                const Constraint<Pol>* mpConstraint;
                /// The Boolean variable, in case this formula wraps a Boolean variable.
                carl::Variable mBoolean;
                /// The uninterpreted equality, in case this formula wraps an uninterpreted equality.
                UEquality mUIEquality;
            };
            /// The propositions of this formula.
            Condition mProperties;

            /**
             * Constructs the formula (true), if the given bool is true and the formula (false) otherwise.
             * @param _true Specifies whether to create the formula (true) or (false).
             * @param _id A unique id of the formula to create.
             */
            FormulaContent( bool _true, size_t _id = 0 );

            /**
             * Constructs a formula being a Boolean variable.
             * @param _boolean The pointer to the string representing the name of the Boolean variable.
             */
            FormulaContent( carl::Variable::Arg _boolean );

            /**
             * Constructs a formula being a constraint.
             * @param _constraint The pointer to the constraint.
             */
            FormulaContent( const Constraint<Pol>* _constraint );

            /**
             * Constructs a formula being an uninterpreted equality.
             * @param _ueq The pointer to the constraint.
             */
            FormulaContent( UEquality&& _ueq );

            /**
             * Constructs the negation of the given formula: (not _subformula)
             * @param _subformula The sub-formula, which is negated by the constructed formula.
             */
            FormulaContent( const Formula<Pol>& _subformula );

            /**
             * Constructs an implication from the first argument to the second: (=> _subformulaA _subformulaB)
             * @param _premise The premise of the formula to create.
             * @param _conclusion The conclusion of the formula to create.
             */
            FormulaContent( const Formula<Pol>& _premise, const Formula<Pol>& _conclusion );

            /**
             * Constructs an if-then-else (ITE) expression: (ite _condition _then _else)
             * @param _condition The condition of the ITE expression to create.
             * @param _then The first case of the ITE expression to create.
             * @param _else The second case of the ITE expression to create.
             */
            FormulaContent( const Formula<Pol>& _condition, const Formula<Pol>& _then, const Formula<Pol>& _else );

            /**
             * Constructs a quantifier expression: (exists (vars) term) or (forall (vars) term)
             * @param _type The type of the quantifier to construct.
             * @param _vars The variables that are bound.
             * @param _term The term in which the variables are bound.
             */
            FormulaContent(const FormulaType _type, const std::vector<carl::Variable>&& _vars, const Formula<Pol>& _term);

            /**
             * Constructs the formula of the given type. 
             * @param _type The type of the formula to construct.
             * @param _subformulas The sub-formulas of the formula to construct.
             */
            FormulaContent( const FormulaType _type, Formulas<Pol>&& _subformulas );

            FormulaContent(); // Disabled
            FormulaContent( const FormulaContent& ); // Disabled

        public:

            /**
             * Destructor.
             */
            ~FormulaContent();

            size_t hash() const
            {
                return mHash;
            }

            bool operator==( const FormulaContent& _content ) const;
            
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
    
    /**
     * The formula class.
     */
    template<typename Pol>
    class Formula
    {
        friend class FormulaPool<Pol>;
        
        public:
            /// A constant iterator to a sub-formula of a formula.
            typedef typename Formulas<Pol>::const_iterator const_iterator;
            /// A constant reverse iterator to a sub-formula of a formula.
            typedef typename Formulas<Pol>::const_reverse_iterator const_reverse_iterator;
            
            /**
             * Adds the propositions of the given constraint to the propositions of this formula.
             * @param _constraint The constraint to add propositions for.
			 * @param _properties
             */
            static void addConstraintProperties( const Constraint<Pol>& _constraint, Condition& _properties );
            
        private:
            // Members.

            /// The content of this formula.
            const FormulaContent<Pol>* mpContent;
            
            
            Formula( const FormulaContent<Pol>* _content ):
                mpContent( _content )
            {}
            
        public:
            
            /**
             * Gets the propositions of this formula. It updates and stores the propositions
             * if they are not up to date, hence this method is quite efficient.
             */
            static void init( FormulaContent<Pol>& _content );
            
            explicit Formula( FormulaType _type = TRUE ):
                mpContent( _type == TRUE ? FormulaPool<Pol>::getInstance().trueFormula() : FormulaPool<Pol>::getInstance().falseFormula() )
            {}
                
            explicit Formula( Variable::Arg _booleanVar ):
                mpContent( FormulaPool<Pol>::getInstance().create( _booleanVar ) )
            {}
                
            explicit Formula( const Pol& _pol, Relation _rel ):
                mpContent( FormulaPool<Pol>::getInstance().create( ConstraintPool<Pol>::getInstance().newConstraint( _pol, _rel ) ) )
            {}
                
            explicit Formula( Pol&& _pol, Relation _rel ):
                mpContent( FormulaPool<Pol>::getInstance().create( ConstraintPool<Pol>::getInstance().newConstraint( std::move( _pol ), _rel ) ) )
            {}
                
            explicit Formula( const Constraint<Pol>* _constraint ):
                mpContent( FormulaPool<Pol>::getInstance().create( _constraint ) )
            {}
                
            explicit Formula( FormulaType _type, const Formula& _subformula ):
                mpContent( FormulaPool<Pol>::getInstance().createNegation( _subformula ) )
            {
                assert( _type == FormulaType::NOT );
            }
                
            explicit Formula( FormulaType _type, const Formula& _subformulaA, const Formula& _subformulaB )
            {
                assert( _type == FormulaType::AND || _type == FormulaType::IFF || _type == FormulaType::IMPLIES || _type == FormulaType::OR || _type == FormulaType::XOR );
                if( _type == FormulaType::IMPLIES )
                {
                    mpContent = FormulaPool<Pol>::getInstance().createImplication( _subformulaA, _subformulaB );
                }
                else
                {
                    mpContent = FormulaPool<Pol>::getInstance().create( _type, _subformulaA, _subformulaB );
                }
            }
            
            explicit Formula( FormulaType _type, const Formula& _subformulaA, const Formula& _subformulaB, const Formula& _subformulaC )
            {
                assert( _type == FormulaType::AND || _type == FormulaType::IFF || _type == FormulaType::ITE || _type == FormulaType::OR || _type == FormulaType::XOR );
                if( _type == FormulaType::ITE )
                {
                    mpContent = FormulaPool<Pol>::getInstance().createIte( _subformulaA, _subformulaB, _subformulaC );
                }
                else
                {
                    Formulas<Pol> subFormulas;
                    subFormulas.insert( _subformulaA );
                    subFormulas.insert( _subformulaB );
                    subFormulas.insert( _subformulaC );
                    mpContent = FormulaPool<Pol>::getInstance().create( _type, subFormulas );
                }
            }
            
            explicit Formula( FormulaType _type, const FormulasMulti<Pol>& _subformulas ):
                mpContent( FormulaPool<Pol>::getInstance().create( _subformulas ) )
            {
                assert( _type == FormulaType::XOR );
            }
            
            explicit Formula( FormulaType _type, const Formulas<Pol>& _subasts ):
                mpContent( FormulaPool<Pol>::getInstance().create( _type, _subasts ) )
            {}
            
            explicit Formula( FormulaType _type, Formulas<Pol>&& _subasts ):
                mpContent( FormulaPool<Pol>::getInstance().create( _type, std::move(_subasts) ) )
            {}
            
            explicit Formula( FormulaType _type, const std::vector<Variable>&& _vars, const Formula& _term ):
                mpContent( FormulaPool<Pol>::getInstance().create( _type, std::move( _vars ), _term ) )
            {}
            
            explicit Formula( FormulaType _type, const std::vector<Variable>& _vars, const Formula& _term ):
                Formula( _type, std::move( std::vector<Variable>( _vars ) ), _term )
            {}
            
            explicit Formula( const UEquality::Arg& _lhs, const UEquality::Arg& _rhs, bool _negated ):
                mpContent( FormulaPool<Pol>::getInstance().create( _lhs, _rhs, _negated ) )
            {}
            
            explicit Formula( UEquality&& _eq ):
                mpContent( FormulaPool<Pol>::getInstance().create( std::move( _eq ) ) )
            {}

            // Methods.

            /**
             * @return Some value stating an expected difficulty of solving this formula for satisfiability.
             */
            const double& difficulty() const
            {
                return mpContent->mDifficulty;
            }

            /**
             * Sets the difficulty to the given value.
             * @param difficulty The value to set the difficulty to.
             */
            void setDifficulty( double difficulty ) const
            {
                mpContent->mDifficulty = difficulty;
            }

            /**
             * @return The activity for this formula, which means, how much is this formula involved in the solving procedure.
             */
            double activity() const
            {
                return mpContent->mActivity;
            }

            /**
             * Sets the activity to the given value.
             * @param _activity The value to set the activity to.
             */
            void setActivity( double _activity ) const
            {
                mpContent->mActivity = _activity;
            }

            /**
             * @return The type of this formula.
             */
            FormulaType getType() const
            {
                return mpContent->mType;
            }
            
            /**
             * @return A hash value for this formula.
             */
            size_t getHash() const
            {
                return mpContent->mHash;
            }
            
            /**
             * @return The unique id for this formula.
             */
            size_t getId() const
            {
                return mpContent->mId;
            }
            
            /**
             * @return true, if this formula represents TRUE.
             */
            bool isTrue() const
            {
                return mpContent->mType == FormulaType::TRUE;
            }
            
            /**
             * @return true, if this formula represents FALSE.
             */
            bool isFalse() const
            {
                return mpContent->mType == FormulaType::FALSE;
            }

            /**
             * @return The bit-vector representing the propositions of this formula. For further information see the Condition class.
             */
            const Condition& properties() const
            {
                return mpContent->mProperties;
            }

            /**
             * Collects all real valued variables occurring in this formula.
             * @param _realVars The container to collect the real valued variables in.
             */
            void realValuedVars( Variables& _realVars ) const
            {
                collectVariables( _realVars, carl::VariableType::VT_REAL );
            }
            
            /**
             * Collects all integer valued variables occurring in this formula.
             * @param _intVars The container to collect the integer valued variables in.
             */
            void integerValuedVars( Variables& _intVars ) const
            {
                collectVariables( _intVars, carl::VariableType::VT_INT );
            }
            
            /**
             * Collects all arithmetic variables occurring in this formula.
             * @param _arithmeticVars The container to collect the arithmetic variables in.
             */
            void arithmeticVars( Variables& _arithmeticVars ) const
            {
                collectVariables( _arithmeticVars, carl::VariableType::VT_BOOL, false );
            }
            
            /**
             * Collects all arithmetic variables occurring in this formula.
             * @param _booleanVars The container to collect the arithmetic variables in.
             */
            void booleanVars( Variables& _booleanVars ) const
            {
                collectVariables( _booleanVars, carl::VariableType::VT_BOOL );
            }
            
            /**
             * @return A constant reference to the only sub-formula, in case this formula is an negation.
             */
            const Formula& subformula() const
            {
                assert( mpContent->mType == NOT );
                return mpContent->mSubformula;
            }
            
            /**
             * @return A constant reference to the premise, in case this formula is an implication.
             */
            const Formula& premise() const
            {
                assert( mpContent->mType == IMPLIES );
                return mpContent->mpImpliesContent->mPremise;
            }
            
            /**
             * @return A constant reference to the conclusion, in case this formula is an implication.
             */
            const Formula& conclusion() const
            {
                assert( mpContent->mType == IMPLIES );
                return mpContent->mpImpliesContent->mConclusion;
            }
            
            /**
             * @return A constant reference to the condition, in case this formula is an ite-expression of formulas.
             */
            const Formula& condition() const
            {
                assert( mpContent->mType == ITE );
                return mpContent->mpIteContent->mCondition;
            }
            
            /**
             * @return A constant reference to the then-case, in case this formula is an ite-expression of formulas.
             */
            const Formula& firstCase() const
            {
                assert( mpContent->mType == ITE );
                return mpContent->mpIteContent->mThen;
            }
            
            /**
             * @return A constant reference to the else-case, in case this formula is an ite-expression of formulas.
             */
            const Formula& secondCase() const
            {
                assert( mpContent->mType == ITE );
                return mpContent->mpIteContent->mElse;
            }

            /**
             * @return A constant reference to the quantifed variables, in case this formula is a quantified formula.
             */
			const std::vector<carl::Variable>& quantifiedVariables() const
			{
				assert( mpContent->mType == FormulaType::EXISTS || mpContent->mType == FormulaType::FORALL );
				return mpContent->mpQuantifierContent->mVariables;
			}

            /**
             * @return A constant reference to the bound formula, in case this formula is a quantified formula.
             */
			const Formula& quantifiedFormula() const
			{
				assert( mpContent->mType == FormulaType::EXISTS || mpContent->mType == FormulaType::FORALL );
				return mpContent->mpQuantifierContent->mFormula;
			}

            /**
             * @return A constant reference to the list of sub-formulas of this formula. Note, that
             *          this formula has to be a Boolean combination, if you invoke this method.
             */
            const Formulas<Pol>& subformulas() const
            {
                assert( isNary() );
                return *mpContent->mpSubformulas;
            }

            /**
             * @return A pointer to the constraint represented by this formula. Note, that
             *          this formula has to be of type CONSTRAINT, if you invoke this method.
             */
            const Constraint<Pol>* pConstraint() const
            {
                assert( mpContent->mType == FormulaType::CONSTRAINT || mpContent->mType == FormulaType::TRUE || mpContent->mType == FormulaType::FALSE );
                return mpContent->mpConstraint;
            }

            /**
             * @return A constant reference to the constraint represented by this formula. Note, that
             *          this formula has to be of type CONSTRAINT, if you invoke this method.
             */
            const Constraint<Pol>& constraint() const
            {
                assert( mpContent->mType == FormulaType::CONSTRAINT || mpContent->mType == FormulaType::TRUE || mpContent->mType == FormulaType::FALSE );
                return *mpContent->mpConstraint;
            }

            /**
             * @return The name of the Boolean variable represented by this formula. Note, that
             *          this formula has to be of type BOOL, if you invoke this method.
             */
            carl::Variable::Arg boolean() const
            {
                assert( mpContent->mType == FormulaType::BOOL );
                return mpContent->mBoolean;
            }
            
            /**
             * @return A constant reference to the uninterpreted equality represented by this formula. Note, that
             *          this formula has to be of type UEQ, if you invoke this method.
             */
            const UEquality& uequality() const
            {
                assert( mpContent->mType == FormulaType::UEQ );
                return mpContent->mUIEquality;
            }

            /**
             * @return The number of sub-formulas of this formula.
             */
            size_t size() const
            {
                if( mpContent->mType == FormulaType::BOOL || mpContent->mType == FormulaType::CONSTRAINT || mpContent->mType == FormulaType::TRUE 
                        || mpContent->mType == FormulaType::FALSE || mpContent->mType == FormulaType::NOT )
                    return 1;
                else if( mpContent->mType == FormulaType::IMPLIES )
                    return 2;
                else
                    return mpContent->mpSubformulas->size();
            }

            /**
             * @return true, if this formula has sub-formulas;
             *          false, otherwise.
             */
            bool empty() const
            {
                if( mpContent->mType == FormulaType::BOOL || mpContent->mType == FormulaType::CONSTRAINT 
                        || mpContent->mType == FormulaType::TRUE || mpContent->mType == FormulaType::FALSE )
                    return false;
                else
                    return mpContent->mpSubformulas->empty();
            }

            /**
             * @return A constant iterator to the beginning of the list of sub-formulas of this formula.
             */
            const_iterator begin() const
            {
                assert( isNary() );
                return mpContent->mpSubformulas->begin();
            }

            /**
             * @return A constant iterator to the end of the list of sub-formulas of this formula.
             */
            const_iterator end() const
            {
                assert( mpContent->mType == FormulaType::AND || mpContent->mType == FormulaType::OR 
                        || mpContent->mType == FormulaType::IFF || mpContent->mType == FormulaType::XOR );
                return mpContent->mpSubformulas->end();
            }

            /**
             * @return A constant reverse iterator to the beginning of the list of sub-formulas of this formula.
             */
            const_reverse_iterator rbegin() const
            {
                assert( isNary() );
                return mpContent->mpSubformulas->rbegin();
            }

            /**
             * @return A constant reverse iterator to the end of the list of sub-formulas of this formula.
             */
            const_reverse_iterator rend() const
            {
                assert( isNary() );
                return mpContent->mpSubformulas->rend();
            }

            /**
             * @return A reference to the last sub-formula of this formula.
             */
            const Formula& back() const
            {
                assert( isBooleanCombination() );
                if( mpContent->mType == FormulaType::NOT )
                    return mpContent->mSubformula;
                else if( mpContent->mType == FormulaType::IMPLIES )
                    return mpContent->mpImpliesContent->mConclusion;
                else if( mpContent->mType == FormulaType::ITE )
                    return mpContent->mpIteContent->mElse;
                else
                    return *(--(mpContent->mpSubformulas->end()));
            }
            
            /**
             * Checks if the given property holds for this formula. (Very cheap operation which only relies on bit checks)
             * @param _property The property to check this formula for.
             * @return true, if the given property holds for this formula;
             *         false, otherwise.
             */
            bool propertyHolds( const Condition& _property ) const
            {
                return (mpContent->mProperties | ~_property) == ~PROP_TRUE;
            }

            /**
             * @return true, if this formula is a Boolean atom.
             */
            bool isAtom() const
            {
                return (mpContent->mType == FormulaType::CONSTRAINT || mpContent->mType == FormulaType::BOOL 
                        || mpContent->mType == FormulaType::FALSE || mpContent->mType == FormulaType::TRUE);
            }

            /**
             * @return true, if the outermost operator of this formula is Boolean;
             *          false, otherwise.
             */
            bool isBooleanCombination() const
            {
                return !isAtom();
            }

            /**
             * @return true, if the type of this formulas allows n-ary combinations of sub-formulas, for an arbitrary n.
             */
            bool isNary() const
            {
                return (mpContent->mType == FormulaType::AND || mpContent->mType == FormulaType::OR 
                        || mpContent->mType == FormulaType::IFF || mpContent->mType == FormulaType::XOR);
            }
            
            /**
             * @return true, if this formula is a conjunction of constraints;
             *          false, otherwise.
             */
            bool isConstraintConjunction() const
            {
                if( PROP_IS_PURE_CONJUNCTION <= properties() )
                    return !(PROP_CONTAINS_BOOLEAN <= properties());
                else
                    return false;
            }

            /**
             * @return true, if this formula is a conjunction of real constraints;
             *          false, otherwise.
             */
            bool isRealConstraintConjunction() const
            {
                if( PROP_IS_PURE_CONJUNCTION <= properties() )
                    return (!(PROP_CONTAINS_INTEGER_VALUED_VARS <= properties()) && !(PROP_CONTAINS_BOOLEAN <= properties()));
                else
                    return false;
            }

            /**
             * @return true, if this formula is a conjunction of integer constraints;
             *         false, otherwise.
             */
            bool isIntegerConstraintConjunction() const
            {
                if( PROP_IS_PURE_CONJUNCTION <= properties() )
                    return (!(PROP_CONTAINS_REAL_VALUED_VARS <= properties()) && !(PROP_CONTAINS_BOOLEAN <= properties()));
                else
                    return false;
            }

            /**
             * @param _formula The pointer to the formula for which to check whether it points to a sub-formula
             *                  of this formula.
             * @return true, the given pointer to a formula points to a sub-formula of this formula;
             *          false, otherwise.
             */
            bool contains( const Formula& _formula ) const
            {
                if( isAtom() )
                    return false;
                if( mpContent->mType == FormulaType::NOT )
                    return mpContent->mSubformula == _formula;
                else if( mpContent->mType == FormulaType::IMPLIES )
                    return (mpContent->mpImpliesContent->mPremise == _formula || mpContent->mpImpliesContent->mConclusion == _formula);
                else if( mpContent->mType == FormulaType::ITE )
                    return (mpContent->mpIteContent->mCondition == _formula || mpContent->mpIteContent->mThen == _formula || mpContent->mpIteContent->mElse == _formula);
                else
                    return mpContent->mpSubformulas->find( _formula ) != mpContent->mpSubformulas->end();
            }
            
            /**
             * Collects all constraint occurring in this formula.
             * @param _constraints The container to insert the constraint into.
             */
            void getConstraints( std::vector<const Constraint<Pol>*>& _constraints ) const
            {
                if( mpContent->mType == FormulaType::CONSTRAINT )
                    _constraints.push_back( mpContent->mpConstraint );
                else if( mpContent->mType == FormulaType::AND || mpContent->mType == FormulaType::OR || mpContent->mType == FormulaType::NOT 
                        || mpContent->mType == FormulaType::IFF || mpContent->mType == FormulaType::XOR || mpContent->mType == FormulaType::IMPLIES )
                    for( const_iterator subAst = mpContent->mpSubformulas->begin(); subAst != mpContent->mpSubformulas->end(); ++subAst )
                        (*subAst)->getConstraints( _constraints );
            }

            /**
             * Collects all Boolean variables occurring in this formula.
             * @param _vars The container to collect the Boolean variables in.
			 * @param _type
			 * @param _ofThisType
             */
            void collectVariables( Variables& _vars, carl::VariableType _type, bool _ofThisType = true ) const;
            
            /**
             * @param _formula The formula to compare with.
             * @return true, if this formula and the given formula are equal;
             *         false, otherwise.
             */
            bool operator==( const Formula& _formula ) const;
            
            /**
             * @param _formula The formula to compare with.
             * @return true, if this formula and the given formula are not equal.
             */
            bool operator!=( const Formula& _formula ) const
            {
                assert( mpContent->mId != 0 );
                assert( _formula.getId() != 0 );
                return mpContent->mId != _formula.getId();
            }
            
            /**
             * @param _formula The formula to compare with.
             * @return true, if the id of this formula is less than the id of the given one.
             */
            bool operator<( const Formula& _formula ) const
            {
                assert( mpContent->mId != 0 );
                assert( _formula.getId() != 0 );
                return mpContent->mId < _formula.getId();
            }
            
            /**
             * @param _formula The formula to compare with.
             * @return true, if the id of this formula is greater than the id of the given one.
             */
            bool operator>( const Formula& _formula ) const
            {
                assert( mpContent->mId != 0 );
                assert( _formula.getId() != 0 );
                return mpContent->mId > _formula.getId();
            }
            
            /**
             * @param _formula The formula to compare with.
             * @return true, if the id of this formula is less or equal than the id of the given one.
             */
            bool operator<=( const Formula& _formula ) const
            {
                assert( mpContent->mId != 0 );
                assert( _formula.getId() != 0 );
                return mpContent->mId <= _formula.getId();
            }
            
            /**
             * @param _formula The formula to compare with.
             * @return true, if the id of this formula is greater or equal than the id of the given one.
             */
            bool operator>=( const Formula& _formula ) const
            {
                assert( mpContent->mId != 0 );
                assert( _formula.getId() != 0 );
                return mpContent->mId >= _formula.getId();
            }
            
            /**
             * @param _assignment The assignment for which to check whether this formula is satisfied by it.
             * @return 0, if this formula is violated by the given assignment;
             *         1, if this formula is satisfied by the given assignment;
             *         2, otherwise.
             */
            unsigned satisfiedBy( const EvaluationMap<typename Pol::NumberType>& _assignment ) const;
            
        private:
            
            /**
             * @return 
             */
            void initHash();
            
        public:
            
            /**
             * Gives the string representation of this formula.
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
            inline std::string toString( bool _withActivity = false, unsigned _resolveUnequal = 0, const std::string _init = "", bool _oneline = true, bool _infix = false, bool _friendlyNames = true ) const
            {
                return mpContent->toString( _withActivity, _resolveUnequal, _init, _oneline, _infix, _friendlyNames );
            }
            
            /**
             * The output operator of a formula.
             * @param _out The stream to print on.
             * @param _formula
             */
            template<typename P>
            friend std::ostream& operator<<( std::ostream& _out, const Formula<P>& _formula );
            
            /**
             * Prints the propositions of this formula.
             * @param _out The stream to print on.
             * @param _init The string to print initially in every row.
             */
            void printProposition( std::ostream& _out = std::cout, const std::string _init = "" ) const;
            
            /**
             * @param _withVariables A flag indicating whether the variables shall be displayed before the formula.
             * @return A string which represents this formula in the input format of the tool Redlog.
             */
            std::string toRedlogFormat( bool _withVariables = true ) const;
            
            /**
             * Gets a string, which represents all variables occurring in this formula in a row, separated by the given separator.
             * @param _separator The separator between the variables.
             * @param _variableIds Maps variable names to names, which shall be used instead in the result.
             * @return The string, which represents all variables occurring in this formula in a row, separated by the given separator.
             */
            std::string variableListToString( std::string _separator = ",", const std::unordered_map<std::string, std::string>& _variableIds = (std::unordered_map<std::string, std::string>())) const;
            
            /**
             * Resolves the outermost negation of this formula.
             * @param _keepConstraints A flag indicating whether to change constraints in order 
             * to resolve the negation in front of them, or to keep the constraints and leave 
             * the negation.
             */
            Formula resolveNegation( bool _keepConstraints = true ) const;
            
            /**
             * [Auxiliary method]
             * @return The formula combining the first to the second last sub-formula of this formula by the 
             *         same operator as the one of this formula.
             *         Example: this = (op a1 a2 .. an) -> return = (op a1 .. an-1)
             *                  If n = 2, return = a1
             */
            Formula connectPrecedingSubformulas() const;

			/**
			 * Transforms this formula to its quantifier free equivalent.
			 * The quantifiers are represented by the parameter variables. Each entry in variables contains all variables between two quantifier alternations.
			 * The even entries (starting with 0) are quantified existentially, the odd entries are quantified universally.
			 * @param variables Contains the quantified variables.
			 * @param level Used for internal recursion.
			 * @param negated Used for internal recursion.
			 * @return The quantifier-free version of this formula.
			 */
			Formula toQF(QuantifiedVariables& variables, unsigned level = 0, bool negated = false) const;

            /**
             * Transforms this formula to conjunctive normal form (CNF).
             * @param _keepConstraints A flag indicating whether to keep the constraints as they are, or to
             *                          resolve constraints p!=0 to (or p<0 p>0) and to resolve negations in
             *                          front of constraints, e.g., (not p<0) gets p>=0.
             * @param _simplifyConstraintCombinations A flag, which is true, if simplifications shall be applied to conjunctions
             *                                        disjunctions of constraints.
             * @param _tseitinWithEquivalence A flag, which is true, if variables, which are introduced by the tseitin encoding
             *                                are set to be equivalent to the formula they represent. Otherwise, they imply the formula,
             *                                which is also valid, as the current formula context is in NNF.
             */
            Formula toCNF( bool _keepConstraints = true, bool _simplifyConstraintCombinations = false, bool _tseitinWithEquivalence = true ) const;
            
            /**
             * Substitutes all occurrences of the given arithmetic variables in this formula by the given polynomials.
             * @param _arithmeticSubstitutions A substitution-mapping of arithmetic variables to polynomials.
             * @return The resulting formula after substitution.
             */
            Formula substitute( const std::map<carl::Variable, Pol>& _arithmeticSubstitutions ) const
            {
                std::map<carl::Variable, Formula> booleanSubstitutions;
                return substitute( booleanSubstitutions, _arithmeticSubstitutions );
            }
            
            /**
             * Substitutes all occurrences of the given Boolean variables in this formula by the given formulas.
             * @param _booleanSubstitutions A substitution-mapping of Boolean variables to formulas.
             * @return The resulting formula after substitution.
             */
            Formula substitute( const std::map<carl::Variable, Formula>& _booleanSubstitutions ) const
            {
                std::map<carl::Variable, Pol> arithmeticSubstitutions;
                return substitute( _booleanSubstitutions, arithmeticSubstitutions );
            }
            
            /**
             * Substitutes all occurrences of the given Boolean and arithmetic variables in this formula by the given formulas resp. polynomials.
             * @param _booleanSubstitutions A substitution-mapping of Boolean variables to formulas.
             * @param _arithmeticSubstitutions A substitution-mapping of arithmetic variables to polynomials.
             * @return The resulting formula after substitution.
             */
            Formula substitute( const std::map<carl::Variable, Formula>& _booleanSubstitutions, const std::map<carl::Variable,Pol>& _arithmeticSubstitutions ) const;
            
            /// A map from formula pointers to a map of rationals to a pair of a constraint relation and a formula pointer. (internally used)
            typedef FastPointerMap<Pol, std::map<typename Pol::NumberType, std::pair<Relation,Formula>>> ConstraintBounds;
            
            /**
             * Adds the bound to the bounds of the polynomial specified by this constraint. E.g., if the constraint is p+b~0, where p is a sum 
             * of terms, being a rational (actually integer) coefficient times a non-trivial (!=1) monomial( product of variables to the power 
             * of an exponent), b is a rational and ~ is any constraint relation. Furthermore, the leading coefficient of p is 1. Then we add
             * the bound -b to the bounds of p (means that p ~ -b) stored in the given constraint bounds.
             * @param _constraintBounds An object collecting bounds of polynomials.
             * @param _constraint The constraint to find a bound for a polynomial for.
             * @param _inConjunction true, if the constraint is part of a conjunction.
             *                       false, if the constraint is part of a disjunction.
             * @return Formula( FALSE ), if the yet determined bounds imply that the conjunction (_inConjunction == true) or disjunction 
             *                (_inConjunction == false) of which we got the given constraint is invalid resp. valid;
             *         false, the added constraint.
             */
            static Formula addConstraintBound( ConstraintBounds& _constraintBounds, const Formula& _constraint, bool _inConjunction );
            
            /**
             * Stores for every polynomial for which we determined bounds for given constraints a minimal set of constraints
             * representing these bounds into the given set of sub-formulas of a conjunction (_inConjunction == true) or disjunction 
             * (_inConjunction == false) to construct.
             * @param _constraintBounds An object collecting bounds of polynomials.
             * @param _intoAsts A set of sub-formulas of a conjunction (_inConjunction == true) or disjunction (_inConjunction == false) to construct.
             * @param _inConjunction true, if constraints representing the polynomial's bounds are going to be part of a conjunction.
             *                       false, if constraints representing the polynomial's bounds are going to be part of a disjunction.
             * @return true, if the yet added bounds imply that the conjunction (_inConjunction == true) or disjunction 
             *                (_inConjunction == false) to which the bounds are added is invalid resp. valid;
             *         false, otherwise.
             */
            static bool swapConstraintBounds( ConstraintBounds& _constraintBounds, Formulas<Pol>& _intoAsts, bool _inConjunction );
    };
    
    /**
     * Prints the given formula on the given stream.
     * @param _out The stream to print the given formula on.
     * @param _formula The formula to print.
     * @return The stream after printing the given formula on it.
     */
    template<typename Poly>
    std::ostream& operator<<( std::ostream& _out, const Formula<Poly>& _formula );

}    // namespace carl

namespace std
{
    /**
     * Implements std::hash for formula contents.
     */
    template<typename Pol>
    struct hash<carl::FormulaContent<Pol>>
    {
    public:
        /**
         * @param _formulaContent The formula content to get the hash for.
         * @return The hash of the given formula content.
         */
        size_t operator()( const carl::FormulaContent<Pol>& _formulaContent ) const 
        {
            return _formulaContent.hash();
        }
    };
    
    /**
     * Implements std::hash for formulas.
     */
    template<typename Pol>
    struct hash<carl::Formula<Pol>>
    {
    public:
        /**
         * @param _formula The formula to get the hash for.
         * @return The hash of the given formula.
         */
        size_t operator()( const carl::Formula<Pol>& _formula ) const 
        {
            return _formula.getHash();
        }
    };
}    // namespace std

#include "Formula.tpp"
