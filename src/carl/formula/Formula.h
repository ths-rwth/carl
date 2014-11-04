/**
 * Formula.h
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 * @since 2014-11-03
 * @version 2014-11-03
 */

#pragma once

#include "AstPool.h"


namespace carl
{
    template<typename Pol>
    class Formula
    {
        private:
            /// A pointer to the actual formula.
            const Ast<Pol>* mpContent;
            
        public:
            
            Formula( Ast<Pol>::Type _type = TRUE ):
                mpContent( _type == TRUE ? AstPool<Pol>::getInstance().trueAst() : AstPool<Pol>::getInstance().falseAst() )
            {}
                
            Formula( Variable::Arg _booleanVar ):
                mpContent( AstPool<Pol>::getInstance().newBoolean( _booleanVar ) )
            {}
                
            Formula( const Pol& _pol, Relation _rel ):
                mpContent( AstPool<Pol>::getInstance().newConstraintAst( ConstraintPool<Pol>::getInstance().newConstraint( _pol, _rel ) ) )
            {}
                
            Formula( Pol&& _pol, Relation _rel ):
                mpContent( AstPool<Pol>::getInstance().newConstraintAst( ConstraintPool<Pol>::getInstance().newConstraint( std::move( _pol ), _rel ) ) )
            {}
                
            Formula( const Constraint<Pol>* _constraint ):
                mpContent( AstPool<Pol>::getInstance().newConstraintAst( _constraint ) )
            {}
                
            Formula( Ast<Pol>::Type _type, const Ast<Pol>* _subast ):
                mpContent( AstPool<Pol>::getInstance().newNegation( _subast ) )
            {
                assert( _type == Ast<Pol>::Type::NOT );
            }
                
            Formula( Ast<Pol>::Type _type, const Ast<Pol>* _subastA, const Ast<Pol>* _subastB )
            {
                assert( _type == Ast<Pol>::Type::AND || _type == Ast<Pol>::Type::IFF || _type == Ast<Pol>::Type::IMPLIES 
                        || _type == Ast<Pol>::Type::OR || _type == Ast<Pol>::Type::XOR );
                if( _type == Ast<Pol>::Type::IMPLIES )
                {
                    mpContent = AstPool<Pol>::getInstance().newImplication( _subastA, _subastB );
                }
                else
                {
                    mpContent = AstPool<Pol>::getInstance().newAst( _type, _subastA, _subastB );
                }
            }
            
            Formula( Ast<Pol>::Type _type, const Ast<Pol>* _subastA, const Ast<Pol>* _subastB, const Ast<Pol>* _subastC )
            {
                assert( _type == Ast<Pol>::Type::AND || _type == Ast<Pol>::Type::IFF || _type == Ast<Pol>::Type::ITE 
                        || _type == Ast<Pol>::Type::OR || _type == Ast<Pol>::Type::XOR );
                if( _type == Ast<Pol>::Type::ITE )
                {
                    mpContent = AstPool<Pol>::getInstance().newIte( _subastA, _subastB, _subastC );
                }
                else
                {
                    PointerSet<Ast<Pol>> subAsts;
                    subAsts.insert( _subastA );
                    subAsts.insert( _subastB );
                    subAsts.insert( _subastC );
                    mpContent = AstPool<Pol>::getInstance().newAst( _type, subAsts );
                }
            }
            
            Formula( Ast<Pol>::Type _type, const PointerMultiSet<Ast<Pol>>& _subasts ):
                mpContent( AstPool<Pol>::getInstance().newExclusiveDisjunction( _subasts ) )
            {
                assert( _type == Ast<Pol>::Type::XOR );
            }
            
            Formula( Ast<Pol>::Type _type, const PointerSet<Ast<Pol>>& _subasts ):
                mpContent( AstPool<Pol>::getInstance().newAst( _type, _subasts ) )
            {}
            
            Formula( Ast<Pol>::Type _type, PointerSet<Ast<Pol>>&& _subasts ):
                mpContent( AstPool<Pol>::getInstance().newAst( _type, move(_subasts) ) )
            {}
            
            Formula( Ast<Pol>::Type _type, const std::vector<Variable>& _vars, const Ast<Pol>* _term ):
                mpContent( AstPool<Pol>::getInstance().newQuantifier( _type, std::move( _vars ), _term ) )
            {}
            
            Formula( const UEquality::Arg& _lhs, const UEquality::Arg& _rhs, bool _negated ):
                mpContent( AstPool<Pol>::getInstance().newEquality( _lhs, _rhs, _negated ) )
            {}
            
            Formula( UEquality&& eq ):
                mpContent( AstPool<Pol>::getInstance().newAst( std::move( eq ) ) )
            {}
    };
    
}