#include "gtest/gtest.h"
#include "../../carl/formula/Formula.h"
#include "../../carl/formula/VariableNamePool.h"
#include "../../carl/util/stringparser.h"

#include <cln/cln.h>


using namespace carl;

typedef cln::cl_RA Rational;
typedef MultivariatePolynomial<Rational> Pol;
typedef Constraint<Pol> Constr;

TEST(Formula, Construction)
{
    Variable x = newArithmeticVariable( "x", VariableType::VT_REAL );
    Variable y = newArithmeticVariable( "y", VariableType::VT_REAL );
//    Variable i = newArithmeticVariable( "i", VariableType::VT_INT );
    Variable b = newBooleanVariable( "b" );
//    Sort sortS = newSort( "S" );
//    Sort sortT = newSort( "T" );
//    Variable u = VariableNamePool::getInstance().newUninterpretedVariable( "u" );
//    Variable v = VariableNamePool::getInstance().newUninterpretedVariable( "v" );
//    UVariable uu( u, sortS );
//    UVariable uv( v, sortT );

    // Next we see an example how to create polynomials, which form the left-hand sides of the constraints:
    Pol px( x );
    Pol py( y );
    Pol lhsA = px.pow(2) - py;
    Pol lhsB = Rational(4) * px + py - Rational(8) * py.pow(7);
    
    // Constraints can then be constructed as follows:
    const Constr* constraintA = newConstraint<Pol>( lhsA, Relation::LESS );
    
    // Uninterpreted functions are 
    
    // Now, we can construct the atoms of the Boolean Ast
    const Formula<Pol> atomA = Formula<Pol>( constraintA );
    const Formula<Pol> atomB = Formula<Pol>( lhsB, Relation::EQ );
    const Formula<Pol> atomC = Formula<Pol>( b );
    
    // and the Ast itself:
    std::set<Formula<Pol>> subAstsA;
    subAstsA.insert( Formula<Pol>( FormulaType::NOT, atomC ) );
    subAstsA.insert( atomA );
    subAstsA.insert( atomB );
    const Formula<Pol> phiA( FormulaType::AND, subAstsA );
    const Formula<Pol> phiC( FormulaType::OR, Formula<Pol>( FormulaType::NOT, atomA ), atomC );
    const Formula<Pol> phiE( FormulaType::IMPLIES, phiA, phiC );
    std::cout << phiE << std::endl;
}
