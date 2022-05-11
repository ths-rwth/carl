#pragma once

namespace carl {

/**
 * Resolves the outermost negation of this formula.
 * @param _keepConstraint A flag indicating whether to change constraints in order
 * to resolve the negation in front of them, or to keep the constraints and leave
 * the negation.
 */
template<typename Pol>
Formula<Pol> resolve_negation( const Formula<Pol>& f, bool _keepConstraint = true ) {
    if( f.type() != FormulaType::NOT ) return f;
    FormulaType newType = f.type();
    switch( f.subformula().type() )
    {
        case FormulaType::BOOL:
            return f;
        case FormulaType::UEQ:
            if( _keepConstraint )
                return f;
            else
            {
                const UEquality& ueq = f.subformula().u_equality();
                return Formula<Pol>( ueq.lhs(), ueq.rhs(), !ueq.negated() );
            }
        case FormulaType::CONSTRAINT:
        {
            const Constraint<Pol>& constraint = f.subformula().constraint();
            if( _keepConstraint )
                return f;
            else
            {
                switch( constraint.relation() )
                {
                    case Relation::EQ:
                    {
                        return Formula<Pol>( constraint.lhs(), Relation::NEQ );
                    }
                    case Relation::LEQ:
                    {
                        return Formula<Pol>( -constraint.lhs(), Relation::LESS );
                    }
                    case Relation::LESS:
                    {
                        return Formula<Pol>( -constraint.lhs(), Relation::LEQ );
                    }
                    case Relation::GEQ:
                    {
                        return Formula<Pol>( constraint.lhs(), Relation::LESS );
                    }
                    case Relation::GREATER:
                    {
                        return Formula<Pol>( constraint.lhs(), Relation::LEQ );
                    }
                    case Relation::NEQ:
                    {
                        return Formula<Pol>( constraint.lhs(), Relation::EQ );
                    }
                    default:
                    {
                        assert( false );
                        std::cerr << "Unexpected relation symbol!" << std::endl;
                        return f;
                    }
                }
            }
        }
        case FormulaType::VARCOMPARE: {
            return Formula<Pol>(f.subformula().variable_comparison().negation());
        }
        case FormulaType::VARASSIGN: {
            assert(false);
            return Formula<Pol>();
        }
        case FormulaType::BITVECTOR: {
            BVCompareRelation rel = inverse(f.subformula().bv_constraint().relation());
            return Formula<Pol>( BVConstraint::create(rel, f.subformula().bv_constraint().lhs(), f.subformula().bv_constraint().rhs()));
        }
        case FormulaType::TRUE: // (not true)  ->  false
            return Formula<Pol>( FormulaType::FALSE );
        case FormulaType::FALSE: // (not false)  ->  true
            return Formula<Pol>( FormulaType::TRUE );
        case FormulaType::NOT: // (not (not phi))  ->  phi
            return f.subformula().subformula();
        case FormulaType::IMPLIES:
        {
            assert( f.subformula().size() == 2 );
            // (not (implies lhs rhs))  ->  (and lhs (not rhs))
            Formulas<Pol> subFormulas;
            subFormulas.push_back( f.subformula().premise() );
            subFormulas.push_back( Formula<Pol>( NOT, f.subformula().conclusion() ) );
            return Formula<Pol>( AND, move( subFormulas ) );
        }
        case FormulaType::ITE: // (not (ite cond then else))  ->  (ite cond (not then) (not else))
        {
            return Formula<Pol>( ITE, {f.subformula().condition(), Formula<Pol>( NOT, f.subformula().first_case() ), Formula<Pol>( NOT, f.subformula().second_case() )} );
        }
        case FormulaType::IFF: // (not (iff phi_1 .. phi_n))  ->  (and (or phi_1 .. phi_n) (or (not phi_1) .. (not phi_n)))
        {
            Formulas<Pol> subFormulasA;
            Formulas<Pol> subFormulasB;
            for( auto& subFormula : f.subformula().subformulas() )
            {
                subFormulasA.push_back( subFormula );
                subFormulasB.push_back( Formula<Pol>( NOT, subFormula ) );
            }
            return Formula<Pol>( AND, {Formula<Pol>( OR, move( subFormulasA ) ), Formula<Pol>( OR, move( subFormulasB ) )} );
        }
        case FormulaType::XOR: // (not (xor phi_1 .. phi_n))  ->  (xor (not phi_1) phi_2 .. phi_n)
        {
            auto subFormula = f.subformula().subformulas().begin();
            Formulas<Pol> subFormulas;
            subFormulas.push_back( Formula<Pol>( NOT, *subFormula ) );
            ++subFormula;
            for( ; subFormula != f.subformula().subformulas().end(); ++subFormula )
                subFormulas.push_back( *subFormula );
            return Formula<Pol>( XOR, move( subFormulas ) );
        }
        case FormulaType::AND: // (not (and phi_1 .. phi_n))  ->  (or (not phi_1) .. (not phi_n))
            newType = FormulaType::OR;
            break;
        case FormulaType::OR: // (not (or phi_1 .. phi_n))  ->  (and (not phi_1) .. (not phi_n))
            newType = FormulaType::AND;
            break;
        case FormulaType::EXISTS: // (not (exists (vars) phi)) -> (forall (vars) (not phi))
            newType = FormulaType::FORALL;
            break;
        case FormulaType::FORALL: // (not (forall (vars) phi)) -> (exists (vars) (not phi))
            newType = FormulaType::EXISTS;
            break;
        default:
            assert( false );
            std::cerr << "Unexpected type of formula!" << std::endl;
            return f;
    }
    assert( newType != f.subformula().type() );
    assert( f.subformula().type() == FormulaType::AND || f.subformula().type() == FormulaType::OR );
    Formulas<Pol> subFormulas;
    for( const Formula<Pol>& subsubformula : f.subformula().subformulas() )
        subFormulas.push_back( Formula<Pol>( FormulaType::NOT, subsubformula ) );
    return Formula<Pol>( newType, move( subFormulas ) );
}

}