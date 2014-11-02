/**
 * @file Ast.h
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
    /// The possible types of a ast.
    enum Type { AND, OR, NOT, IFF, XOR, IMPLIES, ITE, BOOL, CONSTRAINT, TRUE, FALSE, EXISTS, FORALL, UEQ };
    
    // Forward declaration
    template<typename Pol>
    class AstPool;
    
    /**
     * The ast class.
     */
    template<typename Pol>
    class Ast
    {
        friend class AstPool<Pol>;
        
        public:
            /// A constant iterator to a sub-ast of a ast.
            typedef typename PointerSet<Ast>::const_iterator const_iterator;
            /// A constant reverse iterator to a sub-ast of a ast.
            typedef typename PointerSet<Ast>::const_reverse_iterator const_reverse_iterator;
            
        private:
            
            /**
             * Stores the sub-asts of a ast being an implication.
             */
            struct IMPLIESContent
            {
                /// The premise of the implication.
                const Ast* mpPremise;
                /// The conclusion of the implication.
                const Ast* mpConlusion;
                
                /**
                 * Constructs the content of a ast being an implication.
                 * @param _premise The premise of the implication.
                 * @param _conclusion The conclusion of the implication.
                 */
                IMPLIESContent( const Ast* _premise, const Ast* _conclusion): 
                    mpPremise( _premise ), mpConlusion( _conclusion ) {}
            };
            
            /**
             * Stores the sub-asts of a asts being an if-then-else expression of asts.
             */
            struct ITEContent
            {
                /// The condition of the if-then-else expression.
                const Ast* mpCondition;
                /// The then-case of the if-then-else expression.
                const Ast* mpThen;
                /// The else-case of if-then-else expression.
                const Ast* mpElse;
                
                /**
                 * Constructs the content of a ast being an implication.
                 * @param _condition The condition of the if-then-else expression.
                 * @param _then The then-case of the if-then-else expression.
                 * @param _else The else-case of if-then-else expression.
                 */
                ITEContent( const Ast* _condition, const Ast* _then, const Ast* _else ): 
                    mpCondition( _condition ), mpThen( _then ), mpElse( _else ) {}
            };

            /**
             * Stores the variables and the ast bound by a quantifier.
             */
			struct QuantifierContent
			{
                /// The quantified variables.
				std::vector<carl::Variable> mVariables;
                /// The ast bound by this quantifier.
				const Ast* mpAst;
                
                /**
                 * Constructs the content of a quantified ast.
                 * @param _vars The quantified variables.
                 * @param _ast The ast bound by this quantifier.
                 */
				QuantifierContent( const std::vector<carl::Variable>&& _vars, const Ast* _ast ):
					mVariables( _vars ), 
                    mpAst( _ast )
                {}
                
                /**
                 * Checks this content of a quantified ast and the given content of a quantified ast is equal.
                 * @param _qc The content of a quantified ast to check for equality.
                 * @return true, if this content of a quantified ast and the given content of a quantified ast is equal.
                 */
				bool operator==( const QuantifierContent& _qc )
                {
					return (this->mVariables == _qc.mVariables) && (this->mpAst == _qc.mpAst);
				}
			};

            // Members.

            /// The deduction flag, which indicates, that this ast g is a direct sub-ast of
            /// a conjunction of asts (and g f_1 .. f_n), and, that (implies (and f_1 .. f_n) g) holds.
            mutable bool mDeducted;
            /// The hash value.
            size_t mHash;
            /// The unique id.
            size_t mId;
            /// The activity for this ast, which means, how much is this ast involved in the solving procedure.
            mutable double mActivity;
            /// Some value stating an expected difficulty of solving this ast for satisfiability.
            mutable double mDifficulty;
            /// The type of this ast.
            Type mType;
            /// The content of this ast.
            union
            {
                /// The only sub-ast, in case this ast is an negation.
                const Ast* mpSubast;
                /// The premise and conclusion, in case this ast is an implication.
                IMPLIESContent* mpImpliesContent;
                /// The condition, then- and else-case, in case this ast is an ite-expression of asts.
                ITEContent* mpIteContent;
                /// The quantifed variables and the bound ast, in case this ast is a quantified ast.
				QuantifierContent* mpQuantifierContent;
                /// The subasts, in case this ast is a n-nary operation as AND, OR, IFF or XOR.
                PointerSet<Ast>* mpSubasts;
                /// The constraint, in case this asts wraps a constraint.
                const Constraint<Pol>* mpConstraint;
                /// The Boolean variable, in case this ast wraps a Boolean variable.
                carl::Variable mBoolean;
                /// The uninterpreted equality, in case this ast wraps an uninterpreted equality.
                UEquality mUIEquality;
            };
            /// The propositions of this ast.
            Condition mProperties;
            
            /**
             * Constructs the ast (true), if the given bool is true and the ast (false) otherwise.
             * @param _true Specifies whether to create the ast (true) or (false).
             * @param _id A unique id of the ast to create.
             */
            Ast( bool _true, size_t _id = 0 );
            
            /**
             * Constructs a ast being a Boolean variable.
             * @param _booleanVarName The pointer to the string representing the name of the Boolean variable.
             * @param _id A unique id of the ast to create.
             */
            Ast( carl::Variable::Arg _boolean );
            
            /**
             * Constructs a ast being a constraint.
             * @param _constraint The pointer to the constraint.
             */
            Ast( const Constraint<Pol>* _constraint );
            
            /**
             * Constructs a ast being an uninterpreted equality.
             * @param _ueq The pointer to the constraint.
             */
            Ast( UEquality&& _ueq );
            
            /**
             * Constructs the negation of the given ast: (not _subast)
             * @param _subast The sub-ast, which is negated by the constructed ast.
             */
            Ast( const Ast* _subast );
            
            /**
             * Constructs an implication from the first argument to the second: (=> _subastA _subastB)
             * @param _premise The premise of the ast to create.
             * @param _conclusion The conclusion of the ast to create.
             */
            Ast( const Ast* _premise, const Ast* _conclusion );
            
            /**
             * Constructs an if-then-else (ITE) expression: (ite _condition _then _else)
             * @param _condition The condition of the ITE expression to create.
             * @param _then The first case of the ITE expression to create.
             * @param _else The second case of the ITE expression to create.
             */
            Ast( const Ast* _condition, const Ast* _then, const Ast* _else );

			/**
			 * Constructs a quantifier expression: (exists (vars) term) or (forall (vars) term)
			 * @param _type The type of the quantifier to construct.
			 * @param _vars The variables that are bound.
			 * @param _term The term in which the variables are bound.
			 */
			Ast(const Type _type, const std::vector<carl::Variable>&& _vars, const Ast* _term);
            
            /**
             * Constructs the ast of the given type. 
             * @param _type The type of the ast to construct.
             * @param _subasts The sub-asts of the ast to construct.
             */
            Ast( const Type _type, PointerSet<Ast>&& _subasts );
            
            Ast(); // Disabled
            Ast( const Ast& ); // Disabled
            
        public:

            /**
             * Destructor.
             */
            ~Ast();

            // Methods.

            /**
             * Sets the deduction flag to the given value..
             * @param _deducted The value to set the deduction flag to.
             */
            void setDeducted( bool _deducted ) const
            {
                mDeducted = _deducted;
            }

            /**
             * @return The deduction flag, which indicates, that this ast g is a direct sub-ast of
             *          a conjunction of asts (and g f_1 .. f_n), and, that (implies (and f_1 .. f_n) g) holds.
             */
            bool deducted() const
            {
                return mDeducted;
            }

            /**
             * @return Some value stating an expected difficulty of solving this ast for satisfiability.
             */
            const double& difficulty() const
            {
                return mDifficulty;
            }

            /**
             * Sets the difficulty to the given value.
             * @param difficulty The value to set the difficulty to.
             */
            void setDifficulty( double difficulty ) const
            {
                mDifficulty = difficulty;
            }

            /**
             * @return The activity for this ast, which means, how much is this ast involved in the solving procedure.
             */
            double activity() const
            {
                return mActivity;
            }

            /**
             * Sets the activity to the given value.
             * @param _activity The value to set the activity to.
             */
            void setActivity( double _activity ) const
            {
                mActivity = _activity;
            }

            /**
             * @return The type of this ast.
             */
            Type getType() const
            {
                return mType;
            }
            
            /**
             * @return A hash value for this ast.
             */
            size_t getHash() const
            {
                return mHash;
            }
            
            /**
             * @return The unique id for this ast.
             */
            size_t getId() const
            {
                return mId;
            }

            /**
             * @return The bit-vector representing the propositions of this ast. For further information see the Condition class.
             */
            const Condition& properties() const
            {
                return mProperties;
            }

            /**
             * Collects all real valued variables occurring in this ast.
             * @param _realVars The container to collect the real valued variables in.
             */
            void realValuedVars( Variables& _realVars ) const
            {
                collectVariables( _realVars, carl::VariableType::VT_REAL );
            }
            
            /**
             * Collects all integer valued variables occurring in this ast.
             * @param _intVars The container to collect the integer valued variables in.
             */
            void integerValuedVars( Variables& _intVars ) const
            {
                collectVariables( _intVars, carl::VariableType::VT_INT );
            }
            
            /**
             * Collects all arithmetic variables occurring in this ast.
             * @param _arithmeticVars The container to collect the arithmetic variables in.
             */
            void arithmeticVars( Variables& _arithmeticVars ) const
            {
                collectVariables( _arithmeticVars, carl::VariableType::VT_BOOL, false );
            }
            
            /**
             * Collects all arithmetic variables occurring in this ast.
             * @param _arithmeticVars The container to collect the arithmetic variables in.
             */
            void booleanVars( Variables& _booleanVars ) const
            {
                collectVariables( _booleanVars, carl::VariableType::VT_BOOL );
            }
            
            /**
             * @return A pointer to the only sub-ast, in case this ast is an negation.
             */
            const Ast* pSubast() const
            {
                assert( mType == NOT );
                return mpSubast;
            }
            
            /**
             * @return A constant reference to the only sub-ast, in case this ast is an negation.
             */
            const Ast& subast() const
            {
                assert( mType == NOT );
                return *mpSubast;
            }
            
            /**
             * @return A pointer to the premise, in case this ast is an implication.
             */
            const Ast* pPremise() const
            {
                assert( mType == IMPLIES );
                return mpImpliesContent->mpPremise;
            }
            
            /**
             * @return A constant reference to the premise, in case this ast is an implication.
             */
            const Ast& premise() const
            {
                assert( mType == IMPLIES );
                return *mpImpliesContent->mpPremise;
            }
            
            /**
             * @return A pointer to the conclusion, in case this ast is an implication.
             */
            const Ast* pConclusion() const
            {
                assert( mType == IMPLIES );
                return mpImpliesContent->mpConlusion;
            }
            
            /**
             * @return A constant reference to the conclusion, in case this ast is an implication.
             */
            const Ast& conclusion() const
            {
                assert( mType == IMPLIES );
                return *mpImpliesContent->mpConlusion;
            }
            
            /**
             * @return A pointer to the condition, in case this ast is an ite-expression of asts.
             */
            const Ast* pCondition() const
            {
                assert( mType == ITE );
                return mpIteContent->mpCondition;
            }
            
            /**
             * @return A constant reference to the condition, in case this ast is an ite-expression of asts.
             */
            const Ast& condition() const
            {
                assert( mType == ITE );
                return *mpIteContent->mpCondition;
            }
            
            /**
             * @return A pointer to the then-case, in case this ast is an ite-expression of asts.
             */
            const Ast* pFirstCase() const
            {
                assert( mType == ITE );
                return mpIteContent->mpThen;
            }
            
            /**
             * @return A constant reference to the then-case, in case this ast is an ite-expression of asts.
             */
            const Ast& firstCase() const
            {
                assert( mType == ITE );
                return *mpIteContent->mpThen;
            }
            
            /**
             * @return A pointer to the else-case, in case this ast is an ite-expression of asts.
             */
            const Ast* pSecondCase() const
            {
                assert( mType == ITE );
                return mpIteContent->mpElse;
            }
            
            /**
             * @return A constant reference to the else-case, in case this ast is an ite-expression of asts.
             */
            const Ast& secondCase() const
            {
                assert( mType == ITE );
                return *mpIteContent->mpElse;
            }

            /**
             * @return A constant reference to the quantifed variables, in case this ast is a quantified ast.
             */
			const std::vector<carl::Variable>& quantifiedVariables() const
			{
				assert( mType == Type::EXISTS || mType == Type::FORALL );
				return mpQuantifierContent->mVariables;
			}

            /**
             * @return A pointer to the bound ast, in case this ast is a quantified ast.
             */
			const Ast* pQuantifiedAst() const
			{
				assert( mType == Type::EXISTS || mType == Type::FORALL );
				return mpQuantifierContent->mpAst;
			}

            /**
             * @return A constant reference to the bound ast, in case this ast is a quantified ast.
             */
			const Ast& quantifiedAst() const
			{
				assert( mType == Type::EXISTS || mType == Type::FORALL );
				return *mpQuantifierContent->mpAst;
			}

            /**
             * @return A constant reference to the list of sub-asts of this ast. Note, that
             *          this ast has to be a Boolean combination, if you invoke this method.
             */
            const PointerSet<Ast>& subasts() const
            {
                assert( isNary() );
                return *mpSubasts;
            }

            /**
             * @return A pointer to the constraint represented by this ast. Note, that
             *          this ast has to be of type CONSTRAINT, if you invoke this method.
             */
            const Constraint<Pol>* pConstraint() const
            {
                assert( mType == CONSTRAINT || mType == TRUE || mType == FALSE );
                return mpConstraint;
            }

            /**
             * @return A constant reference to the constraint represented by this ast. Note, that
             *          this ast has to be of type CONSTRAINT, if you invoke this method.
             */
            const Constraint<Pol>& constraint() const
            {
                assert( mType == CONSTRAINT || mType == TRUE || mType == FALSE );
                return *mpConstraint;
            }

            /**
             * @return The name of the Boolean variable represented by this ast. Note, that
             *          this ast has to be of type BOOL, if you invoke this method.
             */
            carl::Variable::Arg boolean() const
            {
                assert( mType == BOOL );
                return mBoolean;
            }
            
            /**
             * @return A constant reference to the uninterpreted equality represented by this ast. Note, that
             *          this ast has to be of type UEQ, if you invoke this method.
             */
            const UEquality& uequality() const
            {
                assert( mType == UEQ );
                return mUIEquality;
            }

            /**
             * @return The number of sub-asts of this ast.
             */
            size_t size() const
            {
                if( mType == BOOL || mType == CONSTRAINT || mType == TRUE || mType == FALSE || mType == NOT )
                    return 1;
                else if( mType == IMPLIES )
                    return 2;
                else
                    return mpSubasts->size();
            }

            /**
             * @return true, if this ast has sub-asts;
             *          false, otherwise.
             */
            bool empty() const
            {
                if( mType == BOOL || mType == CONSTRAINT || mType == TRUE || mType == FALSE )
                    return false;
                else
                    return mpSubasts->empty();
            }

            /**
             * @return A constant iterator to the beginning of the list of sub-asts of this ast.
             */
            const_iterator begin() const
            {
                assert( isNary() );
                return mpSubasts->begin();
            }

            /**
             * @return A constant iterator to the end of the list of sub-asts of this ast.
             */
            const_iterator end() const
            {
                assert( mType == AND || mType == OR || mType == IFF || mType == XOR );
                return mpSubasts->end();
            }

            /**
             * @return A constant reverse iterator to the beginning of the list of sub-asts of this ast.
             */
            const_reverse_iterator rbegin() const
            {
                assert( isNary() );
                return mpSubasts->rbegin();
            }

            /**
             * @return A constant reverse iterator to the end of the list of sub-asts of this ast.
             */
            const_reverse_iterator rend() const
            {
                assert( isNary() );
                return mpSubasts->rend();
            }

            /**
             * @return A pointer to the last sub-ast of this ast.
             */
            const Ast* back() const
            {
                assert( isBooleanCombination() );
                if( mType == NOT )
                    return mpSubast;
                else if( mType == IMPLIES )
                    return mpImpliesContent->mpConlusion;
                else if( mType == ITE )
                    return mpIteContent->mpElse;
                else
                    return *(--mpSubasts->end());
            }

            /**
             * @return A reference to the last sub-ast of this ast.
             */
            const Ast& rBack() const
            {
                assert( isBooleanCombination() );
                if( mType == NOT )
                    return *mpSubast;
                else if( mType == IMPLIES )
                    return *mpImpliesContent->mpConlusion;
                else if( mType == ITE )
                    return *mpIteContent->mpElse;
                else
                    return **(mpSubasts->end());
            }
            
            /**
             * Checks if the given property holds for this ast. (Very cheap operation which only relies on bit checks)
             * @param _property The property to check this ast for.
             * @return true, if the given property holds for this ast;
             *         false, otherwise.
             */
            bool propertyHolds( const Condition& _property ) const
            {
                return (mProperties | ~_property) == ~PROP_TRUE;
            }

            /**
             * @return true, if this ast is a Boolean atom.
             */
            bool isAtom() const
            {
                return (mType == CONSTRAINT || mType == BOOL || mType == FALSE || mType == TRUE);
            }

            /**
             * @return true, if the outermost operator of this ast is Boolean;
             *          false, otherwise.
             */
            bool isBooleanCombination() const
            {
                return !isAtom();
            }

            /**
             * @return true, if the type of this asts allows n-ary combinations of sub-asts, for an arbitrary n.
             */
            bool isNary() const
            {
                return (mType == AND || mType == OR || mType == IFF || mType == XOR);
            }
            
            /**
             * @return true, if this ast is a conjunction of constraints;
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
             * @return true, if this ast is a conjunction of real constraints;
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
             * @return true, if this ast is a conjunction of integer constraints;
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
             * @param _ast The pointer to the ast for which to check whether it points to a sub-ast
             *                  of this ast.
             * @return true, the given pointer to a ast points to a sub-ast of this ast;
             *          false, otherwise.
             */
            bool contains( const Ast* const _ast ) const
            {
                if( isAtom() )
                    return false;
                if( mType == NOT )
                    return mpSubast == _ast;
                else if( mType == IMPLIES )
                    return (mpImpliesContent->mpPremise == _ast || mpImpliesContent->mpConlusion == _ast);
                else if( mType == ITE )
                    return (mpIteContent->mpCondition == _ast || mpIteContent->mpThen == _ast || mpIteContent->mpElse == _ast);
                else
                    return mpSubasts->find( _ast ) != mpSubasts->end();
            }
            
            /**
             * Collects all constraint occurring in this ast.
             * @param _constraints The container to insert the constraint into.
             */
            void getConstraints( std::vector<const Constraint<Pol>*>& _constraints ) const
            {
                if( mType == CONSTRAINT )
                    _constraints.push_back( mpConstraint );
                else if( mType == AND || mType == OR || mType == NOT || mType == IFF || mType == XOR || mType == IMPLIES )
                    for( const_iterator subAst = mpSubasts->begin(); subAst != mpSubasts->end(); ++subAst )
                        (*subAst)->getConstraints( _constraints );
            }

            /**
             * Collects all Boolean variables occurring in this ast.
             * @param _booleanVars The container to collect the Boolean variables in.
             */
            void collectVariables( Variables& _vars, carl::VariableType _type, bool _ofThisType = true ) const;
            
            /**
             * @param _ast The ast to compare with.
             * @return true, if this ast and the given ast have the same id;
             *         false, otherwise.
             */
            bool operator==( const Ast& _ast ) const;
            
            /**
             * @param _ast The ast to compare with.
             * @return true, if the id of this ast is smaller than the id of the given one.
             */
            bool operator<( const Ast& _ast ) const
            {
                assert( mId != 0 );
                assert( _ast.getId() != 0 );
                return mId < _ast.getId();
            }
            
            /**
             * @param _assignment The assignment for which to check whether this ast is satisfied by it.
             * @return 0, if this ast is violated by the given assignment;
             *         1, if this ast is satisfied by the given assignment;
             *         2, otherwise.
             */
            unsigned satisfiedBy( const EvaluationMap<typename Pol::NumberType>& _assignment ) const;
            
        private:

            /**
             * Gets the propositions of this ast. It updates and stores the propositions
             * if they are not up to date, hence this method is quite efficient.
             */
            void initProperties();
            
            /**
             * @return 
             */
            void initHash();

            /**
             * Adds the propositions of the given constraint to the propositions of this ast.
             * @param _constraint The constraint to add propositions for.
             */
            void addConstraintProperties( const Constraint<Pol>& _constraint );
            
        public:
            
            /**
             * Gives the string representation of this ast.
             * @param _withActivity A flag which indicates whether to add the ast's activity to the result.
             * @param _resolveUnequal A switch which indicates how to represent the relation symbol for unequal. 
             *                         (for further description see documentation of Constraint::toString( .. ))
             * @param _init The initial string of every row of the result.
             * @param _oneline A flag indicating whether the ast shall be printed on one line.
             * @param _infix A flag indicating whether to print the ast in infix or prefix notation.
             * @param _friendlyNames A flag that indicates whether to print the variables with their internal representation (false)
             *                        or with their dedicated names.
             * @return The resulting string representation of this ast.
             */
            std::string toString( bool _withActivity = false, unsigned _resolveUnequal = 0, const std::string _init = "", bool _oneline = true, bool _infix = false, bool _friendlyNames = true ) const; 
            
            /**
             * The output operator of a ast.
             * @param _out The stream to print on.
             * @param _init
             */
            friend std::ostream& operator<<( std::ostream& _out, const Ast& _ast );
            
            /**
             * Prints the propositions of this ast.
             * @param _out The stream to print on.
             * @param _init The string to print initially in every row.
             */
            void printProposition( std::ostream& _out = std::cout, const std::string _init = "" ) const;
            
            /**
             * @param _withVariables A flag indicating whether the variables shall be displayed before the ast.
             * @return A string which represents this ast in the input format of the tool Redlog.
             */
            std::string toRedlogFormat( bool _withVariables = true ) const;
            
            /**
             * Gets a string, which represents all variables occurring in this ast in a row, separated by the given separator.
             * @param _separator The separator between the variables.
             * @param _variableIds Maps variable names to names, which shall be used instead in the result.
             * @return The string, which represents all variables occurring in this ast in a row, separated by the given separator.
             */
            std::string variableListToString( std::string _separator = ",", const std::unordered_map<std::string, std::string>& _variableIds = (std::unordered_map<std::string, std::string>())) const;
            
            /**
             * @param _type The ast type to get the string representation for.
             * @return The string representation of the given type.
             */
            static std::string AstTypeToString( Type _type );
            
            /**
             * Resolves the outermost negation of this ast.
             * @param _keepConstraints A flag indicating whether to change constraints in order 
             * to resolve the negation in front of them, or to keep the constraints and leave 
             * the negation.
             */
            const Ast* resolveNegation( bool _keepConstraints = true ) const;
            
            /**
             * [Auxiliary method]
             * @return The ast combining the first to the second last sub-ast of this ast by the 
             *         same operator as the one of this ast.
             *         Example: this = (op a1 a2 .. an) -> return = (op a1 .. an-1)
             *                  If n = 2, return = a1
             */
            const Ast* connectPrecedingSubasts() const;

			/**
			 * Transforms this ast to its quantifier free equivalent.
			 * The quantifiers are represented by the parameter variables. Each entry in variables contains all variables between two quantifier alternations.
			 * The even entries (starting with 0) are quantified existentially, the odd entries are quantified universally.
			 * @param variables Contains the quantified variables.
			 * @param level Used for internal recursion.
			 * @param negated Used for internal recursion.
			 * @return The quantifier-free version of this ast.
			 */
			const Ast* toQF(QuantifiedVariables& variables, unsigned level = 0, bool negated = false) const;

            /**
             * Transforms this ast to conjunctive normal form (CNF).
             * @param _keepConstraints A flag indicating whether to keep the constraints as they are, or to
             *                          resolve constraints p!=0 to (or p<0 p>0) and to resolve negations in
             *                          front of constraints, e.g., (not p<0) gets p>=0.
             */
            const Ast* toCNF( bool _keepConstraints = true, bool _simplifyConstraintCombinations = false ) const;
            
            /**
             * Substitutes all occurrences of the given arithmetic variables in this ast by the given polynomials.
             * @param _arithmeticSubstitutions A substitution-mapping of arithmetic variables to polynomials.
             * @return The resulting ast after substitution.
             */
            const Ast* substitute( const std::map<carl::Variable, Pol>& _arithmeticSubstitutions ) const
            {
                std::map<carl::Variable, const Ast*> booleanSubstitutions;
                return substitute( booleanSubstitutions, _arithmeticSubstitutions );
            }
            
            /**
             * Substitutes all occurrences of the given Boolean variables in this ast by the given asts.
             * @param _booleanSubstitutions A substitution-mapping of Boolean variables to asts.
             * @return The resulting ast after substitution.
             */
            const Ast* substitute( const std::map<carl::Variable, const Ast*>& _booleanSubstitutions ) const
            {
                std::map<carl::Variable, Pol> arithmeticSubstitutions;
                return substitute( _booleanSubstitutions, arithmeticSubstitutions );
            }
            
            /**
             * Substitutes all occurrences of the given Boolean and arithmetic variables in this ast by the given asts resp. polynomials.
             * @param _booleanSubstitutions A substitution-mapping of Boolean variables to asts.
             * @param _arithmeticSubstitutions A substitution-mapping of arithmetic variables to polynomials.
             * @return The resulting ast after substitution.
             */
            const Ast* substitute( const std::map<carl::Variable, const Ast*>& _booleanSubstitutions, const std::map<carl::Variable,Pol>& _arithmeticSubstitutions ) const;
            
            /// A map from ast pointers to a map of rationals to a pair of a constraint relation and a ast pointer. (internally used)
            typedef FastPointerMap<Pol, std::map<typename Pol::NumberType, std::pair<Relation, const Ast*>>> ConstraintBounds;
            
            /**
             * Adds the bound to the bounds of the polynomial specified by this constraint. E.g., if the constraint is p+b~0, where p is a sum 
             * of terms, being a rational (actually integer) coefficient times a non-trivial (!=1) monomial( product of variables to the power 
             * of an exponent), b is a rational and ~ is any constraint relation. Furthermore, the leading coefficient of p is 1. Then we add
             * the bound -b to the bounds of p (means that p ~ -b) stored in the given constraint bounds.
             * @param _constraintBounds An object collecting bounds of polynomials.
             * @param _constraint The constraint to find a bound for a polynomial for.
             * @param _inConjunction true, if the constraint is part of a conjunction.
             *                       false, if the constraint is part of a disjunction.
             * @return true, if the yet determined bounds imply that the conjunction (_inConjunction == true) or disjunction 
             *                (_inConjunction == false) of which we got the given constraint is invalid resp. valid;
             *         false, otherwise.
             */
            static const Ast* addConstraintBound( ConstraintBounds& _constraintBounds, const Ast* _constraint, bool _inConjunction );
            
            /**
             * Stores for every polynomial for which we determined bounds for given constraints a minimal set of constraints
             * representing these bounds into the given set of sub-asts of a conjunction (_inConjunction == true) or disjunction 
             * (_inConjunction == false) to construct.
             * @param _constraintBounds An object collecting bounds of polynomials.
             * @param _intoAsts A set of sub-asts of a conjunction (_inConjunction == true) or disjunction (_inConjunction == false) to construct.
             * @param _inConjunction true, if constraints representing the polynomial's bounds are going to be part of a conjunction.
             *                       false, if constraints representing the polynomial's bounds are going to be part of a disjunction.
             * @return true, if the yet added bounds imply that the conjunction (_inConjunction == true) or disjunction 
             *                (_inConjunction == false) to which the bounds are added is invalid resp. valid;
             *         false, otherwise.
             */
            static bool swapConstraintBounds( ConstraintBounds& _constraintBounds, PointerSet<Ast>& _intoAsts, bool _inConjunction );
    };
    
    /**
     * Prints the given ast on the given stream.
     * @param _out The stream to print the given ast on.
     * @param _ast The ast to print.
     * @return The stream after printing the given ast on it.
     */
    template<typename Poly>
    std::ostream& operator<<( std::ostream& _out, const Ast<Poly>& _ast );
}    // namespace carl

namespace std
{
    /**
     * Implements std::hash for asts.
     */
    template<typename Pol>
    struct hash<carl::Ast<Pol>>
    {
    public:
        /**
         * @param _ast The ast to get the hash for.
         * @return The hash of the given ast.
         */
        size_t operator()( const carl::Ast<Pol>& _ast ) const 
        {
            return _ast.getHash();
        }
    };
}    // namespace std

#include "Ast.tpp"
