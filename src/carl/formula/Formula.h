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
#include "Constraint.h"
#include "uninterpreted/UEquality.h"
#include "uninterpreted/UFManager.h"
#include "bitvector/BVConstraintPool.h"
#include "bitvector/BVConstraint.h"
#include "pseudoboolean/PBConstraint.h"
#include "VariableAssignment.h"
#include "VariableComparison.h"

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
     * The formula class representing a SMT formula.
     */
    template<typename Pol>
    class Formula
    {
        friend class FormulaPool<Pol>;
        friend class FormulaContent<Pol>;
        
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
            #define COLLECT_VARIABLES_LOCK_GUARD std::lock_guard<std::mutex> lock3( mpContent->mCollectVariablesMutex );
            #else
            #define ACTIVITY_LOCK_GUARD
            #define DIFFICULTY_LOCK_GUARD
            #define COLLECT_VARIABLES_LOCK_GUARD
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
			
			explicit Formula( const PBConstraint<Pol>& _constraint ):
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
            
            explicit Formula( const UEquality::Arg& _lhs, const UEquality::Arg& _rhs, bool _negated ):
                Formula( FormulaPool<Pol>::getInstance().create( _lhs, _rhs, _negated ) )
            {}
            
            explicit Formula( UEquality&& _eq ):
                Formula( FormulaPool<Pol>::getInstance().create( std::move( _eq ) ) )
            {}
            
            explicit Formula( const UEquality& _eq ):
                Formula( FormulaPool<Pol>::getInstance().create( std::move( UEquality( _eq ) ) ) )
            {}
				
			explicit Formula( PBConstraint<Pol>&& _pbc ):
                Formula( FormulaPool<Pol>::getInstance().create( std::move( _pbc ) ) )
            {}
            
            Formula( const Formula& _formula ):
                mpContent( _formula.mpContent )
            {
                if( _formula.mpContent != nullptr )
                    FormulaPool<Pol>::getInstance().reg( _formula.mpContent );
            }
            
            Formula( Formula&& _formula ):
                mpContent( _formula.mpContent )
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
            
            bool isTseitinClause() const
            {
                return mpContent->mTseitinClause;
            }
            
            const Variables& variables() const
            {
                COLLECT_VARIABLES_LOCK_GUARD
                if( mpContent->mpVariables != nullptr )
                {
                    return *(mpContent->mpVariables);
                }
                mpContent->mpVariables = new Variables();
                allVars( *(mpContent->mpVariables) );
                return *(mpContent->mpVariables);
            }

            /**
             * Collects all real valued variables occurring in this formula.
             * @param _realVars The container to collect the real valued variables in.
             */
            void realValuedVars( Variables& _realVars ) const
            {
                if( propertyHolds( PROP_CONTAINS_REAL_VALUED_VARS ) )
                    collectVariables( _realVars, false, true, false, false, false );
            }
            
            /**
             * Collects all integer valued variables occurring in this formula.
             * @param _intVars The container to collect the integer valued variables in.
             */
            void integerValuedVars( Variables& _intVars ) const
            {
                if( propertyHolds( PROP_CONTAINS_INTEGER_VALUED_VARS ) )
                    collectVariables( _intVars, false, false, true, false, false );
            }
            
            /**
             * Collects all arithmetic variables occurring in this formula.
             * @param _arithmeticVars The container to collect the arithmetic variables in.
             */
            void arithmeticVars( Variables& _arithmeticVars ) const
            {
                if( propertyHolds( PROP_CONTAINS_REAL_VALUED_VARS ) || propertyHolds( PROP_CONTAINS_INTEGER_VALUED_VARS ) )
                    collectVariables( _arithmeticVars, false, true, true, false, false );
            }
            
            /**
             * Collects all boolean variables occurring in this formula.
             * @param _booleanVars The container to collect the boolean variables in.
             */
            void booleanVars( Variables& _booleanVars ) const
            {
                if( propertyHolds( PROP_CONTAINS_BOOLEAN ) )
                    collectVariables( _booleanVars, true, false, false, false, false );
            }
            
            /**
             * Collects all variables occurring in this formula.
             * @param _vars The container to collect the variables in.
             */
            void allVars( Variables& _vars ) const
            {
                collectVariables( _vars, true, true, true, true, true );
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
#ifdef __VS
                return *mpContent->mpSubformulaVS;
#else
				return mpContent->mSubformula;
#endif
            }
            
            /**
             * @return A constant reference to the premise, in case this formula is an implication.
             */
            const Formula& premise() const
            {
                assert( mpContent->mType == IMPLIES );
#ifdef __VS
				return mpContent->mpSubformulasVS->at(0);
#else
				return mpContent->mSubformulas[0];
#endif
            }
            
            /**
             * @return A constant reference to the conclusion, in case this formula is an implication.
             */
            const Formula& conclusion() const
            {
                assert( mpContent->mType == IMPLIES );
#ifdef __VS
                return mpContent->mpSubformulasVS->at(1);
#else
				return mpContent->mSubformulas[1];
#endif
            }
            
            /**
             * @return A constant reference to the condition, in case this formula is an ite-expression of formulas.
             */
            const Formula& condition() const
            {
                assert( mpContent->mType == ITE );
#ifdef __VS
				return mpContent->mpSubformulasVS->at(0);
#else
				return mpContent->mSubformulas[0];
#endif
            }
            
            /**
             * @return A constant reference to the then-case, in case this formula is an ite-expression of formulas.
             */
            const Formula& firstCase() const
            {
                assert( mpContent->mType == ITE );
#ifdef __VS
				return mpContent->mpSubformulasVS->at(1);
#else
				return mpContent->mSubformulas[1];
#endif
            }
            
            /**
             * @return A constant reference to the else-case, in case this formula is an ite-expression of formulas.
             */
            const Formula& secondCase() const
            {
                assert( mpContent->mType == ITE );
#ifdef __VS
				return mpContent->mpSubformulasVS->at(2);
#else
				return mpContent->mSubformulas[2];
#endif
            }

            /**
             * @return A constant reference to the quantifed variables, in case this formula is a quantified formula.
             */
			const std::vector<carl::Variable>& quantifiedVariables() const
			{
				assert( mpContent->mType == FormulaType::EXISTS || mpContent->mType == FormulaType::FORALL );
#ifdef __VS
				return mpContent->mpQuantifierContentVS->mVariables;
#else
				return mpContent->mQuantifierContent.mVariables;
#endif
			}

            /**
             * @return A constant reference to the bound formula, in case this formula is a quantified formula.
             */
			const Formula& quantifiedFormula() const
			{
				assert( mpContent->mType == FormulaType::EXISTS || mpContent->mType == FormulaType::FORALL );
#ifdef __VS
				return mpContent->mpQuantifierContentVS->mFormula;
#else
				return mpContent->mQuantifierContent.mFormula;
#endif
			}

            /**
             * @return A constant reference to the list of sub-formulas of this formula. Note, that
             *          this formula has to be a Boolean combination, if you invoke this method.
             */
            const Formulas<Pol>& subformulas() const
            {
                assert( isNary() );
#ifdef __VS
                return *mpContent->mpSubformulasVS;
#else
				return mpContent->mSubformulas;
#endif
            }

            /**
             * @return A constant reference to the constraint represented by this formula. Note, that
             *          this formula has to be of type CONSTRAINT, if you invoke this method.
             */
            const Constraint<Pol>& constraint() const
            {
                assert( mpContent->mType == FormulaType::CONSTRAINT || mpContent->mType == FormulaType::TRUE || mpContent->mType == FormulaType::FALSE );
#ifdef __VS
		return *mpContent->mpConstraintVS;
#else
		return mpContent->mConstraint;
#endif
            }
			
			const VariableComparison<Pol>& variableComparison() const {
				assert(mpContent->mType == FormulaType::VARCOMPARE);
#ifdef __VS
				return *mpContent->mpVariableComparisonVS;
#else
				return mpContent->mVariableComparison;
#endif
			}
			
			const VariableAssignment<Pol>& variableAssignment() const {
				assert(mpContent->mType == FormulaType::VARASSIGN);
#ifdef __VS
				return *mpContent->mpVariableAssignmentVS;
#else
				return mpContent->mVariableAssignment;
#endif
			}

            const BVConstraint& bvConstraint() const
            {
                assert( mpContent->mType == FormulaType::BITVECTOR );
#ifdef __VS
                return *mpContent->mpBVConstraintVS;
#else
				return mpContent->mBVConstraint;
#endif
            }

            /**
             * @return The name of the Boolean variable represented by this formula. Note, that
             *          this formula has to be of type BOOL, if you invoke this method.
             */
            carl::Variable::Arg boolean() const
            {
                assert( mpContent->mType == FormulaType::BOOL );
#ifdef __VS
                return *mpContent->mpVariableVS;
#else
				return mpContent->mVariable;
#endif
            }
            
            /**
             * @return A constant reference to the uninterpreted equality represented by this formula. Note, that
             *          this formula has to be of type UEQ, if you invoke this method.
             */
            const UEquality& uequality() const
            {
                assert( mpContent->mType == FormulaType::UEQ );
#ifdef __VS
                return *mpContent->mpUIEqualityVS;
#else
				return mpContent->mUIEquality;
#endif
            }
			
			/**
             * @return A constant reference to the pseudoboolean constraint represented by this formula. Note, that
             *          this formula has to be of type PBCONSTRAINT, if you invoke this method.
             */
            const PBConstraint<Pol>& pbConstraint() const
            {
                assert( mpContent->mType == FormulaType::PBCONSTRAINT );
#ifdef __VS
                return *mpContent->mpPBConstraintVS;
#else
				return mpContent->mPBConstraint;
#endif
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
#ifdef __VS
                    return mpContent->mpSubformulasVS->size();
#else
					return mpContent->mSubformulas.size();
#endif
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
#ifdef __VS
                    return mpContent->mpSubformulasVS->empty();
#else
					return mpContent->mSubformulas.empty();
#endif
            }

            /**
             * @return A constant iterator to the beginning of the list of sub-formulas of this formula.
             */
            const_iterator begin() const
            {
                assert( isNary() );
#ifdef __VS
				return mpContent->mpSubformulasVS->begin();
#else
				return mpContent->mSubformulas.begin();
#endif
            }

            /**
             * @return A constant iterator to the end of the list of sub-formulas of this formula.
             */
            const_iterator end() const
            {
                assert( mpContent->mType == FormulaType::AND || mpContent->mType == FormulaType::OR 
                        || mpContent->mType == FormulaType::IFF || mpContent->mType == FormulaType::XOR );
#ifdef __VS
				return mpContent->mpSubformulasVS->end();
#else
				return mpContent->mSubformulas.end();
#endif
            }

            /**
             * @return A constant reverse iterator to the beginning of the list of sub-formulas of this formula.
             */
            const_reverse_iterator rbegin() const
            {
                assert( isNary() );
#ifdef __VS
				return mpContent->mpSubformulasVS->rbegin();
#else
				return mpContent->mSubformulas.rbegin();
#endif
            }

            /**
             * @return A constant reverse iterator to the end of the list of sub-formulas of this formula.
             */
            const_reverse_iterator rend() const
            {
                assert( isNary() );
#ifdef __VS
				return mpContent->mpSubformulasVS->rend();
#else
				return mpContent->mSubformulas.rend();
#endif
            }

            /**
             * @return A reference to the last sub-formula of this formula.
             */
            const Formula& back() const
            {
                assert( isBooleanCombination() );
#ifdef __VS
				if (mpContent->mType == FormulaType::NOT)
					return *mpContent->mpSubformulaVS;
				else
					return *(--(mpContent->mpSubformulasVS->end()));
#else
				if (mpContent->mType == FormulaType::NOT)
                    return mpContent->mSubformula;
				else
					return *(--(mpContent->mSubformulas.end()));
#endif
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
#ifdef __VS
				if (mpContent->mType == FormulaType::CONSTRAINT) return mpContent->mpConstraintVS->isBound();
				if (mpContent->mType == FormulaType::NOT) {
					if (mpContent->mType != FormulaType::CONSTRAINT) return false;
					return mpContent->mpConstraintVS->relation() != Relation::EQ;
				}
#else
				if (mpContent->mType == FormulaType::CONSTRAINT) return mpContent->mConstraint.isBound();
				if (mpContent->mType == FormulaType::NOT) {
					if (mpContent->mSubformula.mpContent->mType != FormulaType::CONSTRAINT) return false;
					return mpContent->mSubformula.mpContent->mConstraint.isBound(true);
				}
#endif

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
                    && !(carl::PROP_CONTAINS_REAL_VALUED_VARS <= properties());
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
#ifdef __VS
                    return (*mpContent->mpSubformulaVS) == _formula;
				else
					return std::find(mpContent->mpSubformulasVS->begin(), mpContent->mpSubformulasVS->end(), _formula) != mpContent->mpSubformulasVS->end();
#else
					return mpContent->mSubformula == _formula;
				else
					return std::find(mpContent->mSubformulas.begin(), mpContent->mSubformulas.end(), _formula) != mpContent->mSubformulas.end();
#endif
            }
            
            /**
             * Collects all constraint occurring in this formula.
             * @param _constraints The container to insert the constraint into.
             */
            void getConstraints( std::vector<Constraint<Pol>>& _constraints ) const
            {
#ifdef __VS
				if (mpContent->mType == FormulaType::CONSTRAINT)
					_constraints.push_back(*mpContent->mpConstraintVS);
                else if( mpContent->mType == FormulaType::NOT )
                    mpContent->mpSubformulaVS->getConstraints( _constraints );
                else if( isNary() )
                {
                    for( const_iterator subAst = mpContent->mpSubformulasVS->begin(); subAst != mpContent->mpSubformulasVS->end(); ++subAst )
                        subAst->getConstraints( _constraints );
                }
#else
                if (mpContent->mType == FormulaType::CONSTRAINT)
                    _constraints.push_back(mpContent->mConstraint);
                else if (mpContent->mType == FormulaType::NOT)
                    mpContent->mSubformula.getConstraints(_constraints);
                else if (isNary())
                {
                    for (const_iterator subAst = mpContent->mSubformulas.begin(); subAst != mpContent->mSubformulas.end(); ++subAst)
                        subAst->getConstraints(_constraints);
                }
#endif
            }
            
            /**
             * Collects all constraint occurring in this formula.
             * @param _constraints The container to insert the constraint into.
             */
            void getConstraints( std::vector<Formula>& _constraints ) const
            {
                if( mpContent->mType == FormulaType::CONSTRAINT )
                    _constraints.push_back( *this );
#ifdef __VS
                else if( mpContent->mType == FormulaType::NOT )
                    mpContent->mpSubformulaVS->getConstraints( _constraints );
                else if( isNary() )
                {
                    for( const_iterator subAst = mpContent->mpSubformulasVS->begin(); subAst != mpContent->mpSubformulasVS->end(); ++subAst )
                        subAst->getConstraints( _constraints );
                }
#else
				else if (mpContent->mType == FormulaType::NOT)
					mpContent->mSubformula.getConstraints(_constraints);
				else if (isNary())
				{
					for (const_iterator subAst = mpContent->mSubformulas.begin(); subAst != mpContent->mSubformulas.end(); ++subAst)
						subAst->getConstraints(_constraints);
				}
#endif
            }

            /**
             * Collects all Boolean variables occurring in this formula.
             * @param _vars The container to collect the Boolean variables in.
			 * @param _type
			 * @param _ofThisType
             */
            void collectVariables( Variables& _vars, bool _booleanVars, bool _realVars, bool _integerVars, bool _uninterpretedVars, bool _bitvectorVars ) const;
            void collectVariables_( Variables& _vars, std::set<BVVariable>* _bvVars, std::set<UVariable>* _ueVars, bool _booleanVars, bool _realVars, bool _integerVars, bool _uninterpretedVars, bool _bitvectorVars ) const;
            
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
            std::string toString( bool _withActivity = false, unsigned _resolveUnequal = 0, const std::string _init = "", bool _oneline = true, bool _infix = false, bool _friendlyNames = true, bool _withVariableDefinition = false ) const;
            
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
    
    /**
     * Prints the given formula on the given stream.
     * @param _out The stream to print the given formula on.
     * @param _formula The formula to print.
     * @return The stream after printing the given formula on it.
     */
    template<typename Poly>
    std::ostream& operator<<( std::ostream& _out, const Formula<Poly>& _formula );

	/**
	 * This class provides a generic visitor for the above Formula class.
	 */
	template<typename Formula>
	struct FormulaVisitor {
		/**
		 * Recursively calls func on every subformula.
		 * @param formula Formula to visit.
		 * @param func Function to call.
		 */
		void visit(const Formula& formula, const std::function<void(Formula)>& func);
		/**
		 * Recursively calls func on every subformula and return a new formula.
		 * On every call of func, the passed formula is replaced by the result.
		 * @param formula Formula to visit.
		 * @param func Function to call.
		 * @return New formula.
		 */
		Formula visitResult(const Formula& formula, const std::function<Formula(Formula)>& func);
	};
    
    template<typename Formula>
    struct FormulaSubstitutor {
    private:
        FormulaVisitor<Formula> visitor;
		
		struct Substitutor {
			const std::map<Formula,Formula>& replacements;
            Substitutor(const std::map<Formula,Formula>& repl): replacements(repl) {}
            Formula operator()(const Formula& formula) {
				auto it = replacements.find(formula);
				if (it == replacements.end()) return formula;
				return it->second;
            }
		};
        
        struct PolynomialSubstitutor {
            const std::map<Variable,typename Formula::PolynomialType>& replacements;
            PolynomialSubstitutor(const std::map<Variable,typename Formula::PolynomialType>& repl): replacements(repl) {}
            Formula operator()(const Formula& formula) {
                if (formula.getType() != FormulaType::CONSTRAINT) return formula;
                return Formula(formula.constraint().lhs().substitute(replacements), formula.constraint().relation());
            }
        };
        
        struct BitvectorSubstitutor {
            const std::map<BVVariable,BVTerm>& replacements;
            BitvectorSubstitutor(const std::map<BVVariable,BVTerm>& repl): replacements(repl) {}
            Formula operator()(const Formula& formula) {
                if (formula.getType() != FormulaType::BITVECTOR) return formula;
                BVTerm lhs = formula.bvConstraint().lhs().substitute(replacements);
                BVTerm rhs = formula.bvConstraint().rhs().substitute(replacements);
                return Formula(BVConstraint::create(formula.bvConstraint().relation(), lhs, rhs));
            }
        };
        
        struct UninterpretedSubstitutor {
            const std::map<UVariable,UFInstance>& replacements;
            UninterpretedSubstitutor(const std::map<UVariable,UFInstance>& repl): replacements(repl) {}
            Formula operator()(const Formula& formula) {
                if (formula.getType() != FormulaType::UEQ) return formula;
                
            }
        };
    public:
        template<typename Source, typename Target>
        Formula substitute(const Formula& formula, const Source& source, const Target& target) {
            std::map<Source,Target> tmp;
            tmp.emplace(source, target);
            return substitute(formula, tmp);
        }
        
		Formula substitute(const Formula& formula, const std::map<Formula,Formula>& replacements);
        Formula substitute(const Formula& formula, const std::map<Variable,typename Formula::PolynomialType>& replacements);
        Formula substitute(const Formula& formula, const std::map<BVVariable,BVTerm>& replacements);
        Formula substitute(const Formula& formula, const std::map<UVariable,UFInstance>& replacements);
    };

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
        size_t operator()( const carl::Formula<Pol>& _formula ) const 
        {
            return _formula.getHash();
        }
    };
}    // namespace std

#include "Formula.tpp"
