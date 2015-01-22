#include "gtest/gtest.h"
#include "../../carl/formula/bitvector/BVConstraint.h"
#include "../../carl/core/MultivariatePolynomial.h"
// #include "../../carl/core/VariablePool.h"
// #include "../../carl/formula/Formula.h"
// #include "../../carl/util/stringparser.h"

#include <cln/cln.h>


using namespace carl;

typedef cln::cl_RA Rational;
typedef MultivariatePolynomial<Rational> Pol;
// typedef Constraint<Pol> Constr;

TEST(BVConstraint, Construction)
{
	Variable a = VariablePool::getInstance().getFreshVariable( "a", VariableType::VT_REAL ); // TODO: Should be VT_BITVECTOR
	Variable b = VariablePool::getInstance().getFreshVariable( "b", VariableType::VT_REAL ); // TODO: Should be VT_BITVECTOR

	const BVTerm<Pol> a_t( BVTermType::VARIABLE, a, 16 );
	const BVTerm<Pol> b_t( BVTermType::VARIABLE, b, 16 );

	const BVTerm<Pol> oxfff0( BVTermType::CONSTANT, BVValue( 16, 65520 ));

	const Formula<Pol> trueFormula = Formula<Pol>( FormulaType::TRUE );

	const BVTerm<Pol> ite( BVTermType::ITE, trueFormula, a_t, b_t );

	const BVTerm<Pol> bvand( BVTermType::AND, a_t, oxfff0 );
	const BVTerm<Pol> bvmul( BVTermType::MUL, bvand, b_t );
	
	const BVTerm<Pol> bvnot( BVTermType::NOT, bvmul );

	const BVTerm<Pol> oxa( BVTermType::CONSTANT, BVValue( 4, 10 ));
	const BVTerm<Pol> oxaa( BVTermType::REPEAT, oxa, 2 );

	const BVTerm<Pol> bvextract( BVTermType::EXTRACT, bvnot, 6, 8 );
	const BVTerm<Pol> bvconcat( BVTermType::CONCAT, bvextract, bvextract );

	const BVTerm<Pol> bvzeroext( BVTermType::EXT_U, bvconcat, 8);

	BVConstraint<Pol> constraint( BVCompareRelation::SLT, oxaa, bvzeroext );

	std::cout << constraint.toString(false, 0, "", false, false, true) << std::endl;
}
//     Variable x = VariablePool::getInstance().getFreshVariable( "x", VariableType::VT_REAL );
//     Variable y = VariablePool::getInstance().getFreshVariable( "y", VariableType::VT_REAL );
// //    Variable i = newArithmeticVariable( "i", VariableType::VT_INT );
//     Variable b = VariablePool::getInstance().getFreshVariable( "b", VariableType::VT_BOOL );
// //    Sort sortS = newSort( "S" );
// //    Sort sortT = newSort( "T" );
// //    Variable u = VariableNamePool::getInstance().newUninterpretedVariable( "u" );
// //    Variable v = VariableNamePool::getInstance().newUninterpretedVariable( "v" );
// //    UVariable uu( u, sortS );
// //    UVariable uv( v, sortT );

//     // Next we see an example how to create polynomials, which form the left-hand sides of the constraints:
//     Pol px( x );
//     Pol py( y );
//     Pol lhsA = px.pow(2) - py;
//     Pol lhsB = Rational(4) * px + py - Rational(8) * py.pow(7);
    
//     // Constraints can then be constructed as follows:
//     const Constr* constraintA = newConstraint<Pol>( lhsA, Relation::LESS );
    
//     // Uninterpreted functions are 
    
//     // Now, we can construct the atoms of the Boolean Ast
//     const Formula<Pol> atomA = Formula<Pol>( constraintA );
//     const Formula<Pol> atomB = Formula<Pol>( lhsB, Relation::EQ );
//     const Formula<Pol> atomC = Formula<Pol>( b );
    
//     // and the Ast itself:
//     std::set<Formula<Pol>> subAstsA;
//     subAstsA.insert( Formula<Pol>( FormulaType::NOT, atomC ) );
//     subAstsA.insert( atomA );
//     subAstsA.insert( atomB );
//     const Formula<Pol> phiA( FormulaType::AND, subAstsA );
//     const Formula<Pol> phiC( FormulaType::OR, Formula<Pol>( FormulaType::NOT, atomA ), atomC );
//     const Formula<Pol> phiE( FormulaType::IMPLIES, phiA, phiC );
//     std::cout << phiE << std::endl;
// }
