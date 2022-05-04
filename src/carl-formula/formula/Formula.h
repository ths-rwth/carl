/**
 * @file Formula.h
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 * @author Ulrich Loup
 * @author Sebastian Junges
 * @since 2012-02-09
 * @version 2014-10-30
 */

#pragma once

#include <cstring>
#include <functional>
#include <string>
#include <set>
#include <boost/dynamic_bitset.hpp>
#include "Condition.h"
#include "../arithmetic/Constraint.h"
#include "../uninterpreted/UEquality.h"
#include "../uninterpreted/UFManager.h"
#include "../bitvector/BVConstraintPool.h"
#include "../bitvector/BVConstraint.h"
#include "../arithmetic/VariableAssignment.h"
#include "../arithmetic/VariableComparison.h"
#include "Logic.h"

#include "FormulaContent.h"

namespace carl
{
    // Forward definition.
    template<typename Pol>
    class Formula;

    // Forward declaration
    template<typename Pol>
    class FormulaPool;

    /**
     * Represent an SMT formula, which can be an atom for some background
     * theory or a boolean combination of (sub)formulas.
     */
    template<typename Pol>
    class Formula
    {
        friend class FormulaPool<Pol>;
        friend class FormulaContent<Pol>;
		template<typename P>
		friend std::ostream& operator<<(std::ostream& os, const Formula<P>& f);

        public:
            /// A constant iterator to a sub-formula of a formula.
            using const_iterator = typename Formulas<Pol>::const_iterator;
            /// A constant reverse iterator to a sub-formula of a formula.
            using const_reverse_iterator = typename Formulas<Pol>::const_reverse_iterator;
			/// A typedef for the template argument.
			using PolynomialType = Pol;

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


            explicit Formula( const FormulaContent<Pol>* _content ):
                mpContent( _content )
            {
                if( _content != nullptr )
                    FormulaPool<Pol>::getInstance().reg( _content );
            }

            #ifdef THREAD_SAFE
            #define ACTIVITY_LOCK_GUARD std::lock_guard<std::mutex> lock1( mpContent->mActivityMutex );
            #define DIFFICULTY_LOCK_GUARD std::lock_guard<std::mutex> lock2( mpContent->mDifficultyMutex );
            #define VARIABLES_LOCK_GUARD std::lock_guard<std::mutex> lock3( mpContent->mVariablesMutex );
            #else
            #define ACTIVITY_LOCK_GUARD
            #define DIFFICULTY_LOCK_GUARD
            #define VARIABLES_LOCK_GUARD
            #endif

        public:

            /**
             * Gets the propositions of this formula. It updates and stores the propositions
             * if they are not up to date, hence this method is quite efficient.
             */
            static void init( FormulaContent<Pol>& _content );

            explicit Formula( FormulaType _type = FALSE ):
                Formula( FormulaPool<Pol>::getInstance().create( _type ) )
            {}

            explicit Formula( Variable::Arg _booleanVar ):
                Formula( FormulaPool<Pol>::getInstance().create( _booleanVar ) )
            {}

            explicit Formula( const Pol& _pol, Relation _rel ):
                Formula( FormulaPool<Pol>::getInstance().create( Constraint<Pol>( _pol, _rel ) ) )
            {}

            explicit Formula( const Constraint<Pol>& _constraint ):
                Formula( FormulaPool<Pol>::getInstance().create( _constraint ) )
            {}

			explicit Formula( const VariableComparison<Pol>& _variableComparison ):
				Formula( FormulaPool<Pol>::getInstance().create( _variableComparison ) )
			{}

			explicit Formula( const VariableAssignment<Pol>& _variableAssignment ):
				Formula( FormulaPool<Pol>::getInstance().create( _variableAssignment ) )
			{}

            explicit Formula( const BVConstraint& _constraint ):
                Formula( FormulaPool<Pol>::getInstance().create( _constraint ) )
            {}

            explicit Formula( FormulaType _type, Formula&& _subformula ):
                Formula(FormulaPool<Pol>::getInstance().create(_type, std::move(_subformula)))
            {}

            explicit Formula( FormulaType _type, const Formula& _subformula ):
                Formula(FormulaPool<Pol>::getInstance().create(_type, std::move(Formula(_subformula))))
            {}

            explicit Formula( FormulaType _type, const Formula& _subformulaA, const Formula& _subformulaB ):
                Formula( FormulaPool<Pol>::getInstance().create( _type, {_subformulaA, _subformulaB} ))
            {
                assert( _type == FormulaType::AND || _type == FormulaType::IFF || _type == FormulaType::IMPLIES || _type == FormulaType::OR || _type == FormulaType::XOR );
            }

            explicit Formula( FormulaType _type, const Formula& _subformulaA, const Formula& _subformulaB, const Formula& _subformulaC):
                Formula( FormulaPool<Pol>::getInstance().create(_type, {_subformulaA, _subformulaB, _subformulaC}))
            {}

            explicit Formula( FormulaType _type, const FormulasMulti<Pol>& _subformulas ):
                Formula( FormulaPool<Pol>::getInstance().create( _subformulas ) )
            {
                assert( _type == FormulaType::XOR );
            }

            explicit Formula( FormulaType _type, const Formulas<Pol>& _subasts ):
                Formula( FormulaPool<Pol>::getInstance().create( _type, _subasts ) )
            {}

            explicit Formula( FormulaType _type, Formulas<Pol>&& _subasts ):
                Formula( FormulaPool<Pol>::getInstance().create( _type, std::move(_subasts) ) )
            {}

            explicit Formula( FormulaType _type, const std::initializer_list<Formula<Pol>>& _subasts ):
                Formula( FormulaPool<Pol>::getInstance().create( _type, std::move(Formulas<Pol>(_subasts.begin(), _subasts.end()) ) ))
            {}

            explicit Formula( FormulaType _type, const FormulaSet<Pol>& _subasts ):
                Formula( FormulaPool<Pol>::getInstance().create( _type, std::move(Formulas<Pol>(_subasts.begin(), _subasts.end()) ) ))
            {}

            // TODO: Does the following constructor anything more efficient than the one before?
            explicit Formula( FormulaType _type, FormulaSet<Pol>&& _subasts ):
                Formula( FormulaPool<Pol>::getInstance().create( _type, std::move(Formulas<Pol>(_subasts.begin(), _subasts.end()) ) ))
            {}

            explicit Formula( FormulaType _type, std::vector<Variable>&& _vars, const Formula& _term ):
                Formula( FormulaPool<Pol>::getInstance().create( _type, std::move( _vars ), _term ) )
            {}

            explicit Formula( FormulaType _type, const std::vector<Variable>& _vars, const Formula& _term ):
                Formula( _type, std::move( std::vector<Variable>( _vars ) ), _term )
            {}

            explicit Formula( const UTerm& _lhs, const UTerm& _rhs, bool _negated ):
                Formula( FormulaPool<Pol>::getInstance().create( _lhs, _rhs, _negated ) )
            {}

            explicit Formula( UEquality&& _eq ):
                Formula( FormulaPool<Pol>::getInstance().create( std::move( _eq ) ) )
            {}

            explicit Formula( const UEquality& _eq ):
                Formula( FormulaPool<Pol>::getInstance().create( std::move( UEquality( _eq ) ) ) )
            {}

            Formula( const Formula& _formula ):
                mpContent( _formula.mpContent )
            {
                if( _formula.mpContent != nullptr )
                    FormulaPool<Pol>::getInstance().reg( _formula.mpContent );
            }

            Formula(Formula&& _formula) noexcept:
                mpContent(_formula.mpContent)
            {
                _formula.mpContent = nullptr;
            }

            ~Formula()
            {
                if( mpContent != nullptr )
                {
                    FormulaPool<Pol>::getInstance().free( mpContent );
                }
            }

            Formula& operator=( const Formula& _formula )
            {
                if( _formula.mpContent != nullptr )
                    FormulaPool<Pol>::getInstance().reg( _formula.mpContent );
                if( mpContent != nullptr )
                    FormulaPool<Pol>::getInstance().free( mpContent );
                mpContent = _formula.mpContent;
                return *this;
            }

            Formula& operator=( Formula&& _formula )
            {
                if( mpContent != nullptr )
                    FormulaPool<Pol>::getInstance().free( mpContent );
                mpContent = _formula.mpContent;
                _formula.mpContent = nullptr;
                return *this;
            }

            // Methods.

            /**
             * @return The activity for this formula, which means, how much is this formula involved in the solving procedure.
             */
            double activity() const
            {
                ACTIVITY_LOCK_GUARD
                return mpContent->mActivity;
            }

            /**
             * Sets the activity to the given value.
             * @param _activity The value to set the activity to.
             */
            void setActivity( double _activity ) const
            {
                ACTIVITY_LOCK_GUARD
                mpContent->mActivity = _activity;
            }

            /**
             * @return Some value stating an expected difficulty of solving this formula for satisfiability.
             */
            double difficulty() const
            {
                DIFFICULTY_LOCK_GUARD
                return mpContent->mDifficulty;
            }

            /**
             * Sets the difficulty to the given value.
             * @param difficulty The value to set the difficulty to.
             */
            void setDifficulty( double difficulty ) const
            {
                DIFFICULTY_LOCK_GUARD
                mpContent->mDifficulty = difficulty;
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
            std::size_t getHash() const
            {
                return mpContent->mHash;
            }

            /**
             * @return The unique id for this formula.
             */
            std::size_t getId() const
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

            const Variables& variables() const
            {
                VARIABLES_LOCK_GUARD
                if( mpContent->mpVariables != nullptr )
                {
                    return *(mpContent->mpVariables);
                }
                carlVariables vars;
                gatherVariables(vars);
                mpContent->mpVariables = new Variables(vars.begin(), vars.end());
                return *(mpContent->mpVariables);
            }

            Formula negated() const
            {
                return Formula( mpContent->mNegation );
            }
			Formula baseFormula() const
			{
				return Formula(FormulaPool<Pol>::getInstance().getBaseFormula(mpContent));
			}

            const Formula& removeNegations() const
            {
                if( getType() == FormulaType::NOT )
                    return subformula().removeNegations();
                return *this;
            }

            /**
             * @return A constant reference to the only sub-formula, in case this formula is an negation.
             */
            const Formula& subformula() const
            {
                assert( mpContent->mType == NOT );
				return std::get<Formula<Pol>>(mpContent->mContent);
            }

            /**
             * @return A constant reference to the premise, in case this formula is an implication.
             */
            const Formula& premise() const
            {
                assert( mpContent->mType == IMPLIES );
                return std::get<Formulas<Pol>>(mpContent->mContent)[0];
            }

            /**
             * @return A constant reference to the conclusion, in case this formula is an implication.
             */
            const Formula& conclusion() const
            {
                assert( mpContent->mType == IMPLIES );
                return std::get<Formulas<Pol>>(mpContent->mContent)[1];
            }

            /**
             * @return A constant reference to the condition, in case this formula is an ite-expression of formulas.
             */
            const Formula& condition() const
            {
                assert( mpContent->mType == ITE );
                return std::get<Formulas<Pol>>(mpContent->mContent)[0];
            }

            /**
             * @return A constant reference to the then-case, in case this formula is an ite-expression of formulas.
             */
            const Formula& firstCase() const
            {
                assert( mpContent->mType == ITE );
                return std::get<Formulas<Pol>>(mpContent->mContent)[1];
            }

            /**
             * @return A constant reference to the else-case, in case this formula is an ite-expression of formulas.
             */
            const Formula& secondCase() const
            {
                assert( mpContent->mType == ITE );
                return std::get<Formulas<Pol>>(mpContent->mContent)[2];
            }

            /**
             * @return A constant reference to the quantifed variables, in case this formula is a quantified formula.
             */
			const std::vector<carl::Variable>& quantifiedVariables() const
			{
				assert( mpContent->mType == FormulaType::EXISTS || mpContent->mType == FormulaType::FORALL );
				return std::get<QuantifierContent<Pol>>(mpContent->mContent).mVariables;
			}

            /**
             * @return A constant reference to the bound formula, in case this formula is a quantified formula.
             */
			const Formula& quantifiedFormula() const
			{
				assert( mpContent->mType == FormulaType::EXISTS || mpContent->mType == FormulaType::FORALL );
				return std::get<QuantifierContent<Pol>>(mpContent->mContent).mFormula;
			}

            /**
             * @return A constant reference to the list of sub-formulas of this formula. Note, that
             *          this formula has to be a Boolean combination, if you invoke this method.
             */
            const Formulas<Pol>& subformulas() const
            {
                assert( isNary() );
                return std::get<Formulas<Pol>>(mpContent->mContent);
            }

            /**
             * @return A constant reference to the constraint represented by this formula. Note, that
             *          this formula has to be of type CONSTRAINT, if you invoke this method.
             */
            const Constraint<Pol>& constraint() const
            {
                assert( mpContent->mType == FormulaType::CONSTRAINT || mpContent->mType == FormulaType::TRUE || mpContent->mType == FormulaType::FALSE );
                return std::get<Constraint<Pol>>(mpContent->mContent);
            }

			const VariableComparison<Pol>& variableComparison() const {
				assert(mpContent->mType == FormulaType::VARCOMPARE);
                return std::get<VariableComparison<Pol>>(mpContent->mContent);
			}

			const VariableAssignment<Pol>& variableAssignment() const {
				assert(mpContent->mType == FormulaType::VARASSIGN);
                return std::get<VariableAssignment<Pol>>(mpContent->mContent);
			}

            const BVConstraint& bvConstraint() const
            {
                assert( mpContent->mType == FormulaType::BITVECTOR );
                return std::get<BVConstraint>(mpContent->mContent);
            }

            /**
             * @return The name of the Boolean variable represented by this formula. Note, that
             *          this formula has to be of type BOOL, if you invoke this method.
             */
            carl::Variable::Arg boolean() const
            {
                assert( mpContent->mType == FormulaType::BOOL );
                return std::get<carl::Variable>(mpContent->mContent);
            }

            /**
             * @return A constant reference to the uninterpreted equality represented by this formula. Note, that
             *          this formula has to be of type UEQ, if you invoke this method.
             */
            const UEquality& uequality() const
            {
                assert( mpContent->mType == FormulaType::UEQ );
                return std::get<UEquality>(mpContent->mContent);
            }

            /**
             * @return The number of sub-formulas of this formula.
             */
            size_t size() const
            {
                if( mpContent->mType == FormulaType::BOOL || mpContent->mType == FormulaType::CONSTRAINT || mpContent->mType == FormulaType::TRUE
                        || mpContent->mType == FormulaType::FALSE || mpContent->mType == FormulaType::NOT || mpContent->mType == FormulaType::UEQ
                        || mpContent->mType == FormulaType::BITVECTOR )
                    return 1;
                else
                    return std::get<Formulas<Pol>>(mpContent->mContent).size();
            }

            /**
             * @return true, if this formula has sub-formulas;
             *          false, otherwise.
             */
            bool empty() const
            {
                if( mpContent->mType == FormulaType::BOOL || mpContent->mType == FormulaType::CONSTRAINT
                        || mpContent->mType == FormulaType::TRUE || mpContent->mType == FormulaType::FALSE
                        || mpContent->mType == FormulaType::BITVECTOR )
                    return false;
                else
					return std::get<Formulas<Pol>>(mpContent->mContent).empty();
            }

            /**
             * @return A constant iterator to the beginning of the list of sub-formulas of this formula.
             */
            const_iterator begin() const
            {
                assert( isNary() );
				return std::get<Formulas<Pol>>(mpContent->mContent).begin();
            }

            /**
             * @return A constant iterator to the end of the list of sub-formulas of this formula.
             */
            const_iterator end() const
            {
                assert( mpContent->mType == FormulaType::AND || mpContent->mType == FormulaType::OR
                        || mpContent->mType == FormulaType::IFF || mpContent->mType == FormulaType::XOR );
                return std::get<Formulas<Pol>>(mpContent->mContent).end();
            }

            /**
             * @return A constant reverse iterator to the beginning of the list of sub-formulas of this formula.
             */
            const_reverse_iterator rbegin() const
            {
                assert( isNary() );
                return std::get<Formulas<Pol>>(mpContent->mContent).rbegin();
            }

            /**
             * @return A constant reverse iterator to the end of the list of sub-formulas of this formula.
             */
            const_reverse_iterator rend() const
            {
                assert( isNary() );
                return std::get<Formulas<Pol>>(mpContent->mContent).rend();
            }

            /**
             * @return A reference to the last sub-formula of this formula.
             */
            const Formula& back() const
            {
                assert( isBooleanCombination() );
				if (mpContent->mType == FormulaType::NOT)
                    return std::get<Formula<Pol>>(mpContent->mContent);
				else
					return *(--(std::get<Formulas<Pol>>(mpContent->mContent).end()));
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
						|| mpContent->mType == FormulaType::VARCOMPARE || mpContent->mType == FormulaType::VARASSIGN
                        || mpContent->mType == FormulaType::UEQ || mpContent->mType == FormulaType::BITVECTOR
                        || mpContent->mType == FormulaType::FALSE || mpContent->mType == FormulaType::TRUE);
            }

            bool isLiteral() const
            {
                return propertyHolds( PROP_IS_A_LITERAL );
            }

            /**
             * @return true, if the outermost operator of this formula is Boolean;
             *          false, otherwise.
             */
            bool isBooleanCombination() const
            {
                return !isAtom();
            }

			bool isBound() const
			{
				if (mpContent->mType == FormulaType::CONSTRAINT) return is_bound(std::get<Constraint<Pol>>(mpContent->mContent));
				if (mpContent->mType == FormulaType::NOT) {
					if (std::get<Formula<Pol>>(mpContent->mContent).mpContent->mType != FormulaType::CONSTRAINT) return false;
					return is_bound(std::get<Constraint<Pol>>(std::get<Formula<Pol>>(mpContent->mContent).mpContent->mContent), true);
				}
				return false;
			}

            /**
             * @return true, if the type of this formulas allows n-ary combinations of sub-formulas, for an arbitrary n.
             */
            bool isNary() const
            {
                return mpContent->isNary();
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
             * @return true, if this formula is propositional;
             *         false, otherwise.
             */
            bool isOnlyPropositional() const
            {
                return !(carl::PROP_CONTAINS_BITVECTOR <= properties())
                    && !(carl::PROP_CONTAINS_UNINTERPRETED_EQUATIONS <= properties())
                    && !(carl::PROP_CONTAINS_INTEGER_VALUED_VARS <= properties())
                    && !(carl::PROP_CONTAINS_REAL_VALUED_VARS <= properties())
                    && !(carl::PROP_CONTAINS_PSEUDOBOOLEAN <= properties());
            }

            Logic logic() const {
                if (carl::PROP_CONTAINS_BITVECTOR <= properties()
                && !(carl::PROP_CONTAINS_UNINTERPRETED_EQUATIONS <= properties())
                && !(carl::PROP_CONTAINS_INTEGER_VALUED_VARS <= properties())
                && !(carl::PROP_CONTAINS_REAL_VALUED_VARS <= properties())
                && !(carl::PROP_CONTAINS_PSEUDOBOOLEAN <= properties())) {
                    return Logic::QF_BV;
                } else if (!(carl::PROP_CONTAINS_BITVECTOR <= properties())
                && (carl::PROP_CONTAINS_UNINTERPRETED_EQUATIONS <= properties())
                && !(carl::PROP_CONTAINS_INTEGER_VALUED_VARS <= properties())
                && !(carl::PROP_CONTAINS_REAL_VALUED_VARS <= properties())
                && !(carl::PROP_CONTAINS_PSEUDOBOOLEAN <= properties())) {
                    return Logic::QF_UF;
                } else if (!(carl::PROP_CONTAINS_BITVECTOR <= properties())
                && !(carl::PROP_CONTAINS_UNINTERPRETED_EQUATIONS <= properties())
                && !(carl::PROP_CONTAINS_INTEGER_VALUED_VARS <= properties())
                && !(carl::PROP_CONTAINS_REAL_VALUED_VARS <= properties())
                && (carl::PROP_CONTAINS_PSEUDOBOOLEAN <= properties())) {
                    return Logic::QF_PB;
                } else if (!(carl::PROP_CONTAINS_BITVECTOR <= properties())
                && !(carl::PROP_CONTAINS_UNINTERPRETED_EQUATIONS <= properties())
                && (carl::PROP_CONTAINS_INTEGER_VALUED_VARS <= properties())
                && !(carl::PROP_CONTAINS_REAL_VALUED_VARS <= properties())
                && !(carl::PROP_CONTAINS_PSEUDOBOOLEAN <= properties())) {
                    if (carl::PROP_CONTAINS_NONLINEAR_POLYNOMIAL <= properties()) {
                        return Logic::QF_NIA;
                    } else {
                        return Logic::QF_LIA;
                    }
                } else if (!(carl::PROP_CONTAINS_BITVECTOR <= properties())
                && !(carl::PROP_CONTAINS_UNINTERPRETED_EQUATIONS <= properties())
                && !(carl::PROP_CONTAINS_INTEGER_VALUED_VARS <= properties())
                && (carl::PROP_CONTAINS_REAL_VALUED_VARS <= properties())
                && !(carl::PROP_CONTAINS_PSEUDOBOOLEAN <= properties())) {
                    if (carl::PROP_CONTAINS_NONLINEAR_POLYNOMIAL <= properties()) {
                        return Logic::QF_NRA;
                    } else {
                        return Logic::QF_LRA;
                    }
                } else if (!(carl::PROP_CONTAINS_BITVECTOR <= properties())
                && !(carl::PROP_CONTAINS_UNINTERPRETED_EQUATIONS <= properties())
                && (carl::PROP_CONTAINS_INTEGER_VALUED_VARS <= properties())
                && (carl::PROP_CONTAINS_REAL_VALUED_VARS <= properties())
                && !(carl::PROP_CONTAINS_PSEUDOBOOLEAN <= properties())) {
                    if (carl::PROP_CONTAINS_NONLINEAR_POLYNOMIAL <= properties()) {
                        return Logic::QF_NIRA;
                    } else {
                        return Logic::QF_LIRA;
                    }
                } else {
                    return Logic::UNDEFINED;
                }
            }

            /**
             * @param _formula The pointer to the formula for which to check whether it points to a sub-formula
             *                  of this formula.
             * @return true, the given pointer to a formula points to a sub-formula of this formula;
             *          false, otherwise.
             */
            bool contains( const Formula& _formula ) const
            {
                if( *this == _formula )
                    return true;
                if( isAtom() )
                    return false;
                if( mpContent->mType == FormulaType::NOT )
					return std::get<Formula<Pol>>(mpContent->mContent) == _formula;
				else
					return std::find(std::get<Formulas<Pol>>(mpContent->mContent).begin(), std::get<Formulas<Pol>>(mpContent->mContent).end(), _formula) != std::get<Formulas<Pol>>(mpContent->mContent).end();
            }

            /**
             * Collects all constraint occurring in this formula.
             * @param _constraints The container to insert the constraint into.
             */
            void getConstraints( std::vector<Constraint<Pol>>& _constraints ) const // TODO rename to gatherConstraints
            {
                if (mpContent->mType == FormulaType::CONSTRAINT)
                    _constraints.push_back(std::get<Constraint<Pol>>(mpContent->mContent));
                else if (mpContent->mType == FormulaType::NOT)
                    std::get<Formula<Pol>>(mpContent->mContent).getConstraints(_constraints);
                else if (isNary())
                {
                    for (const_iterator subAst = std::get<Formulas<Pol>>(mpContent->mContent).begin(); subAst != std::get<Formulas<Pol>>(mpContent->mContent).end(); ++subAst)
                        subAst->getConstraints(_constraints);
                }
            }

            /**
             * Collects all constraint occurring in this formula.
             * @param _constraints The container to insert the constraint into.
             */
            void getConstraints( std::vector<Formula>& _constraints ) const // TODO rename to gatherConstraints
            {
                if( mpContent->mType == FormulaType::CONSTRAINT )
                    _constraints.push_back( *this );
				else if (mpContent->mType == FormulaType::NOT)
					std::get<Formula<Pol>>(mpContent->mContent).getConstraints(_constraints);
				else if (isNary())
				{
					for (const_iterator subAst = std::get<Formulas<Pol>>(mpContent->mContent).begin(); subAst != std::get<Formulas<Pol>>(mpContent->mContent).end(); ++subAst)
						subAst->getConstraints(_constraints);
				}
            }

			void gatherVariables(carlVariables& vars) const;
            void gatherUFs(std::set<UninterpretedFunction>& ufs) const;
            void gatherUVariables(std::set<UVariable>& uvs) const;
            void gatherBVVariables(std::set<BVVariable>& bvvs) const;

            /**
             * @return The formula's complexity, which is mainly the number of operations within this formula.
             */
            size_t complexity() const;

            /**
             * @param _formula The formula to compare with.
             * @return true, if this formula and the given formula are equal;
             *         false, otherwise.
             */
            bool operator==( const Formula& _formula ) const
            {
                return mpContent == _formula.mpContent;
            }

            /**
             * @param _formula The formula to compare with.
             * @return true, if this formula and the given formula are not equal.
             */
            bool operator!=( const Formula& _formula ) const
            {
                return mpContent != _formula.mpContent;
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

			Formula operator!() const {
				return negated();
			}

        private:

            /**
             * @return
             */
            void initHash();

        public:

            /**
             * Prints the propositions of this formula.
             * @param _out The stream to print on.
             * @param _init The string to print initially in every row.
             */
            void printProposition( std::ostream& _out = std::cout, const std::string _init = "" ) const;

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
             * Substitutes all occurrences of the given variable in this formula by the given polynomial.
             * @param _var The variable to substitute.
             * @param _var The polynomial to substitute the variable for.
             * @return The resulting formula after substitution.
             */
            Formula substitute( carl::Variable::Arg _var, const Pol& _pol ) const
            {
                std::map<carl::Variable, Formula> booleanSubstitutions;
                std::map<carl::Variable, Pol> arithmeticSubstitutions;
                arithmeticSubstitutions.emplace( _var, _pol );
                return substitute( booleanSubstitutions, arithmeticSubstitutions );
            }

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
            typedef FastMap<Pol, std::map<typename Pol::NumberType, std::pair<Relation,Formula>>> ConstraintBounds;

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

    template<typename Pol>
    inline void variables(const Formula<Pol>& f, carlVariables& vars) {
		f.gatherVariables(vars);
	}

	/**
	 * The output operator of a formula.
	 * @param os The stream to print on.
	 * @param f The formula to print.
	 */
	template<typename P>
	inline std::ostream& operator<<(std::ostream& os, const Formula<P>& f) {
		return os << *f.mpContent;
	}
}    // namespace carl

#include "functions/FormulaVisitor.h"

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
        std::size_t operator()( const carl::FormulaContent<Pol>& _formulaContent ) const
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
        std::size_t operator()( const carl::Formula<Pol>& _formula ) const
        {
            return _formula.getHash();
        }
    };
}    // namespace std

#include "Formula.tpp"
