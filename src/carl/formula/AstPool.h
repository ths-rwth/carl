/**
 * @file AstPool.h
 *
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 * @version 2014-05-08
 */

#pragma once

#include "Ast.h"
#include "VariableNamePool.h"
#include "ConstraintPool.h"
#include "../util/Singleton.h"
#include <mutex>

#define SIMPLIFY_AST

namespace carl
{
    
    template<typename Pol>
    class AstPool : public Singleton<AstPool<Pol>>
    {
        friend Singleton<AstPool>;
        
        private:
            // Members:
            /// id allocator
            unsigned mIdAllocator;
            /// The unique ast representing true.
            Ast<Pol>* mpTrue;
            /// The unique ast representing false.
            Ast<Pol>* mpFalse;
            /// The ast pool.
            FastPointerSet<Ast<Pol>> mAsts;
            /// Mutex to avoid multiple access to the pool
            mutable std::mutex mMutexPool;
            
            #define AST_POOL_LOCK_GUARD std::lock_guard<std::mutex> lock( mMutexPool );
            #define AST_POOL_LOCK mMutexPool.lock();
            #define AST_POOL_UNLOCK mMutexPool.unlock();
            
        protected:
            
            /**
             * Constructor of the ast pool.
             * @param _capacity Expected necessary capacity of the pool.
             */
            AstPool( unsigned _capacity = 10000 );
            
        public:

            /**
             * Destructor.
             */
            ~AstPool();
    
            const Ast<Pol>* trueAst()
            {
                return mpTrue;
            }

            const Ast<Pol>* falseAst()
            {
                return mpFalse;
            }

            /**
             * @param _booleanVar The Boolean variable wrapped by this ast.
             * @return A ast with wrapping the given Boolean variable.
             */
            const Ast<Pol>* newBoolean( Variable::Arg _booleanVar )
            {
                return addAstToPool( new Ast<Pol>( _booleanVar ) );
            }
            
            /**
             * @param _constraint The constraint wrapped by this ast.
             * @return A ast with wrapping the given constraint.
             */
            const Ast<Pol>* newConstraintAst( const Constraint<Pol>* _constraint )
            {
                #ifdef SIMPLIFY_AST
                if( _constraint == constraintPool<Pol>().consistentConstraint() )
                    return mpTrue;
                if( _constraint == constraintPool<Pol>().inconsistentConstraint() )
                    return mpFalse;
                #endif
                return addAstToPool( new Ast<Pol>( _constraint ) );
            }
            
            /**
             * 
             * @param _subast
             * @return 
             */
            const Ast<Pol>* newNegation( const Ast<Pol>* _subast )
            {
                #ifdef SIMPLIFY_AST
                if( _subast == mpTrue )
                    return mpFalse;
                if( _subast == mpFalse )
                    return mpTrue;
				if (_subast->getType() == Ast<Pol>::Type::NOT)
					return _subast->pSubast();
                #endif
                // TODO: Actually we know that this ast does not begin with NOT and is already in the pool. Use this for optimization purposes.
                return addAstToPool( new Ast<Pol>( _subast ) );
            }
    
            /**
             * 
             * @param _subastA
             * @param _subastB
             * @return 
             */
            const Ast<Pol>* newImplication( const Ast<Pol>* _premise, const Ast<Pol>* _conclusion )
            {
                #ifdef SIMPLIFY_AST
                if( _premise == mpFalse )
                    return mpTrue;
                if( _premise == mpTrue )
                    return _conclusion;
                if( _conclusion == mpTrue )
                    return mpTrue;
                if( _conclusion == mpFalse )
                    return newNegation( _premise );
                #endif
                return addAstToPool( new Ast<Pol>( _premise, _conclusion ) );
            }
    
            /**
             * 
             * @param _subastA
             * @param _subastB
             * @return 
             */
            const Ast<Pol>* newIte( const Ast<Pol>* _condition, const Ast<Pol>* _then, const Ast<Pol>* _else )
            {
                #ifdef SIMPLIFY_AST
                if( _condition == mpFalse || *_then == *_else )
                    return _else;
                if( _condition == mpTrue )
                    return _then;
                #endif
                return addAstToPool( new Ast<Pol>( _condition, _then, _else ) );
            }

			/**
			 *
			 * @param _type
			 * @param _vars
			 * @param _term
			 * @return
			 */
			const Ast<Pol>* newQuantifier(typename Ast<Pol>::Type _type, const std::vector<Variable>&& _vars, const Ast<Pol>* _term)
			{
				assert(_type == typename Ast<Pol>::Type::EXISTS || _type == typename Ast<Pol>::Type::FORALL);
				if (_vars.size() > 0) {
					return addAstToPool(new Ast<Pol>(_type, std::move(_vars), _term));
				} else {
					return _term;
				}
			}
            
            /**
             * @param _type The type of the n-ary operator (n>1) of the ast to create.
             * @param _subastA The first sub-ast of the ast to create.
             * @param _subastB The second sub-ast of the ast to create.
             * @return A ast with the given operator and sub-asts.
             */
            const Ast<Pol>* newAst( typename Ast<Pol>::Type _type, const Ast<Pol>* _subastA, const Ast<Pol>* _subastB )
            {
                PointerSet<Ast<Pol>> subasts;
                subasts.insert( _subastA );
                subasts.insert( _subastB );
                return newAst( _type, std::move( subasts ) );
            }
            
            /**
             * @param _type The type of the n-ary operator (n>1) of the ast to create.
             * @param _subasts The sub-asts of the ast to create.
             * @return A ast with the given operator and sub-asts.
             */
            const Ast<Pol>* newExclusiveDisjunction( const PointerMultiSet<Ast<Pol>>& _subasts )
            {
                if( _subasts.empty() ) return mpFalse;
                if( _subasts.size() == 1 ) return *_subasts.begin();
                PointerSet<Ast<Pol>> subasts;
                auto lastSubAst = _subasts.begin();
                auto subAst = lastSubAst;
                ++subAst;
                int counter = 1;
                while( subAst != _subasts.end() )
                {
                    if( **lastSubAst == **subAst )
                    {
                        ++counter;
                    }
                    else
                    {
                        if( counter % 2 == 1 )
                        {
                            subasts.insert( subasts.end(), *lastSubAst ); // set has same order as the multiset
                        }
                        lastSubAst = subAst;
                        counter = 1;
                    }
                    ++subAst;
                }
                if( counter % 2 == 1 )
                {
                    subasts.insert( subasts.end(), *lastSubAst );
                }
                return createAst( Ast<Pol>::XOR, std::move( subasts ) );
            }
            
            /**
             * @param _type The type of the n-ary operator (n>1) of the ast to create.
             * @param _subasts The sub-asts of the ast to create.
             * @return A ast with the given operator and sub-asts.
             */
            const Ast<Pol>* newAst( typename Ast<Pol>::Type _type, const PointerSet<Ast<Pol>>& _subasts )
            {
                return createAst( _type, std::move( PointerSet<Ast<Pol>>( _subasts ) ) );
            }
            
            /**
             * @param _type The type of the n-ary operator (n>1) of the ast to create.
             * @param _subasts The sub-asts of the ast to create.
             * @return A ast with the given operator and sub-asts.
             * Note, that if you use this method to create a ast with the operator XOR
             * and you have collected the sub-asts in a set, multiple occurrences of a
             * sub-ast are condensed. You should only use it, if you can exlcude this 
             * possibility. Otherwise use the method newExclusiveDisjunction.
             */
            const Ast<Pol>* newAst( typename Ast<Pol>::Type _type, PointerSet<Ast<Pol>>&& _subasts )
            {
                assert( _type == AND || _type == OR || _type == IFF || _type == XOR  );
                return createAst( _type, std::move( _subasts ) );
            }

			const Ast<Pol>* newEquality( const UEquality::Arg& _lhs, const UEquality::Arg& _rhs, bool _negated )
			{
				return addAstToPool( new Ast<Pol>( UEquality( _lhs, _rhs, _negated ) ) );
			}

			const Ast<Pol>* newAst( UEquality&& eq )
			{
				return addAstToPool( new Ast<Pol>( std::move( eq ) ) );
			}
            
            template<typename ArgType>
            void forallDo( void (*_func)( ArgType*, const Ast<Pol>* ), ArgType* _arg ) const
            {
                AST_POOL_LOCK_GUARD
                for( const Ast<Pol>* ast : mAsts )
                    (*_func)( _arg, ast );
            }
            
            template<typename ReturnType, typename ArgType>
            PointerMap<Ast<Pol>,ReturnType> forallDo( ReturnType (*_func)( ArgType*, const Ast<Pol>* ), ArgType* _arg ) const
            {
                AST_POOL_LOCK_GUARD
                PointerMap<Ast<Pol>,ReturnType> result;
                for( const Ast<Pol>* ast : mAsts )
                    result.push_back( (*_func)( _arg, ast ) );
                return result;
            }
            
    private:
        
            const Ast<Pol>* createAst( typename Ast<Pol>::Type _type, PointerSet<Ast<Pol>>&& _subasts );
        
            /**
             * Creates a ast of the given type but with only one sub-ast.
             * @param _type
             * @param _subast
             * @return True, if the given type is IFF;
             *         False, if the given type is XOR;
             *         The given sub-ast if the type is AND or OR.
             */
            const Ast<Pol>* newAstWithOneSubast( typename Ast<Pol>::Type _type, const Ast<Pol>* _subast )
            {
                assert( Ast<Pol>::OR || Ast<Pol>::AND || Ast<Pol>::XOR || Ast<Pol>::IFF );
                // We expect that this only happens, if the intended sub-asts are all the same.
                switch( _type )
                {
                    case Ast<Pol>::XOR: // f xor f is false
                        return mpFalse;
                    case Ast<Pol>::IFF: // f iff f is true
                        return mpTrue;
                    default: // f or f = f; f and f = f
                        return _subast;
                }
            }
            
            /**
             * Adds the given ast to the pool, if it does not yet occur in there.
             * Note, that this method uses the allocator which is locked before calling.
             * @sideeffect The given ast will be deleted, if it already occurs in the pool.
             * @param _ast The ast to add to the pool.
             * @return The given ast, if it did not yet occur in the pool;
             *         The equivalent ast already occurring in the pool, otherwise.
             */
            const Ast<Pol>* addAstToPool( Ast<Pol>* _ast );
    };
}    // namespace carl

#include "AstPool.tpp"