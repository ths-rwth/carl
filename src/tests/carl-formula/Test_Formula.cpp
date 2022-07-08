#include <gtest/gtest.h>
#include <carl-arith/core/VariablePool.h>
#include <carl-formula/formula/Formula.h>
#include <carl-io/StringParser.h>

#include "../Common.h"

using namespace carl;

typedef MultivariatePolynomial<Rational> Pol;
typedef Constraint<Pol> Constr;
typedef Formula<Pol> FormulaT;

TEST(Formula, Construction)
{
    Variable x = fresh_real_variable("x");
    Variable y = fresh_real_variable("y");
    Variable i1 = fresh_integer_variable("i1");
    Variable i2 = fresh_integer_variable("i2");
    Variable i3 = fresh_integer_variable("i3");
//    Variable i = newArithmeticVariable( "i", VariableType::VT_INT );
    Variable b = fresh_boolean_variable("b");
//    Sort sortS = newSort( "S" );
//    Sort sortT = newSort( "T" );
//    Variable u = VariableNamePool::getInstance().newUninterpretedVariable( "u" );
//    Variable v = VariableNamePool::getInstance().newUninterpretedVariable( "v" );
//    UVariable uu( u, sortS );
//    UVariable uv( v, sortT );

    // Next we see an example how to create polynomials, which form the left-hand sides of the constraints:
    Pol px( x );
    Pol py( y );
    Pol lhsA = carl::pow(px, 2) - py;
    Pol lhsB = Rational(4) * px + py - Rational(8) * carl::pow(py, 7);
    Pol lhsD = px*py;
    Pol pi1( i1 );
    Pol pi2( i2 );
    Pol pi3( i3 );
    Pol lhsC = Rational(2) * pi1 + Rational(2) * pi2 + Rational(2) * pi3 - Rational(5);

    // Constraints can then be constructed as follows:
    Constr constraintA = Constr( lhsD, Relation::EQ );
    EXPECT_EQ( Constr( px-Rational(1), Relation::LEQ ), Constr( x, Relation::LEQ, Rational(1) ) );
    EXPECT_EQ( Constr( px-Rational(1), Relation::LESS ), Constr( x, Relation::LESS, Rational(1) ) );
    EXPECT_EQ( Constr( px-Rational(1), Relation::GEQ ), Constr( x, Relation::GEQ, Rational(1) ) );
    EXPECT_EQ( Constr( px-Rational(1), Relation::GREATER ), Constr( x, Relation::GREATER, Rational(1) ) );
    EXPECT_EQ( Constr( px+Rational(1), Relation::LEQ ), Constr( x, Relation::LEQ, -Rational(1) ) );
    EXPECT_EQ( Constr( px+Rational(1), Relation::LESS ), Constr( x, Relation::LESS, -Rational(1) ) );
    EXPECT_EQ( Constr( px+Rational(1), Relation::GEQ ), Constr( x, Relation::GEQ, -Rational(1) ) );
    EXPECT_EQ( Constr( px+Rational(1), Relation::GREATER ), Constr( x, Relation::GREATER, -Rational(1) ) );
    // Test should be fixed
    EXPECT_EQ( Constr( px+Rational(1), Relation::EQ ), Constr( x, Relation::EQ, -Rational(1) ) );

    // Uninterpreted functions are

    // Now, we can construct the atoms of the Boolean Ast
    FormulaT atomA( constraintA );
    FormulaT atomB( lhsB, Relation::EQ );
    FormulaT atomC( b );
    FormulaT inEq( lhsC, Relation::EQ );
    EXPECT_TRUE( inEq.type() == FormulaType::FALSE );

    // and the Ast itself:
    Formulas<Pol> subAstsA;
    subAstsA.emplace_back( FormulaType::NOT, atomC );
    subAstsA.push_back( atomA );
    subAstsA.push_back( atomB );
    FormulaT phiA( FormulaType::AND, subAstsA );
    FormulaT tsVarA = FormulaPool<Pol>::getInstance().createTseitinVar( phiA );
    FormulaT phiC( FormulaType::OR, {FormulaT( FormulaType::NOT, atomA ), atomC} );
    FormulaT tsVarC = FormulaPool<Pol>::getInstance().createTseitinVar( phiC );
    FormulaT phiE( FormulaType::IMPLIES, {phiA, phiC} );
    FormulaT tsVarE = FormulaPool<Pol>::getInstance().createTseitinVar( phiE );
    FormulaT( FormulaType::XOR, {tsVarA, tsVarC, tsVarE} );
}

TEST(Formula, BooleanConstructors)
{
    Variable b1 = fresh_boolean_variable("b1");
    Variable b2 = fresh_boolean_variable("b2");

    FormulaT True = FormulaT(FormulaType::TRUE);
    EXPECT_TRUE(True.is_true());
    FormulaT False = FormulaT(FormulaType::FALSE);
    EXPECT_TRUE(False.is_false());

    FormulaT fb1 = FormulaT(b1);
    EXPECT_EQ(FormulaType::BOOL, fb1.type());
    FormulaT fb2 = FormulaT(b2);
    EXPECT_EQ(FormulaType::BOOL, fb2.type());

    FormulaT nb1 = FormulaT(FormulaType::NOT, fb1);
    EXPECT_EQ(FormulaType::NOT, nb1.type());

    FormulaT Fimpl = FormulaT(FormulaType::IMPLIES, {nb1, fb2});
    FormulaT Fand = FormulaT(FormulaType::AND, {nb1, fb2});
    FormulaT For = FormulaT(FormulaType::OR, {nb1, fb2});
    FormulaT Fxor = FormulaT(FormulaType::XOR, {nb1, fb2});
    FormulaT Fiff = FormulaT(FormulaType::IFF, {nb1, fb2});
}

TEST(Formula, FormulaPoolDestructor)
{
    using carl::VariableType;
    carl::Variable b1 = fresh_boolean_variable("b1");
    carl::Variable b2 = fresh_boolean_variable("b2");
    FormulaT test(AND, {FormulaT(b1), FormulaT(b2)});
}

TEST(Formula, ANDConstruction)
{
    FormulaT a( fresh_boolean_variable("a") );
    FormulaT na( FormulaType::NOT, a );
    FormulaT b( fresh_boolean_variable("b") );
    FormulaT nb( FormulaType::NOT, b );
    FormulaT t( FormulaType::TRUE );
    FormulaT f( FormulaType::FALSE );
	EXPECT_EQ(FormulaT(FormulaType::AND, std::initializer_list<FormulaT>{}), t);
	EXPECT_EQ(FormulaT(FormulaType::AND, std::initializer_list<FormulaT>{f}), f);
	EXPECT_EQ(FormulaT(FormulaType::AND, std::initializer_list<FormulaT>{t}), t);
	EXPECT_EQ(FormulaT(FormulaType::AND, std::initializer_list<FormulaT>{a}), a);
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
    FormulaT a( fresh_boolean_variable("a") );
    FormulaT na( FormulaType::NOT, a );
    FormulaT b( fresh_boolean_variable("b") );
    FormulaT nb( FormulaType::NOT, b );
    FormulaT t( FormulaType::TRUE );
    FormulaT f( FormulaType::FALSE );
	EXPECT_EQ(FormulaT(FormulaType::OR, std::initializer_list<FormulaT>{}), f);
	EXPECT_EQ(FormulaT(FormulaType::OR, std::initializer_list<FormulaT>{f}), f);
	EXPECT_EQ(FormulaT(FormulaType::OR, std::initializer_list<FormulaT>{t}), t);
	EXPECT_EQ(FormulaT(FormulaType::OR, std::initializer_list<FormulaT>{a}), a);
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
    FormulaT a( fresh_boolean_variable("a") );
    FormulaT na( FormulaType::NOT, a );
    FormulaT b( fresh_boolean_variable("b") );
    FormulaT nb( FormulaType::NOT, b );
    FormulaT t( FormulaType::TRUE );
    FormulaT f( FormulaType::FALSE );
	EXPECT_EQ(FormulaT(FormulaType::XOR, std::initializer_list<FormulaT>{}), f);
	EXPECT_EQ(FormulaT(FormulaType::XOR, std::initializer_list<FormulaT>{f}), f);
	EXPECT_EQ(FormulaT(FormulaType::XOR, std::initializer_list<FormulaT>{t}), t);
	EXPECT_EQ(FormulaT(FormulaType::XOR, std::initializer_list<FormulaT>{a}), a);
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
    FormulaT a( fresh_boolean_variable("a") );
    FormulaT na( FormulaType::NOT, a );
    FormulaT b( fresh_boolean_variable("b") );
    FormulaT nb( FormulaType::NOT, b );
    FormulaT t( FormulaType::TRUE );
    FormulaT f( FormulaType::FALSE );
	EXPECT_EQ(FormulaT(FormulaType::IFF, std::initializer_list<FormulaT>{}), t);
	EXPECT_EQ(FormulaT(FormulaType::IFF, std::initializer_list<FormulaT>{f}), f);
	EXPECT_EQ(FormulaT(FormulaType::IFF, std::initializer_list<FormulaT>{t}), t);
	EXPECT_EQ(FormulaT(FormulaType::IFF, std::initializer_list<FormulaT>{a}), a);
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
    Variable x = fresh_real_variable("x");
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

    Variable i = fresh_integer_variable("i");
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
    
    // check if the integer normalization works
    Variable j = fresh_integer_variable("j");
    Pol pj( j );
    EXPECT_EQ( Constr( pi+pj-Rational(3), carl::Relation::LESS ).relation(), carl::Relation::LEQ );
    EXPECT_EQ( Constr( Rational(9)*pi+Rational(18)*pj-Rational(4), carl::Relation::GEQ ), Constr( pi+Rational(2)*pj-Rational(1), carl::Relation::GEQ ) );
    EXPECT_EQ( Constr( Rational(9)*pi+Rational(18)*pj-Rational(5), carl::Relation::LEQ ), Constr( pi+Rational(2)*pj, carl::Relation::LEQ ) );
    EXPECT_EQ( Constr( Rational(3)*pi+Rational(6)*pj, carl::Relation::GREATER ), Constr( pi+Rational(2)*pj-Rational(1), carl::Relation::GEQ ) );
    EXPECT_EQ( Constr( Rational(3)*pi+Rational(6)*pj-Rational(3), carl::Relation::LESS ), Constr( pi+Rational(2)*pj, carl::Relation::LEQ ) );
}

TEST(Formula, ConstraintSimplication)
{
    Variable x = fresh_real_variable("x");
    {
        FormulaT ref(Pol(x), Relation::LESS);
        EXPECT_EQ(ref, FormulaT(-Pol(x), Relation::GREATER));
        EXPECT_EQ(ref, FormulaT(FormulaType::NOT, FormulaT(Pol(x), Relation::GEQ)));
        EXPECT_EQ(ref, FormulaT(FormulaType::NOT, FormulaT(-Pol(x), Relation::LEQ)));
    }
    {
        FormulaT ref(-Pol(x), Relation::LEQ);
        EXPECT_EQ(ref, FormulaT(Pol(x), Relation::GEQ));
        EXPECT_EQ(ref, FormulaT(FormulaType::NOT, FormulaT(Pol(x), Relation::LESS)));
        EXPECT_EQ(ref, FormulaT(FormulaType::NOT, FormulaT(-Pol(x), Relation::GREATER)));
    }
}

TEST(Formula, gatherVariables)
{
	Variable x = fresh_real_variable("x");
	Variable y = fresh_real_variable("y");
	Variable z = fresh_real_variable("z");
	{
		carlVariables vars;
		FormulaT f(Pol(x), Relation::LESS);
        variables(f,vars);
		EXPECT_EQ(vars, carlVariables({x}));
	}
	{
		carlVariables vars;
		FormulaT f(Pol(x)+y, Relation::LESS);
		variables(f,vars);
		EXPECT_EQ(vars, carlVariables({x,y}));
	}
	{
		carlVariables vars;
		FormulaT f(Pol(x)*y+z, Relation::LESS);
		variables(f,vars);
		EXPECT_EQ(vars, carlVariables({x,y,z}));
	}
}

TEST(Formula, Uniqueness)
{
	//(X !> (IR ]-594743/343, -1189485/686[, __r^2 + -1031250000/343 R))
	Variable X = fresh_real_variable("X");
	Variable r = fresh_real_variable("R");
	
	Interval<Rational> ir(Rational(-594743)/343, BoundType::STRICT, Rational(-1189485)/686, BoundType::STRICT);
	UnivariatePolynomial<Rational> up(r, {Rational(-1031250000)/343, Rational(0), Rational(1)});
	IntRepRealAlgebraicNumber<Rational> ran = IntRepRealAlgebraicNumber<Rational>::create_safe(up, ir);
	VariableComparison<Pol> vc(X, ran, Relation::GREATER, true);
	
	FormulaT f1 = FormulaT(vc);
	FormulaT f2 = FormulaT(vc);
	EXPECT_EQ(f1, f2);
}
