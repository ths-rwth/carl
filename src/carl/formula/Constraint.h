/**
 * Constraint.h
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 * @author Sebastian Junges
 * @author Ulrich Loup
 * @since 2010-04-26
 * @version 2014-10-30
 */

#pragma once

//#define NDEBUG
#define CONSTRAINT_WITH_FACTORIZATION

#include "../config.h"
#include "../core/Definiteness.h"
#include "../core/Relation.h"
#include "../core/VariableInformation.h"
#include "../core/VariablesInformation.h"
#include "../interval/Interval.h"
#include "../interval/IntervalEvaluation.h"
#include "../util/Common.h"
#include "config.h"

#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>


namespace carl
{
    // Forward definition.
    template<typename Pol>
    class Constraint;
    
    template<typename Poly>
    using Constraints = std::set<Constraint<Poly>, carl::less<Constraint<Poly>, false>>;
    
    // Forward declaration
    template<typename Pol>
    class ConstraintPool;
    
    template<typename Pol>
    using VarInfo = VariableInformation<true, Pol>;
    
    template<typename Pol>
    using VarInfoMap = std::map<Variable, VarInfo<Pol>>;
    
    template<typename Pol, EnableIf<needs_cache<Pol>> = dummy>
    Pol makePolynomial( typename Pol::PolyType&& _poly );

    template<typename Pol, EnableIf<needs_cache<Pol>> = dummy>
    Pol makePolynomial( carl::Variable::Arg _var );
    
    template<typename Pol, EnableIf<needs_cache<Pol>> = dummy>
    Pol makePolynomial( const typename Pol::PolyType& _poly )
    {
        return makePolynomial<Pol>(typename Pol::PolyType(_poly));
    }

    template<typename Pol, DisableIf<needs_cache<Pol>> = dummy>
    Pol makePolynomial( carl::Variable::Arg _var )
    {
        return Pol( _var );
    }

    template<typename Pol, DisableIf<needs_cache<Pol>> = dummy>
    Pol makePolynomial( const typename Pol::PolyType& _poly )
    {
        return _poly;
    }

    /**
     * Turns around the given relation symbol.
     * @param _rel The relation symbol to invert.
     * @return The resulting inverted relation symbol.
     */
    inline Relation turnAroundRelation( const Relation& _rel )
    {
        switch( _rel )
        {
            case Relation::EQ:
                return Relation::EQ;
            case Relation::NEQ:
                return Relation::NEQ;
            case Relation::LEQ:
                return Relation::GEQ;
            case Relation::GEQ:
                return Relation::LEQ;
            case Relation::LESS:
                return Relation::GREATER;
            case Relation::GREATER:
                return Relation::LESS;
            default:
                assert( false );
                return Relation::EQ;
        }
    }
    
    /**
     * Class to create a constraint object.
     */
    template<typename Pol>
    class ConstraintContent
    {
            friend class Constraint<Pol>;
            friend class ConstraintPool<Pol>;
        
        private:
            /// A unique id.
            std::size_t mID = 0;
            /// A unique id.
            mutable std::size_t mUsages = 0;
            /// The relation symbol comparing the polynomial considered by this constraint to zero.
            Relation mRelation = Relation::EQ;
            /// The polynomial which is compared by this constraint to zero.
            Pol mLhs;
			/// The hash value.
			std::size_t mHash;
            /// The factorization of the polynomial considered by this constraint.
            mutable Factors<Pol> mFactorization;
            /// A container which includes all variables occurring in the polynomial considered by this constraint.
            Variables mVariables;
            /// A map which stores information about properties of the variables in this constraint.
            mutable VarInfoMap<Pol> mVarInfoMap;
            /// Definiteness of the polynomial in this constraint.
            mutable Definiteness mLhsDefinitess = Definiteness::NON;
            /// Mutex for access to variable information map.
            mutable std::mutex mVarInfoMapMutex;
            /// Mutex for access to the factorization.
            mutable std::mutex mFactorizationMutex;

            /**
             * Default constructor. (0=0)
             */
            ConstraintContent();
            
            /**
             * Constructs the constraint content:   _lhs _rel 0
             * @param _var The left-hand side of the constraint to construct being a polynomial.
             * @param _rel The relation symbol.
             * @param _id The unique id for this constraint. It should be maintained by a central instance
             *             as the offered ConstraintPool class, therefore the constructors are private and
             *             can only be invoked using the constraint pool or more precisely using the 
             *             method newConstraint( _lhs, _rel )  or
             *             newBound( x, _rel, b ) if _lhs = x - b and x is a variable
             *             and b is a rational.
             */
            ConstraintContent( carl::Variable::Arg _var, Relation _rel, std::size_t _id = 0 ):
                ConstraintContent<Pol>::ConstraintContent( std::move( makePolynomial<Pol>(_var) ), _rel, _id )
            {}
            
            /**
             * Constructs the constraint content:   _lhs _rel 0
             * @param _lhs The left-hand side of the constraint to construct being a polynomial.
             * @param _rel The relation symbol.
             * @param _id The unique id for this constraint. It should be maintained by a central instance
             *             as the offered ConstraintPool class, therefore the constructors are private and
             *             can only be invoked using the constraint pool or more precisely using the 
             *             method newConstraint( _lhs, _rel )  or
             *             newBound( x, _rel, b ) if _lhs = x - b and x is a variable
             *             and b is a rational.
             */
            ConstraintContent( const Pol& _lhs, Relation _rel, std::size_t _id = 0 ):
                ConstraintContent<Pol>::ConstraintContent( std::move( Pol( _lhs ) ), _rel, _id)
            {}
            
            template<typename P = Pol, EnableIf<needs_cache<P>> = dummy>
            ConstraintContent( const typename Pol::PolyType& _lhs, Relation _rel, std::size_t _id = 0 ):
                ConstraintContent<Pol>::ConstraintContent( std::move( makePolynomial<Pol>( _lhs ) ), _rel, _id )
            {}
            
            template<typename P = Pol, EnableIf<needs_cache<P>> = dummy>
            ConstraintContent( typename Pol::PolyType&& _lhs, Relation _rel, std::size_t _id = 0 ):
                ConstraintContent<Pol>::ConstraintContent( std::move( makePolynomial<Pol>( std::move( _lhs ) ) ), _rel, _id )
            {}
            
            ConstraintContent( Pol&& _lhs, Relation _rel, std::size_t _id = 0 );
            
            /**
             * Initializes some basic information of the constraint, such as the definiteness of the left-hand 
             * side and specific information to each variable.
             */
            void initLazy();
            void initEager();
            
            /**
             * Initializes the stored factorization.
             */
            void initFactorization() const;
            
            void initVariableInformations()
            {
                VariablesInformation<false,Pol> varinfos = mLhs.template getVarInfo<false>();
				for (const auto& vi: varinfos) {
					mVarInfoMap.emplace_hint(mVarInfoMap.end(), vi.first, vi.second);
				}
            }
               
            /**
             * Applies some cheap simplifications to the constraints.
             *
             * @return The simplified constraints, if simplifications could be applied;
             *         The constraint itself, otherwise.
             */
            ConstraintContent* simplify() const;
            
        public:

            /**
             * Destructor.
             */
            ~ConstraintContent() noexcept = default;

            /**
             * @return A hash value for this constraint.
             */
            std::size_t hash() const {
                return mHash;
            }
			
			std::size_t id() const {
				return mID;
			}
			Relation relation() const {
				return mRelation;
			}
			const auto& lhs() const {
				return mLhs;
			}
            
            /**
             * @param _variable The variable for which to determine the maximal degree.
             * @return The maximal degree of the given variable in this constraint content. (Monomial-wise)
             */
            uint maxDegree( const Variable& _variable ) const
            {
                auto varInfo = mVarInfoMap.find(_variable);
                if (varInfo == mVarInfoMap.end()) return 0;
                return varInfo->second.maxDegree();
            }
            
            /**
             * @return The maximal degree of all variables in this constraint. (Monomial-wise)
             */
            uint maxDegree() const
            {
                uint result = 0;
                for (const auto& var: mVariables) {
                    uint deg = maxDegree(var);
                    if (deg > result) result = deg;
                }
                return result;
            }
            
            /**
             * Checks if this constraints contains an integer valued variable.
             * @return true, if it does;
             *          false, otherwise.
             */
            bool hasIntegerValuedVariable() const
            {
                for( auto var = mVariables.begin(); var != mVariables.end(); ++var )
                {
                    if( var->type() == VariableType::VT_INT )
                        return true;
                }
                return false;
            }
            
            /**
             * Checks if this constraints contains an real valued variable.
             * @return true, if it does;
             *          false, otherwise.
             */
            bool hasRealValuedVariable() const
            {
                for( auto var = mVariables.begin(); var != mVariables.end(); ++var )
                {
                    if( var->type() == VariableType::VT_REAL )
                        return true;
                }
                return false;
            }
            
            /**
             * Checks, whether the constraint  represented by this constraint content is consistent.
             * It differs between, containing variables, consistent, and inconsistent.
             * @return 0, if the constraint is not consistent.
             *          1, if the constraint is consistent.
             *          2, if the constraint still contains variables.
             */
            unsigned isConsistent() const;
            
            /**
             * Gives the string representation of this constraint.
             * @param _unequalSwitch A switch to indicate which kind of unequal should be used.
             *         For p != 0 with infix:  0: "p != 0", 1: "p <> 0", 2: "p /= 0"
             *                    with prefix: 0: "(!= p 0)", 1: (or (< p 0) (> p 0)), 2: (not (= p 0))
             * @param _infix An infix string which is printed right before the constraint.
             * @param _friendlyVarNames A flag that indicates whether to print the variables with their internal representation (false)
             *                           or with their dedicated names.
             * @return The string representation of this constraint.
             */
            std::string toString( unsigned _unequalSwitch = 0, bool _infix = true, bool _friendlyVarNames = true ) const;
            
    };
	
	/**
	 * @param lhs Left ConstraintContent
	 * @param rhs Right ConstraintContent
	 * @return `lhs == rhs`
	 */
	template<typename Pol>
	bool operator==(const ConstraintContent<Pol>& lhs, const ConstraintContent<Pol>& rhs) {
		if (lhs.id() == 0 || rhs.id() == 0) {
            return lhs.relation() == rhs.relation() && lhs.lhs() == rhs.lhs();
        }
        return lhs.id() == rhs.id();
	}

    /**
     * Prints the representation of the given constraints on the given stream.
     * @param _out The stream to print on.
     * @param _constraint The constraint to print.
     * @return The given stream after printing.
     */
    template<typename P>
    std::ostream& operator<<( std::ostream& _out, const ConstraintContent<P>& _cc )
    {
        return (_out << _cc.toString());
    }

    /**
     * The constraint class.
     */
    template<typename Pol>
    class Constraint
    {
            friend class ConstraintPool<Pol>;
            
        private:
            
            // Members.

            /// The content of this constraint.
            const ConstraintContent<Pol>* mpContent;
            
            explicit Constraint( const ConstraintContent<Pol>* _content );
            
            #ifdef THREAD_SAFE
            #define VARINFOMAP_LOCK_GUARD std::lock_guard<std::mutex> lock1( mpContent->mVarInfoMapMutex );
            #define FACTORIZATION_LOCK_GUARD std::lock_guard<std::mutex> lock1( mpContent->mFactorizationMutex );
            #define FACTORIZATION_LOCK mpContent->mFactorizationMutex.lock();
            #define FACTORIZATION_UNLOCK mpContent->mFactorizationMutex.unlock();
            #else
            #define VARINFOMAP_LOCK_GUARD
            #define FACTORIZATION_LOCK_GUARD
            #define FACTORIZATION_LOCK
            #define FACTORIZATION_UNLOCK
            #endif
            
        public:
            
            explicit Constraint( bool _valid = true );
            
            explicit Constraint( carl::Variable::Arg _var, Relation _rel, const typename Pol::NumberType& _bound = constant_zero<typename Pol::NumberType>::get() );
            
            explicit Constraint( const Pol& _lhs, Relation _rel );
            
            template<typename P = Pol, EnableIf<needs_cache<P>> = dummy>
            explicit Constraint( const typename P::PolyType& _lhs, Relation _rel );
            
            Constraint( const Constraint& _constraint );
            
            Constraint( Constraint&& _constraint ) noexcept;
            
            ~Constraint();
            
            Constraint& operator=( const Constraint& _constraint );
            
            Constraint& operator=( Constraint&& _constraint ) noexcept;
            
            /**
             * @return The considered polynomial being the left-hand side of this constraint.
             *          Hence, the right-hand side of any constraint is always 0.
             */
            const Pol& lhs() const
            {
                return mpContent->lhs();
            }

            /**
             * @return A container containing all variables occurring in the polynomial of this constraint.
             */
            const Variables& variables() const
            {
                return mpContent->mVariables;
            }

            /**
             * @return The relation symbol of this constraint.
             */
            Relation relation() const
            {
                return mpContent->relation();
            }

            /**
             * @return The unique id of this constraint.
             */
            size_t id() const
            {
                return mpContent->id();
            }

            /**
             * @return A hash value for this constraint.
             */
            size_t getHash() const
            {
                return mpContent->hash();
            }

            /**
             * @return true, if the polynomial p compared by this constraint has a proper factorization (!=p);
             *          false, otherwise.
             */
            bool hasFactorization() const
            {
                FACTORIZATION_LOCK
                if( mpContent->mFactorization.empty() )
                    mpContent->initFactorization();
                FACTORIZATION_UNLOCK
                return (mpContent->mFactorization.size() > 1);
            }

            /**
             * @return The factorization of the polynomial compared by this constraint.
             */
            const Factors<Pol>& factorization() const
            {
                FACTORIZATION_LOCK
                if( mpContent->mFactorization.empty() )
                    mpContent->initFactorization();
                FACTORIZATION_UNLOCK
                return mpContent->mFactorization;
            }

            /**
             * @return The constant part of the polynomial compared by this constraint.
             */
            typename Pol::NumberType constantPart() const
            {
                return mpContent->mLhs.constantPart();
            }
            
            /**
             * @param _variable The variable for which to determine the maximal degree.
             * @return The maximal degree of the given variable in this constraint. (Monomial-wise)
             */
            uint maxDegree( const Variable& _variable ) const
            {   
                VARINFOMAP_LOCK_GUARD
                return mpContent->maxDegree( _variable );
            }

            /**
             * @return The maximal degree of all variables in this constraint. (Monomial-wise)
             */
            uint maxDegree() const
            {
                VARINFOMAP_LOCK_GUARD
                return mpContent->maxDegree();
            }

            /**
             * @param _variable The variable for which to determine the minimal degree.
             * @return The minimal degree of the given variable in this constraint. (Monomial-wise)
             */
            uint minDegree( const Variable& _variable ) const
            {
                VARINFOMAP_LOCK_GUARD
                auto varInfo = mpContent->mVarInfoMap.find( _variable );
                if( varInfo == mpContent->mVarInfoMap.end() ) return 0;
                return varInfo->second.minDegree();
            }
            
            /**
             * @param _variable The variable for which to determine the number of occurrences.
             * @return The number of occurrences of the given variable in this constraint. (In 
             *          how many monomials of the left-hand side does the given variable occur?)
             */
            uint occurences( const Variable& _variable ) const
            {
                VARINFOMAP_LOCK_GUARD
                auto varInfo = mpContent->mVarInfoMap.find( _variable );
                if( varInfo == mpContent->mVarInfoMap.end() ) return 0;
                return varInfo->second.occurence();
            }
            
            /**
             * @param _variable The variable to find variable information for.
			 * @param _withCoefficients
             * @return The whole variable information object.
             * Note, that if the given variable is not in this constraints, this method fails.
             * Furthermore, the variable information returned do provide coefficients only, if
             * the given flag _withCoefficients is set to true.
             */
            const VarInfo<Pol>& varInfo( const Variable& _variable, bool _withCoefficients = false ) const
            {
                VARINFOMAP_LOCK_GUARD
                auto varInfo = mpContent->mVarInfoMap.find( _variable );
                assert( varInfo != mpContent->mVarInfoMap.end() );
                if( _withCoefficients && !varInfo->second.hasCoeff() )
                {
                    varInfo->second = mpContent->mLhs.template getVarInfo<true>( _variable );
                }
                return varInfo->second;
            }
			
			bool relationIsStrict() const {
                return isStrict(mpContent->mRelation);
			}
			bool relationIsWeak() const {
				return isWeak(mpContent->mRelation);
			}
            
            /**
             * Checks if the given variable occurs in the constraint.
             * @param _var  The variable to check for.
             * @return true, if the given variable occurs in the constraint;
             *          false, otherwise.
             */
            bool hasVariable( const Variable& _var ) const
            {
                return mpContent->mVariables.find( _var ) != mpContent->mVariables.end();
            }
            
            /**
             * @return true, if it contains only integer valued variables.
             */
            bool integerValued() const
            {
                for( auto var = mpContent->mVariables.begin(); var != mpContent->mVariables.end(); ++var )
                {
                    if( var->getType() != VariableType::VT_INT )
                        return false;
                }
                return true;
            }
            
            /**
             * @return true, if it contains only real valued variables.
             */
            bool realValued() const
            {
                for( auto var = mpContent->mVariables.begin(); var != mpContent->mVariables.end(); ++var )
                {
                    if( var->getType() != VariableType::VT_REAL )
                        return false;
                }
                return true;
            }
            
            /**
             * Checks if this constraints contains an integer valued variable.
             * @return true, if it does;
             *          false, otherwise.
             */
            bool hasIntegerValuedVariable() const
            {
                return mpContent->hasIntegerValuedVariable();
            }
            
            /**
             * Checks if this constraints contains an real valued variable.
             * @return true, if it does;
             *          false, otherwise.
             */
            bool hasRealValuedVariable() const
            {
                return mpContent->hasRealValuedVariable();
            }
            
            /**
             * @return true, if this constraint is a bound.
             */
            bool isBound(bool negated = false) const
            {
				if (mpContent->mVariables.size() != 1 || maxDegree(*mpContent->mVariables.begin()) != 1) return false;
				if (negated) {
					return mpContent->mRelation != Relation::EQ;
				} else {
					return mpContent->mRelation != Relation::NEQ;
				}
            }
            
            /**
             * @return true, if this constraint is a lower bound.
             */
            bool isLowerBound() const
            {
                if( isBound() )
                {
                    if( mpContent->mRelation == Relation::EQ ) return true;
                    const typename Pol::NumberType& coeff = mpContent->mLhs.lterm().coeff();
                    if( coeff < 0 )
                        return (mpContent->mRelation == Relation::LEQ || mpContent->mRelation == Relation::LESS );
                    else
                    {
                        assert( coeff > 0 );
                        return (mpContent->mRelation == Relation::GEQ || mpContent->mRelation == Relation::GREATER );
                    }
                }
                return false;
            }
            
            /**
             * @return true, if this constraint is an upper bound.
             */
            bool isUpperBound() const
            {
                if( isBound() )
                {
                    if( mpContent->mRelation == Relation::EQ ) return true;
                    const typename Pol::NumberType& coeff = mpContent->mLhs.lterm().coeff();
                    if( coeff > 0 )
                        return (mpContent->mRelation == Relation::LEQ || mpContent->mRelation == Relation::LESS );
                    else
                    {
                        assert( coeff < 0 );
                        return (mpContent->mRelation == Relation::GEQ || mpContent->mRelation == Relation::GREATER );
                    }
                }
                return false;
            }
            
            /**
             * @return An approximation of the complexity of this constraint.
             */
            size_t complexity() const
            {
                return 1 + mpContent->mLhs.complexity();
            }
            
            /**
             * Checks whether the given assignment satisfies this constraint.
             * @param _assignment The assignment.
             * @return 1, if the given assignment satisfies this constraint.
             *          0, if the given assignment contradicts this constraint.
             *          2, otherwise (possibly not defined for all variables in the constraint,
             *                       even then it could be possible to obtain the first two results.)
             */
            unsigned satisfiedBy( const EvaluationMap<typename Pol::NumberType>& _assignment ) const;
            
            /**
             * Checks, whether the constraint is consistent.
             * It differs between, containing variables, consistent, and inconsistent.
             * @return 0, if the constraint is not consistent.
             *          1, if the constraint is consistent.
             *          2, if the constraint still contains variables.
             */
            unsigned isConsistent() const
            {
                return mpContent->isConsistent();
            }
            
            /**
             * Checks whether this constraint is consistent with the given assignment from 
             * the its variables to interval domains.
             * @param _solutionInterval The interval domains of the variables.
             * @return 1, if this constraint is consistent with the given intervals;
             *          0, if this constraint is not consistent with the given intervals;
             *          2, if it cannot be decided whether this constraint is consistent with the given intervals.
             */
            unsigned consistentWith( const EvaluationMap<Interval<double>>& _solutionInterval ) const;
            
            /**
             * Checks whether this constraint is consistent with the given assignment from 
             * the its variables to interval domains.
             * @param _solutionInterval The interval domains of the variables.
             * @param _stricterRelation This relation is set to a relation R such that this constraint and the given variable bounds
             *                           imply the constraint formed by R, comparing this constraint's left-hand side to zero.
             * @return 1, if this constraint is consistent with the given intervals;
             *          0, if this constraint is not consistent with the given intervals;
             *          2, if it cannot be decided whether this constraint is consistent with the given intervals.
             */
            unsigned consistentWith( const EvaluationMap<Interval<double>>& _solutionInterval, Relation& _stricterRelation ) const;

			/**
			 * Checks whether the given interval assignment may fulfill the constraint.
			 * Note that the assignment must be complete.
			 * There are three possible outcomes:
			 * - True (4), i.e. all actual assignments satisfy the constraint: $p ~_\alpha 0 \Leftrightarrow True$
			 * - Maybe (3), i.e. some actual assignments satisfy the constraint: $p ~_\alpha 0 \Leftrightarrow ?$
			 * - Not null (2), i.e. all assignments that make the constraint evaluate not to zero satisfy the constraint: $p ~_\alpha 0 \Leftrightarrow p \neq 0$
			 * - Null (1), i.e. only assignments that make the constraint evaluate to zero satisfy the constraint: $p ~_\alpha 0 \Leftrightarrow p_\alpha = 0$
			 * - False (0), i.e. no actual assignment satisfies the constraint: $p ~_\alpha 0 \Leftrightarrow False$
			 * @param _assignment Variable assignment.
			 * @return 0, 1 or 2.
			 */
			unsigned evaluate(const EvaluationMap<Interval<typename carl::UnderlyingNumberType<Pol>::type>>& _assignment) const;

            /**
             * @param _var The variable to check the size of its solution set for.
             * @return true, if it is easy to decide whether this constraint has a finite solution set
             *                in the given variable;
             *          false, otherwise.
             */
            bool hasFinitelyManySolutionsIn( const Variable& _var ) const;
                        
            /**
             * Calculates the coefficient of the given variable with the given degree. Note, that it only
             * computes the coefficient once and stores the result.
             * @param _var The variable for which to calculate the coefficient.
             * @param _degree The according degree of the variable for which to calculate the coefficient.
             * @return The ith coefficient of the given variable, where i is the given degree.
             */
            Pol coefficient( const Variable& _var, uint _degree ) const;
            
            Constraint negation() const {
                CARL_LOG_DEBUG("carl.formula.constraint", "negation of " << *this << " is " << Constraint(lhs(), carl::inverse(relation())));
                return Constraint(lhs(), carl::inverse(relation()));
            }

            /**
             * If this constraint represents a substitution (equation, where at least one variable occurs only linearly),
             * this method detects a (there could be various possibilities) corresponding substitution variable and term.
             * @param _substitutionVariable Is set to the substitution variable, if this constraint represents a substitution.
             * @param _substitutionTerm Is set to the substitution term, if this constraint represents a substitution.
             * @return true, if this constraints represents a substitution;
             *         false, otherwise.
             */
            bool getSubstitution( Variable& _substitutionVariable, Pol& _substitutionTerm, bool _negated = false, const Variable& _exclude = carl::Variable::NO_VARIABLE ) const;
			
			bool getAssignment(Variable& _substitutionVariable, typename Pol::NumberType& _substitutionValue) const;
            
            /**
             * Gives the string representation of this constraint.
             * @param _unequalSwitch A switch to indicate which kind of unequal should be used.
             *         For p != 0 with infix:  0: "p != 0", 1: "p <> 0", 2: "p /= 0"
             *                    with prefix: 0: "(!= p 0)", 1: (or (< p 0) (> p 0)), 2: (not (= p 0))
             * @param _infix An infix string which is printed right before the constraint.
             * @param _friendlyVarNames A flag that indicates whether to print the variables with their internal representation (false)
             *                           or with their dedicated names.
             * @return The string representation of this constraint.
             */
            std::string toString( unsigned _unequalSwitch = 0, bool _infix = true, bool _friendlyVarNames = true ) const
            {
                return mpContent->toString( _unequalSwitch, _infix, _friendlyVarNames);
            }
            /**
             * Determines whether the constraint is pseudo-boolean.
             *
             * @return True if this constraint is pseudo-boolean. False otherwise.
             */
            bool isPseudoBoolean();

            /**
             * Prints the properties of this constraints on the given stream.
             * @param _out The stream to print on.
             */
            void printProperties( std::ostream& _out = std::cout ) const;
		
		template<typename P>
		friend bool operator==(const Constraint<P>& lhs, const Constraint<P>& rhs);
		template<typename P>
		friend bool operator!=(const Constraint<P>& lhs, const Constraint<P>& rhs);
    };
	
	/**
	 * @param lhs Left constraint
	 * @param rhs Right constraint
	 * @return `lhs == rhs`
	 */
	template<typename P>
	bool operator==(const Constraint<P>& lhs, const Constraint<P>& rhs) {
		return lhs.mpContent == rhs.mpContent;
	}
	
	/**
	 * @param lhs Left constraint
	 * @param rhs Right constraint
	 * @return `lhs != rhs`
	 */
	template<typename P>
	bool operator!=(const Constraint<P>& lhs, const Constraint<P>& rhs) {
		return lhs.mpContent != rhs.mpContent;
	}
	
	/**
	 * @param lhs Left constraint
	 * @param rhs Right constraint
	 * @return `lhs < rhs`
	 */
	template<typename P>
	bool operator<(const Constraint<P>& lhs, const Constraint<P>& rhs) {
		assert(lhs.id() != 0 && rhs.id() != 0);
		return lhs.id() < rhs.id();
	}
	
	/**
	 * @param lhs Left constraint
	 * @param rhs Right constraint
	 * @return `lhs > rhs`
	 */
	 template<typename P>
	 bool operator>(const Constraint<P>& lhs, const Constraint<P>& rhs) {
		assert(lhs.id() != 0 && rhs.id() != 0);
		return lhs.id() > rhs.id();
	 }
	
	 /**
 	 * @param lhs Left constraint
 	 * @param rhs Right constraint
 	 * @return `lhs <= rhs`
 	 */
	 template<typename P>
	 bool operator<=(const Constraint<P>& lhs, const Constraint<P>& rhs) {
		assert(lhs.id() != 0 && rhs.id() != 0);
		return lhs.id() <= rhs.id();
	 }
	
	 /**
 	 * @param lhs Left constraint
 	 * @param rhs Right constraint
 	 * @return `lhs >= rhs`
 	 */
	 template<typename P>
	 bool operator>=(const Constraint<P>& lhs, const Constraint<P>& rhs) {
		assert(lhs.id() != 0 && rhs.id() != 0);
		return lhs.id() >= rhs.id();
	 }

    
    const signed A_IFF_B = 2;
    const signed A_IMPLIES_B = 1;
    const signed B_IMPLIES_A = -1;
    const signed NOT__A_AND_B = -2;
    const signed A_AND_B__IFF_C = -3;
    const signed A_XOR_B = -4;
            
    /**
     * Compares _constraintA with _constraintB.
     * @return  2, if it is easy to decide that _constraintA and _constraintB have the same solutions. _constraintA = _constraintB
     *           1, if it is easy to decide that _constraintB includes all solutions of _constraintA;   _constraintA -> _constraintB
     *          -1, if it is easy to decide that _constraintA includes all solutions of _constraintB;   _constraintB -> _constraintA
     *          -2, if it is easy to decide that _constraintA has no solution common with _constraintB; not(_constraintA and _constraintB)
     *          -3, if it is easy to decide that _constraintA and _constraintB can be intersected;      _constraintA and _constraintB = _constraintC
     *          -4, if it is easy to decide that _constraintA is the inverse of _constraintB;           _constraintA xor _constraintB
     *           0, otherwise.
     */
    template<typename Pol>
    signed compare( const Constraint<Pol>& _constraintA, const Constraint<Pol>& _constraintB )
    {
        /*
         * Check whether it holds that 
         * 
         *                      _constraintA  =  a_1*m_1+...+a_k*m_k + c ~ 0
         * and 
         *                      _constraintB  =  b_1*m_1+...+b_k*m_k + d ~ 0, 
         * 
         * where a_1,..., a_k, b_1,..., b_k, c, d are rational coefficients, 
         *       m_1,..., m_k are non-constant monomials and 
         *       exists a rational g such that 
         * 
         *                   a_i = g * b_i for all 1<=i<=k 
         *              or   b_i = g * a_i for all 1<=i<=k 
         */
        typename Pol::NumberType one_divided_by_a = _constraintA.lhs().coprimeFactorWithoutConstant();
        typename Pol::NumberType one_divided_by_b = _constraintB.lhs().coprimeFactorWithoutConstant();
        typename Pol::NumberType c = _constraintA.lhs().constantPart();
        typename Pol::NumberType d = _constraintB.lhs().constantPart();
        assert( carl::isOne(carl::getNum(carl::abs(one_divided_by_b))) );
        Pol tmpA = (_constraintA.lhs() - c) * one_divided_by_a;
        Pol tmpB = (_constraintB.lhs() - d) * one_divided_by_b;
//        std::cout << "tmpA = " << tmpA << std::endl;
//        std::cout << "tmpB = " << tmpB << std::endl;
        if( tmpA != tmpB ) return 0;
        bool termACoeffGreater = false;
        bool signsDiffer = (one_divided_by_a < carl::constant_zero<typename Pol::NumberType>::get()) != (one_divided_by_b < carl::constant_zero<typename Pol::NumberType>::get());
        typename Pol::NumberType g;
        if( carl::getDenom(one_divided_by_a) > carl::getDenom(one_divided_by_b) )
        {
            g = typename Pol::NumberType(carl::getDenom(one_divided_by_a))/carl::getDenom(one_divided_by_b);
            if( signsDiffer )
                g = -g;
            termACoeffGreater = true;
            d *= g;
        }
        else
        {
            g = typename Pol::NumberType(carl::getDenom(one_divided_by_b))/carl::getDenom(one_divided_by_a);
            if( signsDiffer )
                g = -g;
            c *= g;
        }
        // Apply the multiplication by a negative g to the according relation symbol, which
        // has to be turned around then.
        Relation relA = _constraintA.relation();
        Relation relB = _constraintB.relation();
        if( g < 0 )
        {
            if( termACoeffGreater )
            {
                switch( relB )
                {
                    case Relation::LEQ:
                        relB = Relation::GEQ;
                        break;
                    case Relation::GEQ:
                        relB = Relation::LEQ;
                        break;
                    case Relation::LESS:
                        relB = Relation::GREATER;
                        break;
                    case Relation::GREATER:
                        relB = Relation::LESS;
                        break;
                    default:
                        break;
                }
            }
            else
            {
                switch( relA )
                {
                    case Relation::LEQ:
                        relA = Relation::GEQ;
                        break;
                    case Relation::GEQ:
                        relA = Relation::LEQ;
                        break;
                    case Relation::LESS:
                        relA = Relation::GREATER;
                        break;
                    case Relation::GREATER:
                        relA = Relation::LESS;
                        break;
                    default:
                        break;
                }   
            }
        }
//        std::cout << "c = " << c << std::endl;
//        std::cout << "d = " << d << std::endl;
//        std::cout << "g = " << g << std::endl;
//        std::cout << "relA = " << relA << std::endl;
//        std::cout << "relB = " << relB << std::endl;
        // Compare the adapted constant parts.
        switch( relB )
        {
            case Relation::EQ:
                switch( relA )
                {
                    case Relation::EQ: // p+c=0  and  p+d=0
                        if( c == d ) return A_IFF_B; 
                        return NOT__A_AND_B;
                    case Relation::NEQ: // p+c!=0  and  p+d=0
                        if( c == d ) return A_XOR_B;
                        return B_IMPLIES_A;
                    case Relation::LESS: // p+c<0  and  p+d=0
                        if( c < d ) return B_IMPLIES_A;
                        return NOT__A_AND_B;
                    case Relation::GREATER: // p+c>0  and  p+d=0
                        if( c > d ) return B_IMPLIES_A;
                        return NOT__A_AND_B;
                    case Relation::LEQ: // p+c<=0  and  p+d=0
                        if( c <= d ) return B_IMPLIES_A;
                        return NOT__A_AND_B;
                    case Relation::GEQ: // p+c>=0  and  p+d=0
                        if( c >= d ) return B_IMPLIES_A;
                        return NOT__A_AND_B;
                    default:
                        return false;
                }
            case Relation::NEQ:
                switch( relA )
                {
                    case Relation::EQ: // p+c=0  and  p+d!=0
                        if( c == d ) return A_XOR_B;
                        return A_IMPLIES_B;
                    case Relation::NEQ: // p+c!=0  and  p+d!=0
                        if( c == d ) return A_IFF_B;
                        return 0;
                    case Relation::LESS: // p+c<0  and  p+d!=0
                        if( c >= d ) return A_IMPLIES_B;
                        return 0;
                    case Relation::GREATER: // p+c>0  and  p+d!=0
                        if( c <= d ) return A_IMPLIES_B;
                        return 0;
                    case Relation::LEQ: // p+c<=0  and  p+d!=0
                        if( c > d ) return A_IMPLIES_B;
                        if( c == d ) return A_AND_B__IFF_C;
                        return 0;
                    case Relation::GEQ: // p+c>=0  and  p+d!=0
                        if( c < d ) return A_IMPLIES_B;
                        if( c == d ) return A_AND_B__IFF_C;
                        return 0;
                    default:
                        return 0;
                }
            case Relation::LESS:
                switch( relA )
                {
                    case Relation::EQ: // p+c=0  and  p+d<0
                        if( c > d ) return A_IMPLIES_B;
                        return NOT__A_AND_B;
                    case Relation::NEQ: // p+c!=0  and  p+d<0
                        if( c <= d ) return B_IMPLIES_A;
                        return 0;
                    case Relation::LESS: // p+c<0  and  p+d<0
                        if( c == d ) return A_IFF_B;
                        if( c < d ) return B_IMPLIES_A;
                        return A_IMPLIES_B;
                    case Relation::GREATER: // p+c>0  and  p+d<0
                        if( c <= d ) return NOT__A_AND_B;
                        return 0;
                    case Relation::LEQ: // p+c<=0  and  p+d<0
                        if( c > d ) return A_IMPLIES_B;
                        return B_IMPLIES_A;
                    case Relation::GEQ: // p+c>=0  and  p+d<0
                        if( c < d ) return NOT__A_AND_B;
                        if( c == d ) return A_XOR_B;
                        return 0;
                    default:
                        return 0;
                }
            case Relation::GREATER:
            {
                switch( relA )
                {
                    case Relation::EQ: // p+c=0  and  p+d>0
                        if( c < d ) return A_IMPLIES_B;
                        return NOT__A_AND_B;
                    case Relation::NEQ: // p+c!=0  and  p+d>0
                        if( c >= d ) return B_IMPLIES_A;
                        return 0;
                    case Relation::LESS: // p+c<0  and  p+d>0
                        if( c >= d ) return NOT__A_AND_B;
                        return 0;
                    case Relation::GREATER: // p+c>0  and  p+d>0
                        if( c == d ) return A_IFF_B;
                        if( c > d ) return B_IMPLIES_A;
                        return A_IMPLIES_B;
                    case Relation::LEQ: // p+c<=0  and  p+d>0
                        if( c > d ) return NOT__A_AND_B;
                        if( c == d ) return A_XOR_B;
                        return 0;
                    case Relation::GEQ: // p+c>=0  and  p+d>0
                        if( c > d ) return B_IMPLIES_A;
                        return A_IMPLIES_B;
                    default:
                        return 0;
                }
            }
            case Relation::LEQ:
            {
                switch( relA )
                {
                    case Relation::EQ: // p+c=0  and  p+d<=0
                        if( c >= d ) return A_IMPLIES_B;
                        return NOT__A_AND_B;
                    case Relation::NEQ: // p+c!=0  and  p+d<=0
                        if( c < d ) return B_IMPLIES_A;
                        if( c == d ) return A_AND_B__IFF_C;
                        return 0;
                    case Relation::LESS: // p+c<0  and  p+d<=0
                        if( c < d ) return B_IMPLIES_A;
                        return A_IMPLIES_B;
                    case Relation::GREATER: // p+c>0  and  p+d<=0
                        if( c < d ) return NOT__A_AND_B;
                        if( c == d ) return A_XOR_B;
                        return 0;
                    case Relation::LEQ: // p+c<=0  and  p+d<=0
                        if( c == d ) return A_IFF_B;
                        if( c < d ) return B_IMPLIES_A;
                        return A_IMPLIES_B;
                    case Relation::GEQ: // p+c>=0  and  p+d<=0
                        if( c < d ) return NOT__A_AND_B;
                        if( c == d ) return A_AND_B__IFF_C;
                        return 0;
                    default:
                        return 0;
                }
            }
            case Relation::GEQ:
            {
                switch( relA )
                {
                    case Relation::EQ: // p+c=0  and  p+d>=0
                        if( c <= d ) return A_IMPLIES_B;
                        return NOT__A_AND_B;
                    case Relation::NEQ: // p+c!=0  and  p+d>=0
                        if( c > d ) return B_IMPLIES_A;
                        if( c == d ) return A_AND_B__IFF_C;
                        return 0;
                    case Relation::LESS: // p+c<0  and  p+d>=0
                        if( c > d ) return NOT__A_AND_B;
                        if( c == d ) return A_XOR_B;
                        return 0;
                    case Relation::GREATER: // p+c>0  and  p+d>=0
                        if( c < d ) return B_IMPLIES_A;
                        return A_IMPLIES_B;
                    case Relation::LEQ: // p+c<=0  and  p+d>=0
                        if( c > d ) return NOT__A_AND_B;
                        if( c == d ) return A_AND_B__IFF_C;
                        return 0;
                    case Relation::GEQ: // p+c>=0  and  p+d>=0
                        if( c == d ) return A_IFF_B;
                        if( c < d ) return A_IMPLIES_B;
                        return B_IMPLIES_A;
                    default:
                        return 0;
                }
            }
            default:
                return 0;
        }
    }
    
    /**
     * Prints the given constraint on the given stream.
     * @param _out The stream to print the given constraint on.
     * @param _constraint The formula to print.
     * @return The stream after printing the given constraint on it.
     */
    template<typename Poly>
    std::ostream& operator<<( std::ostream& _out, const Constraint<Poly>& _constraint );
    
}    // namespace carl

#define CONSTRAINT_HASH( _lhs, _rel, _type ) (size_t)((size_t)(std::hash<_type>()( _lhs ) << 3) ^ (size_t)_rel)

namespace std
{
    /**
     * Implements std::hash for constraint contents.
     */
    template<typename Pol>
    struct hash<carl::ConstraintContent<Pol>> {
        /**
         * @param _constraintContent The constraint content to get the hash for.
         * @return The hash of the given constraint content.
         */
        std::size_t operator()( const carl::ConstraintContent<Pol>& _constraintContent ) const 
        {
            return _constraintContent.hash();
        }
    };
    
    /**
     * Implements std::hash for constraints.
     */
    template<typename Pol>
    struct hash<carl::Constraint<Pol>> {
        /**
         * @param _constraint The constraint to get the hash for.
         * @return The hash of the given constraint.
         */
        std::size_t operator()( const carl::Constraint<Pol>& _constraint ) const 
        {
            return _constraint.getHash();
        }
    };
    
    /**
     * Implements std::hash for vectors of constraints.
     */
    template<typename Pol>
    struct hash<std::vector<carl::Constraint<Pol>>> {
        /**
         * @param _arg The vector of constraints to get the hash for.
         * @return The hash of the given vector of constraints.
         */
        std::size_t operator()( const std::vector<carl::Constraint<Pol>>& _arg ) const
        {
            std::size_t result = 0;
			for (const auto& c: _arg) {
                result <<= 5;
                result ^= c.id();
            }
            return result;
        }
    };
} // namespace std

#include "Constraint.tpp"
