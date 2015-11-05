#include "gtest/gtest.h"
#include "../../carl/core/VariablePool.h"
#include "../../carl/formula/Formula.h"
#include "../../carl/util/stringparser.h"

#include "../Common.h"

using namespace carl;

typedef MultivariatePolynomial<Rational> Pol;
typedef Constraint<Pol> Constr;
typedef Formula<Pol> FormulaT;

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
    subAstsA.push_back( Formula<Pol>( FormulaType::NOT, atomC ) );
    subAstsA.push_back( atomA );
    subAstsA.push_back( atomB );
    const Formula<Pol> phiA( FormulaType::AND, subAstsA );
    const Formula<Pol> phiC( FormulaType::OR, Formula<Pol>( FormulaType::NOT, atomA ), atomC );
    const Formula<Pol> phiE( FormulaType::IMPLIES, phiA, phiC );
    std::cout << phiE << std::endl;
}

TEST(Formula, BooleanConstructors)
{
    Variable b1 = freshBooleanVariable("b1");
    Variable b2 = freshBooleanVariable("b2");

    FormulaT True = FormulaT(FormulaType::TRUE);
    EXPECT_TRUE(True.isTrue());
    FormulaT False = FormulaT(FormulaType::FALSE);
    EXPECT_TRUE(False.isFalse());

    FormulaT fb1 = FormulaT(b1);
    EXPECT_EQ(FormulaType::BOOL, fb1.getType());
    FormulaT fb2 = FormulaT(b2);
    EXPECT_EQ(FormulaType::BOOL, fb2.getType());

    FormulaT nb1 = FormulaT(FormulaType::NOT, fb1);
    EXPECT_EQ(FormulaType::NOT, nb1.getType());

    FormulaT Fimpl = FormulaT(FormulaType::IMPLIES, nb1, fb2);
    FormulaT Fand = FormulaT(FormulaType::AND, nb1, fb2);
    FormulaT For = FormulaT(FormulaType::OR, nb1, fb2);
    FormulaT Fxor = FormulaT(FormulaType::XOR, nb1, fb2);
    FormulaT Fiff = FormulaT(FormulaType::IFF, nb1, fb2);
}

TEST(Formula, FormulaPoolDestructor)
{
    using carl::VariableType;
    carl::Variable b1 = freshBooleanVariable("b1");
    carl::Variable b2 = freshBooleanVariable("b2");
    FormulaT test(AND, FormulaT(b1), FormulaT(b2));
}

TEST(Formula, ANDConstruction)
{
    FormulaT a( VariablePool::getInstance().getFreshVariable( "a", VariableType::VT_BOOL ) );
    FormulaT na( FormulaType::NOT, a );
    FormulaT b( VariablePool::getInstance().getFreshVariable( "b", VariableType::VT_BOOL ) );
    FormulaT nb( FormulaType::NOT, b );
    FormulaT t( FormulaType::TRUE );
    FormulaT f( FormulaType::FALSE );
	EXPECT_EQ(FormulaT(FormulaType::AND, std::initializer_list<Formula<Pol>>{}), t);
	EXPECT_EQ(FormulaT(FormulaType::AND, std::initializer_list<Formula<Pol>>{f}), f);
	EXPECT_EQ(FormulaT(FormulaType::AND, std::initializer_list<Formula<Pol>>{t}), t);
	EXPECT_EQ(FormulaT(FormulaType::AND, std::initializer_list<Formula<Pol>>{a}), a);
    EXPECT_EQ( FormulaT( FormulaType::AND, {f, f} ), f );
    EXPECT_EQ( FormulaT( FormulaType::AND, {t, t} ), t );
    EXPECT_EQ( FormulaT( FormulaType::AND, {t, f} ), f );
    EXPECT_EQ( FormulaT( FormulaType::AND, {f, t} ), f );
    EXPECT_EQ( FormulaT( FormulaType::AND, {t, a} ), a );
    EXPECT_EQ( FormulaT( FormulaType::AND, {a, f} ), f );
    EXPECT_EQ( FormulaT( FormulaType::AND, {na, a} ), f );
    EXPECT_EQ( FormulaT( FormulaType::AND, {a, na} ), f );
    EXPECT_EQ( FormulaT( FormulaType::AND, {f, f, t} ), f );
    EXPECT_EQ( FormulaT( FormulaType::AND, {f, t, f} ), f );
    EXPECT_EQ( FormulaT( FormulaType::AND, {t, f, f} ), f );
    EXPECT_EQ( FormulaT( FormulaType::AND, {t, t, f} ), f );
    EXPECT_EQ( FormulaT( FormulaType::AND, {t, f, t} ), f );
    EXPECT_EQ( FormulaT( FormulaType::AND, {f, t, t} ), f );
    EXPECT_EQ( FormulaT( FormulaType::AND, {t, t, t} ), t );
    EXPECT_EQ( FormulaT( FormulaType::AND, {f, f, f} ), f );
    EXPECT_EQ( FormulaT( FormulaType::AND, {f, t, a} ), f );
    EXPECT_EQ( FormulaT( FormulaType::AND, {t, f, a} ), f );
    EXPECT_EQ( FormulaT( FormulaType::AND, {t, a, f} ), f );
    EXPECT_EQ( FormulaT( FormulaType::AND, {a, f, t} ), f );
    EXPECT_EQ( FormulaT( FormulaType::AND, {f, f, a} ), f );
    EXPECT_EQ( FormulaT( FormulaType::AND, {t, t, a} ), a );
    EXPECT_EQ( FormulaT( FormulaType::AND, {t, a, t} ), a );
    EXPECT_EQ( FormulaT( FormulaType::AND, {a, t, t} ), a );
    EXPECT_EQ( FormulaT( FormulaType::AND, {a, a, a} ), a );
    EXPECT_EQ( FormulaT( FormulaType::AND, {a, t, b, a, t, nb, a} ), f );
}

TEST(Formula, ORConstruction)
{
    FormulaT a( VariablePool::getInstance().getFreshVariable( "a", VariableType::VT_BOOL ) );
    FormulaT na( FormulaType::NOT, a );
    FormulaT b( VariablePool::getInstance().getFreshVariable( "b", VariableType::VT_BOOL ) );
    FormulaT nb( FormulaType::NOT, b );
    FormulaT t( FormulaType::TRUE );
    FormulaT f( FormulaType::FALSE );
	EXPECT_EQ(FormulaT(FormulaType::OR, std::initializer_list<Formula<Pol>>{}), f);
	EXPECT_EQ(FormulaT(FormulaType::OR, std::initializer_list<Formula<Pol>>{f}), f);
	EXPECT_EQ(FormulaT(FormulaType::OR, std::initializer_list<Formula<Pol>>{t}), t);
	EXPECT_EQ(FormulaT(FormulaType::OR, std::initializer_list<Formula<Pol>>{a}), a);
    EXPECT_EQ( FormulaT( FormulaType::OR, {f, f} ), f );
    EXPECT_EQ( FormulaT( FormulaType::OR, {t, t} ), t );
    EXPECT_EQ( FormulaT( FormulaType::OR, {t, f} ), t );
    EXPECT_EQ( FormulaT( FormulaType::OR, {f, t} ), t );
    EXPECT_EQ( FormulaT( FormulaType::OR, {t, a} ), t );
    EXPECT_EQ( FormulaT( FormulaType::OR, {a, f} ), a );
    EXPECT_EQ( FormulaT( FormulaType::OR, {na, a} ), t );
    EXPECT_EQ( FormulaT( FormulaType::OR, {a, na} ), t );
    EXPECT_EQ( FormulaT( FormulaType::OR, {f, f, t} ), t );
    EXPECT_EQ( FormulaT( FormulaType::OR, {f, t, f} ), t );
    EXPECT_EQ( FormulaT( FormulaType::OR, {t, f, f} ), t );
    EXPECT_EQ( FormulaT( FormulaType::OR, {t, t, f} ), t );
    EXPECT_EQ( FormulaT( FormulaType::OR, {t, f, t} ), t );
    EXPECT_EQ( FormulaT( FormulaType::OR, {f, t, t} ), t );
    EXPECT_EQ( FormulaT( FormulaType::OR, {t, t, t} ), t );
    EXPECT_EQ( FormulaT( FormulaType::OR, {f, f, f} ), f );
    EXPECT_EQ( FormulaT( FormulaType::OR, {f, t, a} ), t );
    EXPECT_EQ( FormulaT( FormulaType::OR, {t, f, a} ), t );
    EXPECT_EQ( FormulaT( FormulaType::OR, {t, a, f} ), t );
    EXPECT_EQ( FormulaT( FormulaType::OR, {a, f, t} ), t );
    EXPECT_EQ( FormulaT( FormulaType::OR, {f, f, a} ), a );
    EXPECT_EQ( FormulaT( FormulaType::OR, {t, t, a} ), t );
    EXPECT_EQ( FormulaT( FormulaType::OR, {t, a, t} ), t );
    EXPECT_EQ( FormulaT( FormulaType::OR, {a, t, t} ), t );
    EXPECT_EQ( FormulaT( FormulaType::OR, {a, a, a} ), a );
    EXPECT_EQ( FormulaT( FormulaType::OR, {a, t, b, a, t, nb, a} ), t );
}

TEST(Formula, XORConstruction)
{
    FormulaT a( VariablePool::getInstance().getFreshVariable( "a", VariableType::VT_BOOL ) );
    FormulaT na( FormulaType::NOT, a );
    FormulaT b( VariablePool::getInstance().getFreshVariable( "b", VariableType::VT_BOOL ) );
    FormulaT nb( FormulaType::NOT, b );
    FormulaT t( FormulaType::TRUE );
    FormulaT f( FormulaType::FALSE );
	EXPECT_EQ(FormulaT(FormulaType::XOR, std::initializer_list<Formula<Pol>>{}), f);
	EXPECT_EQ(FormulaT(FormulaType::XOR, std::initializer_list<Formula<Pol>>{f}), f);
	EXPECT_EQ(FormulaT(FormulaType::XOR, std::initializer_list<Formula<Pol>>{t}), t);
	EXPECT_EQ(FormulaT(FormulaType::XOR, std::initializer_list<Formula<Pol>>{a}), a);
    EXPECT_EQ( FormulaT( FormulaType::XOR, {f, f} ), f );
    EXPECT_EQ( FormulaT( FormulaType::XOR, {t, t} ), f );
    EXPECT_EQ( FormulaT( FormulaType::XOR, {t, f} ), t );
    EXPECT_EQ( FormulaT( FormulaType::XOR, {f, t} ), t );
    EXPECT_EQ( FormulaT( FormulaType::XOR, {t, a} ), na );
    EXPECT_EQ( FormulaT( FormulaType::XOR, {a, f} ), a );
    EXPECT_EQ( FormulaT( FormulaType::XOR, {na, a} ), t );
    EXPECT_EQ( FormulaT( FormulaType::XOR, {a, na} ), t );
    EXPECT_EQ( FormulaT( FormulaType::XOR, {f, f, t} ), t );
    EXPECT_EQ( FormulaT( FormulaType::XOR, {f, t, f} ), t );
    EXPECT_EQ( FormulaT( FormulaType::XOR, {t, f, f} ), t );
    EXPECT_EQ( FormulaT( FormulaType::XOR, {t, t, f} ), f );
    EXPECT_EQ( FormulaT( FormulaType::XOR, {t, f, t} ), f );
    EXPECT_EQ( FormulaT( FormulaType::XOR, {f, t, t} ), f );
    EXPECT_EQ( FormulaT( FormulaType::XOR, {t, t, t} ), t );
    EXPECT_EQ( FormulaT( FormulaType::XOR, {f, f, f} ), f );
    EXPECT_EQ( FormulaT( FormulaType::XOR, {f, t, a} ), na );
    EXPECT_EQ( FormulaT( FormulaType::XOR, {t, f, a} ), na );
    EXPECT_EQ( FormulaT( FormulaType::XOR, {t, a, f} ), na );
    EXPECT_EQ( FormulaT( FormulaType::XOR, {a, f, t} ), na );
    EXPECT_EQ( FormulaT( FormulaType::XOR, {f, f, a} ), a );
    EXPECT_EQ( FormulaT( FormulaType::XOR, {t, t, a} ), a );
    EXPECT_EQ( FormulaT( FormulaType::XOR, {t, a, t} ), a );
    EXPECT_EQ( FormulaT( FormulaType::XOR, {a, f, f} ), a );
    EXPECT_EQ( FormulaT( FormulaType::XOR, {a, a, a} ), a );
    EXPECT_EQ( FormulaT( FormulaType::XOR, {t, f, b, f, t, f, na, a, t, nb} ), t );
}

TEST(Formula, IFFConstruction)
{
    FormulaT a( VariablePool::getInstance().getFreshVariable( "a", VariableType::VT_BOOL ) );
    FormulaT na( FormulaType::NOT, a );
    FormulaT b( VariablePool::getInstance().getFreshVariable( "b", VariableType::VT_BOOL ) );
    FormulaT nb( FormulaType::NOT, b );
    FormulaT t( FormulaType::TRUE );
    FormulaT f( FormulaType::FALSE );
	EXPECT_EQ(FormulaT(FormulaType::IFF, std::initializer_list<Formula<Pol>>{}), t);
	EXPECT_EQ(FormulaT(FormulaType::IFF, std::initializer_list<Formula<Pol>>{f}), f);
	EXPECT_EQ(FormulaT(FormulaType::IFF, std::initializer_list<Formula<Pol>>{t}), t);
	EXPECT_EQ(FormulaT(FormulaType::IFF, std::initializer_list<Formula<Pol>>{a}), a);
    EXPECT_EQ( FormulaT( FormulaType::IFF, {f, f} ), t );
    EXPECT_EQ( FormulaT( FormulaType::IFF, {t, t} ), t );
    EXPECT_EQ( FormulaT( FormulaType::IFF, {t, f} ), f );
    EXPECT_EQ( FormulaT( FormulaType::IFF, {f, t} ), f );
    EXPECT_EQ( FormulaT( FormulaType::IFF, {f, a} ), na );
    EXPECT_EQ( FormulaT( FormulaType::IFF, {a, t} ), a );
    EXPECT_EQ( FormulaT( FormulaType::IFF, {f, f, t} ), f );
    EXPECT_EQ( FormulaT( FormulaType::IFF, {f, t, f} ), f );
    EXPECT_EQ( FormulaT( FormulaType::IFF, {t, f, f} ), f );
    EXPECT_EQ( FormulaT( FormulaType::IFF, {t, t, f} ), f );
    EXPECT_EQ( FormulaT( FormulaType::IFF, {t, f, t} ), f );
    EXPECT_EQ( FormulaT( FormulaType::IFF, {f, t, t} ), f );
    EXPECT_EQ( FormulaT( FormulaType::IFF, {t, t, t} ), t );
    EXPECT_EQ( FormulaT( FormulaType::IFF, {f, f, f} ), t );
    EXPECT_EQ( FormulaT( FormulaType::IFF, {f, t, a} ), f );
    EXPECT_EQ( FormulaT( FormulaType::IFF, {t, f, a} ), f );
    EXPECT_EQ( FormulaT( FormulaType::IFF, {t, a, f} ), f );
    EXPECT_EQ( FormulaT( FormulaType::IFF, {a, f, t} ), f );
    EXPECT_EQ( FormulaT( FormulaType::IFF, {f, f, a} ), na );
    EXPECT_EQ( FormulaT( FormulaType::IFF, {t, t, a} ), a );
    EXPECT_EQ( FormulaT( FormulaType::IFF, {t, a, t} ), a );
    EXPECT_EQ( FormulaT( FormulaType::IFF, {a, f, f} ), na );
    EXPECT_EQ( FormulaT( FormulaType::IFF, {a, a, a} ), t );
    EXPECT_EQ( FormulaT( FormulaType::IFF, {t, f, b, f, t, f, na, a, t, nb} ), f );
}

TEST(Formula, ConstraintConstruction)
{
    Variable x = VariablePool::getInstance().getFreshVariable( "x", VariableType::VT_REAL );
    Pol px( x );
    EXPECT_EQ( FormulaT( Constr( x, carl::Relation::GEQ, Rational(1) ) ), FormulaT( Constr( -px+Rational(1), carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( x, carl::Relation::GEQ, -Rational(1) ) ), FormulaT( Constr( -px-Rational(1), carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( x, carl::Relation::GEQ, Rational(0) ) ), FormulaT( Constr( -px, carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( -px-Rational(1), carl::Relation::GEQ ) ), FormulaT( Constr( px+Rational(1), carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( -px+Rational(1), carl::Relation::GEQ ) ), FormulaT( Constr( px-Rational(1), carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( -px, carl::Relation::GEQ ) ), FormulaT( Constr( px, carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( x, carl::Relation::GREATER, Rational(1) ) ), FormulaT( Constr( -px+Rational(1), carl::Relation::LESS ) ) );
    EXPECT_EQ( FormulaT( Constr( x, carl::Relation::GREATER, -Rational(1) ) ), FormulaT( Constr( -px-Rational(1), carl::Relation::LESS ) ) );
    EXPECT_EQ( FormulaT( Constr( x, carl::Relation::GREATER, Rational(0) ) ), FormulaT( Constr( -px, carl::Relation::LESS ) ) );
    EXPECT_EQ( FormulaT( Constr( -px-Rational(1), carl::Relation::GREATER ) ), FormulaT( Constr( px+Rational(1), carl::Relation::LESS ) ) );
    EXPECT_EQ( FormulaT( Constr( -px+Rational(1), carl::Relation::GREATER ) ), FormulaT( Constr( px-Rational(1), carl::Relation::LESS ) ) );
    EXPECT_EQ( FormulaT( Constr( -px, carl::Relation::GREATER ) ), FormulaT( Constr( px, carl::Relation::LESS ) ) );
    
    Variable i = VariablePool::getInstance().getFreshVariable( "i", VariableType::VT_INT );
    Pol pi( i );
    EXPECT_EQ( FormulaT( Constr( i, carl::Relation::GEQ, Rational(1) ) ), FormulaT( Constr( -pi+Rational(1), carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( i, carl::Relation::GEQ, -Rational(1) ) ), FormulaT( Constr( -pi-Rational(1), carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( i, carl::Relation::GEQ, Rational(0) ) ), FormulaT( Constr( -pi, carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( -pi-Rational(1), carl::Relation::GEQ ) ), FormulaT( Constr( pi+Rational(1), carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( -pi+Rational(1), carl::Relation::GEQ ) ), FormulaT( Constr( pi-Rational(1), carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( -pi, carl::Relation::GEQ ) ), FormulaT( Constr( pi, carl::Relation::LEQ ) ) );
    
    EXPECT_EQ( FormulaT( Constr( i, carl::Relation::GREATER, Rational(1) ) ), FormulaT( Constr( -pi+Rational(1), carl::Relation::LESS ) ) );
    EXPECT_EQ( FormulaT( Constr( i, carl::Relation::GREATER, -Rational(1) ) ), FormulaT( Constr( -pi-Rational(1), carl::Relation::LESS ) ) );
    EXPECT_EQ( FormulaT( Constr( i, carl::Relation::GREATER, Rational(0) ) ), FormulaT( Constr( -pi, carl::Relation::LESS ) ) );
    EXPECT_EQ( FormulaT( Constr( -pi-Rational(1), carl::Relation::GREATER ) ), FormulaT( Constr( pi+Rational(1), carl::Relation::LESS ) ) );
    EXPECT_EQ( FormulaT( Constr( -pi+Rational(1), carl::Relation::GREATER ) ), FormulaT( Constr( pi-Rational(1), carl::Relation::LESS ) ) );
    EXPECT_EQ( FormulaT( Constr( -pi, carl::Relation::GREATER ) ), FormulaT( Constr( pi, carl::Relation::LESS ) ) );
    
    EXPECT_EQ( FormulaT( Constr( i, carl::Relation::LEQ, Rational(1)/Rational(2) ) ), FormulaT( Constr( pi, carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( i, carl::Relation::LEQ, Rational(3)/Rational(2) ) ), FormulaT( Constr( pi-Rational(1), carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( i, carl::Relation::LEQ, -Rational(1)/Rational(2) ) ), FormulaT( Constr( pi+Rational(1), carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( i, carl::Relation::LEQ, -Rational(3)/Rational(2) ) ), FormulaT( Constr( pi+Rational(2), carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( -pi-Rational(1)/Rational(2), carl::Relation::LEQ ) ), FormulaT( Constr( -pi, carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( -pi-Rational(3)/Rational(2), carl::Relation::LEQ ) ), FormulaT( Constr( -pi-Rational(1), carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( -pi+Rational(1)/Rational(2), carl::Relation::LEQ ) ), FormulaT( Constr( -pi+Rational(1), carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( -pi+Rational(3)/Rational(2), carl::Relation::LEQ ) ), FormulaT( Constr( -pi+Rational(2), carl::Relation::LEQ ) ) );
    
    EXPECT_EQ( FormulaT( Constr( i, carl::Relation::LESS, Rational(1) ) ), FormulaT( Constr( pi, carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( i, carl::Relation::LESS, Rational(0) ) ), FormulaT( Constr( pi+Rational(1), carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( i, carl::Relation::LESS, -Rational(1) ) ), FormulaT( Constr( pi+Rational(2), carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( i, carl::Relation::LESS, Rational(1)/Rational(2) ) ), FormulaT( Constr( pi, carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( i, carl::Relation::LESS, Rational(3)/Rational(2) ) ), FormulaT( Constr( pi-Rational(1), carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( i, carl::Relation::LESS, -Rational(1)/Rational(2) ) ), FormulaT( Constr( pi+Rational(1), carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( i, carl::Relation::LESS, -Rational(3)/Rational(2) ) ), FormulaT( Constr( pi+Rational(2), carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( -pi-Rational(1), carl::Relation::LESS ) ), FormulaT( Constr( -pi, carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( -pi, carl::Relation::LESS ) ), FormulaT( Constr( -pi+Rational(1), carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( -pi+Rational(1), carl::Relation::LESS ) ), FormulaT( Constr( -pi+Rational(2), carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( -pi-Rational(1)/Rational(2), carl::Relation::LESS ) ), FormulaT( Constr( -pi, carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( -pi+Rational(1)/Rational(2), carl::Relation::LESS ) ), FormulaT( Constr( -pi+Rational(1), carl::Relation::LEQ ) ) );
    
    EXPECT_EQ( FormulaT( Constr( i, carl::Relation::GEQ, Rational(1)/Rational(2) ) ), FormulaT( Constr( -pi+Rational(1), carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( i, carl::Relation::GEQ, Rational(3)/Rational(2) ) ), FormulaT( Constr( -pi+Rational(2), carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( i, carl::Relation::GEQ, -Rational(1)/Rational(2) ) ), FormulaT( Constr( -pi, carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( i, carl::Relation::GEQ, -Rational(3)/Rational(2) ) ), FormulaT( Constr( -pi-Rational(1), carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( -pi-Rational(1)/Rational(2), carl::Relation::GEQ ) ), FormulaT( Constr( pi+Rational(1), carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( -pi-Rational(3)/Rational(2), carl::Relation::GEQ ) ), FormulaT( Constr( pi+Rational(2), carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( -pi+Rational(1)/Rational(2), carl::Relation::GEQ ) ), FormulaT( Constr( pi, carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( -pi+Rational(3)/Rational(2), carl::Relation::GEQ ) ), FormulaT( Constr( pi-Rational(1), carl::Relation::LEQ ) ) );
    
    EXPECT_EQ( FormulaT( Constr( i, carl::Relation::GREATER, Rational(1) ) ), FormulaT( Constr( -pi+Rational(2), carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( i, carl::Relation::GREATER, Rational(0) ) ), FormulaT( Constr( -pi+Rational(1), carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( i, carl::Relation::GREATER, -Rational(1) ) ), FormulaT( Constr( -pi, carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( i, carl::Relation::GREATER, Rational(1)/Rational(2) ) ), FormulaT( Constr( -pi+Rational(1), carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( i, carl::Relation::GREATER, Rational(3)/Rational(2) ) ), FormulaT( Constr( -pi+Rational(2), carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( i, carl::Relation::GREATER, -Rational(1)/Rational(2) ) ), FormulaT( Constr( -pi, carl::Relation::LEQ ) ) );
    EXPECT_EQ( FormulaT( Constr( i, carl::Relation::GREATER, -Rational(3)/Rational(2) ) ), FormulaT( Constr( -pi-Rational(1), carl::Relation::LEQ ) ) );
}
