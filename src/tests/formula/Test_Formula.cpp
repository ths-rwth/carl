#include "gtest/gtest.h"
#include "../../carl/core/MultivariatePolynomial.h"
#include "../../carl/core/VariablePool.h"
#include "../../carl/formula/Formula.h"
#include "../../carl/util/stringparser.h"

#include <cln/cln.h>


using namespace carl;

typedef cln::cl_RA Rational;
typedef MultivariatePolynomial<Rational> Pol;
typedef Constraint<Pol> Constr;

TEST(Formula, Construction)
{
    Variable x = VariablePool::getInstance().getFreshVariable( "x", VariableType::VT_REAL );
    Variable y = VariablePool::getInstance().getFreshVariable( "y", VariableType::VT_REAL );
    Variable i1 = VariablePool::getInstance().getFreshVariable( "i1", VariableType::VT_INT );
    Variable i2 = VariablePool::getInstance().getFreshVariable( "i2", VariableType::VT_INT );
    Variable i3 = VariablePool::getInstance().getFreshVariable( "i3", VariableType::VT_INT );
//    Variable i = newArithmeticVariable( "i", VariableType::VT_INT );
    Variable b = VariablePool::getInstance().getFreshVariable( "b", VariableType::VT_BOOL );
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
    Pol lhsD = px*py;
    Pol pi1( i1 );
    Pol pi2( i2 );
    Pol pi3( i3 );
    Pol lhsC = Rational(2) * pi1 + Rational(2) * pi2 + Rational(2) * pi3 - Rational(5);
    
    // Constraints can then be constructed as follows:
    Constr constraintA = Constr( lhsD, Relation::EQ );
    
    // Uninterpreted functions are 
    
    // Now, we can construct the atoms of the Boolean Ast
    const Formula<Pol> atomA = Formula<Pol>( constraintA );
    const Formula<Pol> atomB = Formula<Pol>( lhsB, Relation::EQ );
    const Formula<Pol> atomC = Formula<Pol>( b );
    const Formula<Pol> inEq = Formula<Pol>( lhsC, Relation::EQ );
    std::cout << inEq << std::endl;
    EXPECT_TRUE( inEq.getType() == FormulaType::FALSE );
    
    // and the Ast itself:
    Formulas<Pol> subAstsA;
    subAstsA.insert( Formula<Pol>( FormulaType::NOT, atomC ) );
    subAstsA.insert( atomA );
    subAstsA.insert( atomB );
    const Formula<Pol> phiA( FormulaType::AND, subAstsA );
    const Formula<Pol> phiC( FormulaType::OR, Formula<Pol>( FormulaType::NOT, atomA ), atomC );
    const Formula<Pol> phiE( FormulaType::IMPLIES, phiA, phiC );
    std::cout << phiE << std::endl;
}
