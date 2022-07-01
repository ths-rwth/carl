#pragma once

namespace carl::formula::aux {

/**
 * [Auxiliary method]
 * @return The formula combining the first to the second last sub-formula of this formula by the
 *         same operator as the one of this formula.
 *         Example: this = (op a1 a2 .. an) -> return = (op a1 .. an-1)
 *                  If n = 2, return = a1
 */
template<typename Pol>
Formula<Pol> connectPrecedingSubformulas(const Formula<Pol>& f) {
    assert( f.is_nary() );
    if( f.subformulas().size() > 2 )
    {
        Formulas<Pol> tmpSubformulas;
        auto iter = f.subformulas().rbegin();
        ++iter;
        for( ; iter != f.subformulas().rend(); ++iter )
            tmpSubformulas.push_back( *iter );
        return Formula<Pol>( f.type(), tmpSubformulas );
    }
    else
    {
        assert( f.subformulas().size() == 2 );
        return *(f.subformulas().begin());
    }
}

} 
