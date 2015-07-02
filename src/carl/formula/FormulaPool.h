/**
 * @file FormulaPool.h
 *
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 * @version 2014-05-08
 */

#pragma once

#include "../util/Singleton.h"
#include "../core/VariablePool.h"
#include "Formula.h"
#include "ConstraintPool.h"
#include <mutex>
#include <limits>
#include <boost/variant.hpp>
#include "bitvector/BVConstraintPool.h"
#include "bitvector/BVConstraint.h"

#define SIMPLIFY_FORMULA

namespace carl
{
    
    template<typename Pol>
    class FormulaPool : public Singleton<FormulaPool<Pol>>
    {
        friend Singleton<FormulaPool>;
        friend Formula<Pol>;
        
        private:
            
            // Members:
            /// id allocator
            unsigned mIdAllocator;
            /// The unique formula representing true.
            FormulaContent<Pol>* mpTrue;
            /// The unique formula representing false.
            FormulaContent<Pol>* mpFalse;
            /// The formula pool.
            FastPointerSet<FormulaContent<Pol>> mPool;
            /// Mutex to avoid multiple access to the pool
            mutable std::recursive_mutex mMutexPool;
            ///
            FastPointerMap<FormulaContent<Pol>,const FormulaContent<Pol>*> mTseitinVars;
            
            #ifdef THREAD_SAFE
            #define FORMULA_POOL_LOCK_GUARD std::lock_guard<std::recursive_mutex> lock( mMutexPool );
            #define FORMULA_POOL_LOCK mMutexPool.lock();
            #define FORMULA_POOL_UNLOCK mMutexPool.unlock();
            #else
            #define FORMULA_POOL_LOCK_GUARD
            #define FORMULA_POOL_LOCK
            #define FORMULA_POOL_UNLOCK
            #endif
            
        protected:
            
            /**
             * Constructor of the formula pool.
             * @param _capacity Expected necessary capacity of the pool.
             */
            FormulaPool( unsigned _capacity = 10000 );
            
            ~FormulaPool();
    
            const FormulaContent<Pol>* trueFormula() const
            {
                return mpTrue;
            }

            const FormulaContent<Pol>* falseFormula() const
            {
                return mpFalse;
            }
            
        public:
            std::size_t size() const {
                return mPool.size();
            }
            
            void print() const
            {
                std::cout << "Formula pool contains:" << std::endl;
                for( const auto& ele : mPool )
                {
                    std::cout << "id " << ele->mId << "[usages=" << ele->mUsages << "]:  " << *ele << std::endl;
                }
                std::cout << std::endl;
            }
            
            Formula<Pol> getTseitinVar( const Formula<Pol>& _formula )
            {
                auto iter = mTseitinVars.find( _formula.mpContent );
                if( iter != mTseitinVars.end() )
                {
                    return Formula<Pol>( iter->second );
                }
                return trueFormula();
            }
            
            Formula<Pol> createTseitinVar( const Formula<Pol>& _formula )
            {
                auto iter = mTseitinVars.insert( std::make_pair( _formula.mpContent, nullptr ) );
                if( iter.second )
                {
                    const FormulaContent<Pol>* hi = create( carl::freshBooleanVariable() );
                    hi->mDifficulty = _formula.difficulty();
                    iter.first->second = hi;
                }
                return Formula<Pol>( iter.first->second );
            }
            
        private:
            // ##### Core Theory

            /**
             * Create formula representing a boolean value.
             * @param _type Formula type, may be either TRUE or FALSE.
             * @return A formula representing the given bool.
             */
            const FormulaContent<Pol>* create(FormulaType _type) {
                assert(_type == TRUE || _type == FALSE);
                return (_type == TRUE) ? trueFormula() : falseFormula();
            }

            /**
             * Create formula representing a boolean variable.
             * @param _booleanVar The Boolean variable wrapped by this formula.
             * @return A formula with wrapping the given Boolean variable.
             */
            const FormulaContent<Pol>* create(Variable::Arg _variable) {
                return add(new FormulaContent<Pol>(_variable));
            }
            
            /**
             * @param _constraint The constraint wrapped by this formula.
             * @return A formula with wrapping the given constraint.
             */
            const FormulaContent<Pol>* create(Constraint<Pol>&& _constraint) {
                #ifdef SIMPLIFY_FORMULA
                switch (_constraint.isConsistent()) {
                    case 0: return falseFormula();
                    case 1: return trueFormula();
                    default: ;
                }
                #endif
                return add(new FormulaContent<Pol>(std::move(_constraint)));
            }
            const FormulaContent<Pol>* create(const Constraint<Pol>& _constraint) {
                return create(std::move(Constraint<Pol>(_constraint)));
            }
            
            const FormulaContent<Pol>* create(BVConstraint&& _constraint) {
                #ifdef SIMPLIFY_FORMULA
                if (_constraint.isAlwaysConsistent()) return trueFormula();
                if (_constraint.isAlwaysInconsistent()) return falseFormula();
                #endif
                return add(new FormulaContent<Pol>(std::move(_constraint)));
            }
            const FormulaContent<Pol>* create(const BVConstraint& _constraint) {
                return create(std::move(BVConstraint(_constraint)));
            }
            
            
            /**
             * Create formula representing a unary function.
             * @param _type Formula type specifying the function.
             * @param _subFormula Formula representing the function argument.
             * @return A formula representing the given function call.
             */
            const FormulaContent<Pol>* create(FormulaType _type, Formula<Pol>&& _subFormula) {
                switch (_type) {
                    case ITE:
                    case EXISTS:
                    case FORALL:
                        assert(false); break;

                    // Core Theory
                    case TRUE:
                    case FALSE:
                    case BOOL:
                        assert(false); break;
                    case NOT:
                        return _subFormula.mpContent->mNegation;
                    case IMPLIES:
                        assert(false); break;
                    case AND:
                    case OR:
                    case XOR:
                        return _subFormula.mpContent;
                    case IFF:
                        return create(TRUE);

                    // Arithmetic Theory
                    case CONSTRAINT:
                        assert(false); break;

                    case BITVECTOR:
                    case UEQ:
                        assert(false); break;
                }
                return nullptr;
            }
            
            /**
             * Create formula representing a nary function.
             * @param _type Formula type specifying the function.
             * @param _subformulas Formula representing the function arguments.
             * @return A formula representing the given function call.
             */
            const FormulaContent<Pol>* create(FormulaType _type, const Formulas<Pol>& _subformulas) {
                return create(_type, std::move(Formulas<Pol>(_subformulas)));
            }
            const FormulaContent<Pol>* create(FormulaType _type, const std::initializer_list<Formula<Pol>>& _subformulas) {
                return create(_type, std::move(Formulas<Pol>(_subformulas.begin(), _subformulas.end())));
            }
            const FormulaContent<Pol>* create(FormulaType _type, Formulas<Pol>&& _subformulas) {
                switch (_type) {
                    case ITE:
                        return createITE(std::move(_subformulas));
                    case EXISTS:
                    case FORALL:
                    // Core Theory
                    case TRUE:
                    case FALSE:
                    case BOOL:
                    case NOT:
                        assert(false); break;
                    case IMPLIES:
                        return createImplication(std::move(_subformulas));
                    case AND:
                    case OR:
                    case XOR:
                    case IFF:
                        return createNAry(_type, std::move(_subformulas));
                    // Arithmetic Theory
                    case CONSTRAINT:
                    // Bitvector Theory
                    case BITVECTOR:
                    // Uninterpreted Theory
                    case UEQ:
                        assert(false); break;
                }
                return nullptr;
            }
    
            /**
             * Create formula representing an implication.
             * @param _subformulas
             * @return 
             */
            const FormulaContent<Pol>* createImplication(Formulas<Pol>&& _subformulas);
            
            const FormulaContent<Pol>* createNAry(FormulaType _type, Formulas<Pol>&& _subformulas);

            const FormulaContent<Pol>* createITE(Formulas<Pol>&& _subformulas);
            
			/**
			 *
			 * @param _type
			 * @param _vars
			 * @param _term
			 * @return
			 */
			const FormulaContent<Pol>* create(FormulaType _type, std::vector<Variable>&& _vars, const Formula<Pol>& _term) {
				assert(_type == FormulaType::EXISTS || _type == FormulaType::FORALL);
				if (_vars.size() > 0) {
					return add( new FormulaContent<Pol>(_type, std::move(_vars), _term ) );
				} else {
					return _term.mpContent;
				}
			}
            
            /**
             * @param _subformulas The sub-formulas of the formula to create.
             * @return A formula with the given operator and sub-formulas.
             */
            const FormulaContent<Pol>* create( const FormulasMulti<Pol>& _subformulas )
            {
                if( _subformulas.empty() ) return falseFormula();
                if( _subformulas.size() == 1 )
                {
                    return _subformulas.begin()->mpContent;
                }
                Formulas<Pol> subFormulas;
                auto lastSubFormula = _subformulas.begin();
                auto subFormula = lastSubFormula;
                ++subFormula;
                int counter = 1;
                while( subFormula != _subformulas.end() )
                {
                    if( *lastSubFormula == *subFormula )
                    {
                        ++counter;
                    }
                    else
                    {
                        if( counter % 2 == 1 )
                        {
                            subFormulas.insert( subFormulas.end(), *lastSubFormula ); // set has same order as the multiset
                        }
                        lastSubFormula = subFormula;
                        counter = 1;
                    }
                    ++subFormula;
                }
                if( counter % 2 == 1 )
                {
                    subFormulas.insert( subFormulas.end(), *lastSubFormula );
                }
                return create( FormulaType::XOR, std::move( subFormulas ) );
            }
            
			const FormulaContent<Pol>* create( const UEquality::Arg& _lhs, const UEquality::Arg& _rhs, bool _negated )
			{
                #ifdef SIMPLIFY_FORMULA
                if( boost::apply_visitor(UEquality::IsUVariable(), _lhs) && boost::apply_visitor(UEquality::IsUVariable(), _rhs) )
                {
                    if( boost::get<UVariable>(_lhs) < boost::get<UVariable>(_rhs) )
                        return add( new FormulaContent<Pol>( UEquality( boost::get<UVariable>(_lhs), boost::get<UVariable>(_rhs), _negated, true ) ) );
                    if( boost::get<UVariable>(_rhs) < boost::get<UVariable>(_lhs) )
                        return add( new FormulaContent<Pol>( UEquality( boost::get<UVariable>(_rhs), boost::get<UVariable>(_lhs), _negated, true ) ) );
                    else if( _negated )
                        return falseFormula();
                    else
                        return trueFormula();
                }
				else if( boost::apply_visitor(UEquality::IsUVariable(), _lhs) && boost::apply_visitor(UEquality::IsUFInstance(), _rhs) )
                {
                    return add( new FormulaContent<Pol>( UEquality( boost::get<UVariable>(_lhs), boost::get<UFInstance>(_rhs), _negated ) ) );
                }
                else if( boost::apply_visitor(UEquality::IsUFInstance(), _lhs) && boost::apply_visitor(UEquality::IsUVariable(), _rhs) )
                {
                    return add( new FormulaContent<Pol>( UEquality( boost::get<UVariable>(_rhs), boost::get<UFInstance>(_lhs), _negated ) ) );
                }
                else
                {
                    assert( boost::apply_visitor(UEquality::IsUFInstance(), _lhs) && boost::apply_visitor(UEquality::IsUFInstance(), _rhs) );
                    if( boost::get<UFInstance>(_lhs) < boost::get<UFInstance>(_rhs) )
                        return add( new FormulaContent<Pol>( UEquality( boost::get<UFInstance>(_lhs), boost::get<UFInstance>(_rhs), _negated, true ) ) );
                    if( boost::get<UFInstance>(_rhs) < boost::get<UFInstance>(_lhs) )
                        return add( new FormulaContent<Pol>( UEquality( boost::get<UFInstance>(_rhs), boost::get<UFInstance>(_lhs), _negated, true ) ) );
                    else if( _negated )
                        return falseFormula();
                    else
                        return trueFormula();
                }
                #else
                return add( new FormulaContent<Pol>( UEquality( _lhs, _rhs, _negated ) ) );
                #endif
			}

			const FormulaContent<Pol>* create( UEquality&& eq )
			{
				return add( new FormulaContent<Pol>( std::move( eq ) ) );
			}
            
            void free( const FormulaContent<Pol>* _elem )
            {   
                FORMULA_POOL_LOCK_GUARD
                const FormulaContent<Pol>* tmp = _elem->mType == FormulaType::NOT ? _elem->mNegation : _elem;
                assert( tmp->mUsages > 0 );
                --tmp->mUsages;
                if( tmp->mUsages == 1 )
                {
                    mTseitinVars.erase( tmp );
                    mTseitinVars.erase( tmp->mNegation );
                    mPool.erase( tmp );
                    delete tmp->mNegation;
                    delete tmp;
                }
            }
            
            void reg( const FormulaContent<Pol>* _elem ) const
            {
                FORMULA_POOL_LOCK_GUARD
                const FormulaContent<Pol>* tmp = _elem->mType == FormulaType::NOT ? _elem->mNegation : _elem;
                assert( tmp != nullptr );
                assert( tmp->mUsages < std::numeric_limits<size_t>::max() );
                ++tmp->mUsages;
            }
            
        public:
            template<typename ArgType>
            void forallDo( void (*_func)( ArgType*, const Formula<Pol>& ), ArgType* _arg ) const
            {
                FORMULA_POOL_LOCK_GUARD
                for( const FormulaContent<Pol>* formula : mPool )
                {
                    (*_func)( _arg, Formula<Pol>( formula ) );
                    if( formula != mpFalse )
                    {
                        (*_func)( _arg, Formula<Pol>( formula->mNegation ) );
                    }
                }
            }
            
            template<typename ReturnType, typename ArgType>
            std::map<const Formula<Pol>,ReturnType> forallDo( ReturnType (*_func)( ArgType*, const Formula<Pol>& ), ArgType* _arg ) const
            {
                FORMULA_POOL_LOCK_GUARD
                std::map<const Formula<Pol>,ReturnType> result;
                for( const FormulaContent<Pol>* elem : mPool )
                {
                    Formula<Pol> form(elem);
                    result[form] = (*_func)( _arg, form );
                    if( elem != mpFalse )
                    {
                        Formula<Pol> form(elem->mNegation);
                        result[form] = (*_func)( _arg, form );
                    }
                }
                return result;
            }
            
            /**
             */
            bool formulasInverse( const Formula<Pol>& _subformulaA, const Formula<Pol>& _subformulaB );
        
            /**
             * @param _type The type of the n-ary operator (n>1) of the formula to create.
             * @param _subformulas The sub-formulas of the formula to create.
             * @return A formula with the given operator and sub-formulas.
             * Note, that if you use this method to create a formula with the operator XOR
             * and you have collected the sub-formulas in a set, multiple occurrences of a
             * sub-formula are condensed. You should only use it, if you can exlcude this 
             * possibility. Otherwise use the method newExclusiveDisjunction.
             */
            //const FormulaContent<Pol>* create( FormulaType _type, Formulas<Pol>&& _subformulas );
            
    private:
        
            /**
             * Creates a formula of the given type but with only one sub-formula.
             * @param _type
             * @param _subformula
             * @return True, if the given type is IFF;
             *         False, if the given type is XOR;
             *         The given sub-formula if the type is AND or OR.
             */
            const FormulaContent<Pol>* newFormulaWithOneSubformula( FormulaType _type, const Formula<Pol>& _subformula )
            {
                assert( FormulaType::OR || FormulaType::AND || FormulaType::XOR || FormulaType::IFF );
                // We expect that this only happens, if the intended sub-formulas are all the same.
                switch( _type )
                {
                    case FormulaType::XOR: // f xor f is false
                        return falseFormula();
                    case FormulaType::IFF: // f iff f is true
                        return trueFormula();
                    default: // f or f = f; f and f = f
                    {
                        return _subformula.mpContent;
                    }
                }
            }
            
            /**
             * Inserts the given formula to the pool, if it does not yet occur in there.
             * @param _formula The formula to add to the pool.
             * @return The position of the given formula in the pool and true, if it did not yet occur in the pool;
             *         The position of the equivalent formula in the pool and false, otherwise.
             */
            std::pair<typename FastPointerSet<FormulaContent<Pol>>::iterator,bool> insert( FormulaContent<Pol>* _formula );
            
            /**
             * Adds the given formula to the pool, if it does not yet occur in there.
             * Note, that this method uses the allocator which is locked before calling.
             * @param _formula The formula to add to the pool.
             * @return The given formula, if it did not yet occur in the pool;
             *         The equivalent formula already occurring in the pool, otherwise.
             */
            const FormulaContent<Pol>* add( FormulaContent<Pol>* _formula );
    };
}    // namespace carl

#include "FormulaPool.tpp"
