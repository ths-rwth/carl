/**
 * @file FormulaPool.h
 *
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 * @version 2014-05-08
 */

#pragma once

#include <carl-common/memory/Singleton.h>
#include <carl/core/VariablePool.h>
#include "Formula.h"
#include <mutex>
#include <limits>
#include <boost/variant.hpp>
#include "../bitvector/BVConstraintPool.h"
#include "../bitvector/BVConstraint.h"
#include <boost/intrusive/unordered_set.hpp>

#define SIMPLIFY_FORMULA

namespace carl
{

    template<typename Pol>
    inline std::size_t hash_value(const carl::FormulaContent<Pol>& content) {
        return content.hash();
    }


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

            pool::RehashPolicy mRehashPolicy;
            using underlying_set = boost::intrusive::unordered_set<FormulaContent<Pol>>;
            std::unique_ptr<typename underlying_set::bucket_type[]> mPoolBuckets;
            /// The formula pool.
            underlying_set mPool;
            /// Mutex to avoid multiple access to the pool
            mutable std::recursive_mutex mMutexPool;

            ///
            FastPointerMap<FormulaContent<Pol>,const FormulaContent<Pol>*> mTseitinVars;
            ///
            FastPointerMap<FormulaContent<Pol>,typename FastPointerMap<FormulaContent<Pol>,const FormulaContent<Pol>*>::iterator> mTseitinVarToFormula;

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
                for (const auto& ele: mPool) {
                    std::cout << ele->mId << " @ " << static_cast<const void*>(ele) << " [usages=" << ele->mUsages << "]: " << *ele << ", negation " << static_cast<const void*>(ele->mNegation) << std::endl;
                }
                std::cout << "Tseitin variables:" << std::endl;
                for( const auto& tvVar : mTseitinVars )
                {
                    if( tvVar.second != nullptr )
                    {
                        std::cout << "id " << tvVar.first->mId << "  ->  " << tvVar.second->mId << " [remapping: ";
                        auto iter = mTseitinVarToFormula.find( tvVar.second );
                        if( iter != mTseitinVarToFormula.end() )
                        {
                            assert( iter->second != mTseitinVars.end() );
                            std::cout << iter->first->mId << " -> " << iter->second->first->mId << "]" << std::endl;
                        }
                        else
                            std::cout << "not yet remapped!" << std::endl;
                    }
                    else
                        std::cout << "id " << tvVar.first->mId << "  ->  nullptr" << std::endl;
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
                    iter.first->second = hi;
                    mTseitinVarToFormula[hi] = iter.first;
                }
                return Formula<Pol>( iter.first->second );
            }

        private:
			bool isBaseFormula(const Constraint<Pol>& c) const {
				return c < c.negation();
			}
			bool isBaseFormula(const UEquality& ueq) const {
				return !ueq.negated();
			}
			bool isBaseFormula(const VariableComparison<Pol>& vc) const {
				return vc < vc.negation();
			}
			bool isBaseFormula(const VariableAssignment<Pol>& va) const {
				return va < va.negation();
			}
            bool isBaseFormula(const FormulaContent<Pol>* f) const {
                // C++20:
                // return std::visit([]<T>(const T& a, const T& b) { return a < b; }, f->mContent, f->mNegation->mContent);
				if (f->mType == FormulaType::CONSTRAINT) {
					return std::get<Constraint<Pol>>(f->mContent) < std::get<Constraint<Pol>>(f->mNegation->mContent);
				} else if (f->mType == FormulaType::VARCOMPARE) {
					return std::get<VariableComparison<Pol>>(f->mContent) < std::get<VariableComparison<Pol>>(f->mNegation->mContent);
				} else if (f->mType == FormulaType::VARASSIGN) {
					return std::get<VariableAssignment<Pol>>(f->mContent) < std::get<VariableAssignment<Pol>>(f->mNegation->mContent);
				} else if (f->mType == FormulaType::UEQ) {
					return std::get<UEquality>(f->mContent) < std::get<UEquality>(f->mNegation->mContent);
				} else {
                    return f->mType != FormulaType::NOT;
                }
            }

            const FormulaContent<Pol>* getBaseFormula(const FormulaContent<Pol>* f) const {
                assert(f != nullptr);
                if (f->mType == FormulaType::NOT) {
                    CARL_LOG_TRACE("carl.formula", "Base formula of " << static_cast<const void*>(f) << " / " << *f << " is " << *f->mNegation);
                    return f->mNegation;
                }
                if (f->mType == FormulaType::CONSTRAINT || f->mType == FormulaType::UEQ || f->mType == FormulaType::VARCOMPARE || f->mType == FormulaType::VARASSIGN) {
                    if (isBaseFormula(f)) {
                        CARL_LOG_TRACE("carl.formula", "Base formula of " << static_cast<const void*>(f) << " / " << *f << " is " << *f);
                        return f;
                    } else {
                        CARL_LOG_TRACE("carl.formula", "Base formula of " << static_cast<const void*>(f) << " / " << *f << " is " << *f->mNegation);
                        return f->mNegation;
                    }
                }
                CARL_LOG_TRACE("carl.formula", "Base formula of " << static_cast<const void*>(f) << " / " << *f << " is " << *f);
                return f;
            }

            FormulaContent<Pol>* createNegatedContent(const FormulaContent<Pol>* f) const {
                if (f->mType == FormulaType::CONSTRAINT ||
                    f->mType == FormulaType::VARCOMPARE ||
                    f->mType == FormulaType::VARASSIGN ||
                    f->mType == FormulaType::UEQ) {
                    return std::visit(overloaded {
                        [](const Constraint<Pol>& a) { return new FormulaContent<Pol>(a.negation()); },
                        [](const VariableComparison<Pol>& a) { return new FormulaContent<Pol>(a.negation()); },
                        [](const VariableAssignment<Pol>& a) { return new FormulaContent<Pol>(a.negation()); },
                        [](const UEquality& a) { return new FormulaContent<Pol>(a.negation()); },
                        [](const auto&) { assert(false); return new FormulaContent<Pol>(FormulaType::FALSE); }
                    }, f->mContent);
				} else {
                    return new FormulaContent<Pol>(NOT, std::move(Formula<Pol>(f)));
                }
            }

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
             * @param _variable The Boolean variable wrapped by this formula.
             * @return A formula with wrapping the given Boolean variable.
             */
            const FormulaContent<Pol>* create(Variable _variable) {
                return add(FormulaContent<Pol>(_variable));
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
                if (isBaseFormula(_constraint)) {
                    return add(FormulaContent<Pol>(std::move(_constraint)));
                } else {
                    return add(FormulaContent<Pol>(_constraint.negation()))->mNegation;
                }
            }
            const FormulaContent<Pol>* create(const Constraint<Pol>& _constraint) {
                return create(std::move(Constraint<Pol>(_constraint)));
            }
			const FormulaContent<Pol>* create(VariableComparison<Pol>&& _variableComparison) {
				if (isBaseFormula(_variableComparison)) {
                    return add(FormulaContent<Pol>(std::move(_variableComparison)));
                } else {
                    return add(FormulaContent<Pol>(_variableComparison.negation()))->mNegation;
                }
            }
            const FormulaContent<Pol>* create(const VariableComparison<Pol>& _variableComparison) {
				auto val = _variableComparison.as_constraint();
				if (val) return create(Constraint<Pol>(*val));
                return create(std::move(VariableComparison<Pol>(_variableComparison)));
            }
			const FormulaContent<Pol>* create(VariableAssignment<Pol>&& _variableAssignment) {
				if (isBaseFormula(_variableAssignment)) {
                    return add(FormulaContent<Pol>(std::move(_variableAssignment)));
                } else {
                    return add(FormulaContent<Pol>(_variableAssignment.negation()))->mNegation;
                }
            }
            const FormulaContent<Pol>* create(const VariableAssignment<Pol>& _variableAssignment) {
				return create(std::move(VariableAssignment<Pol>(_variableAssignment)));
            }

            const FormulaContent<Pol>* create(BVConstraint&& _constraint) {
                #ifdef SIMPLIFY_FORMULA
                if (_constraint.isAlwaysConsistent()) return trueFormula();
                if (_constraint.isAlwaysInconsistent()) return falseFormula();
                #endif
                return add(FormulaContent<Pol>(std::move(_constraint)));
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
					case VARCOMPARE:
					case VARASSIGN:
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
					// VarCompare
					case VARCOMPARE:
					case VARASSIGN:
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
				if (_vars.empty()) {
					return _term.mpContent;
				} else {
					return add(FormulaContent<Pol>(_type, std::move(_vars), _term ) );
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

			const FormulaContent<Pol>* create( const UTerm& _lhs, const UTerm& _rhs, bool _negated )
			{
                #ifdef SIMPLIFY_FORMULA
				if (_lhs == _rhs) {
					if (_negated) return falseFormula();
					else return trueFormula();
				}
				#endif
				return create(UEquality(_lhs, _rhs, _negated));
			}

			const FormulaContent<Pol>* create( UEquality&& eq )
			{
				if (isBaseFormula(eq)) {
                    return add(FormulaContent<Pol>(std::move(eq)));
                } else {
                    return add(FormulaContent<Pol>(eq.negation()))->mNegation;
                }
			}

            void free( const FormulaContent<Pol>* _elem )
            {
                FORMULA_POOL_LOCK_GUARD
                const FormulaContent<Pol>* tmp = getBaseFormula(_elem);
				assert(tmp == getBaseFormula(tmp));
				assert(isBaseFormula(tmp));
                assert( tmp->mUsages > 0 );
                --tmp->mUsages;
				CARL_LOG_TRACE("carl.formula", "Usage of " << static_cast<const void*>(tmp) << " / " << static_cast<const void*>(tmp->mNegation) << " (coming from " << static_cast<const void*>(_elem) << "): " << tmp->mUsages);
                if( tmp->mUsages == 1 )
                {
					CARL_LOG_DEBUG("carl.formula", "Actually freeing " << *tmp << " from pool");
                    bool stillStoredAsTseitinVariable = false;
                    if( freeTseitinVariable( tmp ) )
                        stillStoredAsTseitinVariable = true;
                    if( freeTseitinVariable( tmp->mNegation ) )
                        stillStoredAsTseitinVariable = true;
                    if( !stillStoredAsTseitinVariable )
                    {
						CARL_LOG_TRACE("carl.formula", "Deleting " << tmp << " / " << tmp->mNegation << " from pool");
                        
                        //auto it = mPool.find(*tmp->mNegation);
                        //assert(it != mPool.end());
                        //mPool.erase(it);
                        assert(mPool.find(*tmp->mNegation) == mPool.end());
                        auto it = mPool.find(*tmp);
                        assert(it != mPool.end());
                        mPool.erase(it);
                        delete tmp->mNegation;
                        delete tmp;
                    }
                }
            }

            bool freeTseitinVariable( const FormulaContent<Pol>* _toDelete )
            {
                bool stillStoredAsTseitinVariable = false;
                auto tvIter = mTseitinVars.find( _toDelete );
                if( tvIter != mTseitinVars.end() )
                {
                    // if this formula HAS a tseitin variable
                    if( tvIter->second->mUsages == 1 )
                    {
                        // the tseitin variable is not used -> delete it
                        const FormulaContent<Pol>* tmp = tvIter->second;
                        mTseitinVars.erase( tvIter );
                        assert( mTseitinVarToFormula.find( tmp ) != mTseitinVarToFormula.end() );
                        mTseitinVarToFormula.erase( tmp );
						CARL_LOG_TRACE("carl.formula", "Deleting " << static_cast<const void*>(tmp) << " / " << static_cast<const void*>(tmp->mNegation) << " from pool");
                        mPool.erase( *tmp );
                        delete tmp->mNegation;
                        delete tmp;
                    }
                    else // the tseitin variable is used, so we cannot delete the formula
                        stillStoredAsTseitinVariable = true;
                }
                else
                {
                    auto tmpTVIter = mTseitinVarToFormula.find( _toDelete );
                    if( tmpTVIter != mTseitinVarToFormula.end() )
                    {
                        const FormulaContent<Pol>* fcont = tmpTVIter->second->first;
                        // if this formula IS a tseitin variable
                        if( fcont->mUsages == 1 )
                        {
                            // the formula variable is not used -> delete it
                            const FormulaContent<Pol>* tmp = getBaseFormula(fcont);
                            //const FormulaContent<Pol>* tmp = fcont->mType == FormulaType::NOT ? fcont->mNegation : fcont;
                            mTseitinVars.erase( tmpTVIter->second );
                            mTseitinVarToFormula.erase( tmpTVIter );
							CARL_LOG_TRACE("carl.formula", "Deleting " << static_cast<const void*>(tmp) << " / " << static_cast<const void*>(tmp->mNegation) << " from pool");
                            mPool.erase( *tmp );
                            delete tmp->mNegation;
                            delete tmp;
                        }
                        else // the formula is used, so we cannot delete the tseitin variable
                            stillStoredAsTseitinVariable = true;
                    }
                }
                return stillStoredAsTseitinVariable;
            }

            void reg( const FormulaContent<Pol>* _elem ) const
            {
                FORMULA_POOL_LOCK_GUARD
                const FormulaContent<Pol>* tmp = getBaseFormula(_elem);
                //const FormulaContent<Pol>* tmp = _elem->mType == FormulaType::NOT ? _elem->mNegation : _elem;
                assert( tmp != nullptr );
                assert( tmp->mUsages < std::numeric_limits<size_t>::max() );
                ++tmp->mUsages;
                if (tmp->mUsages == 1 && (tmp->mType == FormulaType::CONSTRAINT || tmp->mType == FormulaType::UEQ || tmp->mType == FormulaType::VARCOMPARE || tmp->mType == FormulaType::VARASSIGN)) {
                    CARL_LOG_TRACE("carl.formula", "Is a constraint, increasing again");
                    ++tmp->mUsages;
                }
				CARL_LOG_TRACE("carl.formula", "Increased usage of " << static_cast<const void*>(tmp) << " / " << static_cast<const void*>(tmp->mNegation) << "(based on " << static_cast<const void*>(_elem) << ")" << " to " << tmp->mUsages);
            }

        public:
            template<typename ArgType>
            void forallDo( void (*_func)( ArgType*, const Formula<Pol>& ), ArgType* _arg ) const
            {
                FORMULA_POOL_LOCK_GUARD
                for( const FormulaContent<Pol>& formula : mPool )
                {
                    (*_func)( _arg, Formula<Pol>( &formula ) );
                    if( &formula != mpFalse )
                    {
                        (*_func)( _arg, Formula<Pol>( formula.mNegation ) );
                    }
                }
            }

            /**
             */
            bool formulasInverse( const Formula<Pol>& _subformulaA, const Formula<Pol>& _subformulaB );

            /* *
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
             * Adds the given formula to the pool, if it does not yet occur in there.
             * Note, that this method uses the allocator which is locked before calling.
             * @param _formula The formula to add to the pool.
             * @return The given formula, if it did not yet occur in the pool;
             *         The equivalent formula already occurring in the pool, otherwise.
             */
            const FormulaContent<Pol>* add( FormulaContent<Pol>&& _formula );

            void check_rehash() {
                auto rehash = mRehashPolicy.needRehash(mPool.bucket_count(), mPool.size());
                if (rehash.first) {
                    auto new_buckets = new typename underlying_set::bucket_type[rehash.second];
                    mPool.rehash(typename underlying_set::bucket_traits(new_buckets, rehash.second));
                    mPoolBuckets.reset(new_buckets);
                }
            }

    };
}    // namespace carl

#include "FormulaPool.tpp"
