#include "gtest/gtest.h"
#include "../../carl/formula/Ast.h"
#include "../../carl/formula/VariableNamePool.h"
#include "../../carl/util/stringparser.h"

#include <cln/cln.h>


using namespace carl;

typedef cln::cl_RA Rational;
typedef MultivariatePolynomial<Rational> Pol;
typedef Constraint<Pol> Constr;
typedef Ast<Pol> Astt;


TEST(Ast, Construction)
{
    Variable x = newArithmeticVariable( "x", VariableType::VT_REAL );
    Variable y = newArithmeticVariable( "y", VariableType::VT_REAL );
    Variable i = newArithmeticVariable( "i", VariableType::VT_INT );
    Variable b = newBooleanVariable( "b" );
    Sort sortS = newSort( "S" );
    Sort sortT = newSort( "T" );
    Variable u = VariableNamePool::getInstance().newUninterpretedVariable( "u" );
    Variable v = VariableNamePool::getInstance().newUninterpretedVariable( "v" );
    UVariable uu( u, sortS );
    UVariable uv( v, sortT );

    // Next we see an example how to create polynomials, which form the left-hand sides of the constraints:
    Pol px( x );
    Pol py( y );
    Pol lhsA = px.pow(2) - py;
    Pol lhsB = Rational(4) * px + py - Rational(8) * py.pow(7);
    
    // Constraints can then be constructed as follows:
    const Constr* constraintA = newConstraint<Pol>( lhsA, Relation::LESS );
    const Constr* constraintB = newConstraint<Pol>( lhsB, Relation::EQ );
    
    // Uninterpreted functions are 
    
    // Now, we can construct the atoms of the Boolean Ast
    const Astt* atomA = newAst<Pol>( constraintA );
    const Astt* atomB = newAst<Pol>( constraintB );
    const Astt* atomC = newBoolean<Pol>( b );
    
    // and the Ast itself:
    PointerSet<Astt> subAstsA;
    subAstsA.insert( newNegation<Pol>( atomC ) );
    subAstsA.insert( atomA );
    subAstsA.insert( atomB );
    const Astt* phiA = newAst<Pol>( AND, subAstsA );
    PointerSet<Astt> subAstsB;
    subAstsB.insert( newNegation<Pol>( atomA ) );
    subAstsB.insert( atomC );
    const Astt* phiC = newAst<Pol>( OR, subAstsB );
    const Astt* phiE = newImplication<Pol>( phiA, phiC );
}
