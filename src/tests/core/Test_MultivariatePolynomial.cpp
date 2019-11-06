#include "gtest/gtest.h"
#include "carl/core/UnivariatePolynomial.h"
#include "carl/core/polynomialfunctions/Quotient.h"
#include "carl/core/polynomialfunctions/SPolynomial.h"
#include "carl/core/polynomialfunctions/to_univariate_polynomial.h"
#include "carl/core/VariablePool.h"
#include "carl/interval/Interval.h"
#include <list>
#include "carl/converter/OldGinacConverter.h"
#include "carl/util/stringparser.h"
#include "carl/util/platform.h"

#include "../Common.h"

using namespace carl;

template<typename T>
class MultivariatePolynomialTest: public testing::Test {};

TYPED_TEST_CASE(MultivariatePolynomialTest, NumberTypes);

TYPED_TEST(MultivariatePolynomialTest, Constructor)
{
	Variable v0 = carl::freshRealVariable("x");
    Term<TypeParam> t0(v0);
    MultivariatePolynomial<TypeParam> p0(t0);
	
    MultivariatePolynomial<TypeParam> p1(TypeParam(3));
    EXPECT_EQ(1, p1.nrTerms());
    EXPECT_TRUE(p1.isLinear());
}

TYPED_TEST(MultivariatePolynomialTest, Operators)
{
	Variable v0 = carl::freshRealVariable("x");
    Term<TypeParam> t0(v0);
    MultivariatePolynomial<TypeParam> p0a(t0);
    MultivariatePolynomial<TypeParam> p0b(v0);
    EXPECT_EQ(p0a, p0b);

    EXPECT_TRUE(p0a.isUnivariate());
}

TYPED_TEST(MultivariatePolynomialTest, getTerms)
{
	Variable v0 = carl::freshRealVariable("x");
	MultivariatePolynomial<TypeParam> p;
	p += TypeParam(1);
	p += v0;
	p *= v0;
	MultivariatePolynomial<TypeParam> p2 = p + TypeParam(1);

	auto& t = p2.getTerms();
	t.erase(t.begin());

	EXPECT_EQ(p, p2);
}

TEST(MultivariatePolynomial, remainder)
{
	Variable x = carl::freshRealVariable("x");
	Variable y = carl::freshRealVariable("y");
    MultivariatePolynomial<Rational> px( x );
    MultivariatePolynomial<Rational> py( y );
    MultivariatePolynomial<Rational> p( px - Rational(3) );
    MultivariatePolynomial<Rational> p4( p * p * p * p );
    EXPECT_TRUE( carl::isZero(carl::remainder(p4, p)) );
}



TEST(MultivariatePolynomial, to_univariate_polynomial)
{
    Variable x = freshRealVariable("x");
	Variable y = freshRealVariable("y");
	{
		MultivariatePolynomial<Rational> p({(Rational)1*x, (Rational)-1*x*x, (Rational)3*x*x*x});
		UnivariatePolynomial<Rational> res(x, {(Rational)0, (Rational)1, (Rational)-1, (Rational)3});
		ASSERT_EQ(carl::to_univariate_polynomial(p), res);
	}
	{
		MultivariatePolynomial<Rational> p({(Rational)1*x, (Rational)-1*x*x, (Rational)3*x*x*x});
		UnivariatePolynomial<MultivariatePolynomial<Rational>> res(x, {(Rational)0, (Rational)1, (Rational)-1, (Rational)3});
		ASSERT_EQ(carl::to_univariate_polynomial(p, x), res);
	}
	{
		MultivariatePolynomial<Rational> p({(Rational)1*x*y, (Rational)-1*x*x, (Rational)3*x*x*x});
		UnivariatePolynomial<MultivariatePolynomial<Rational>> res(x, {MultivariatePolynomial<Rational>((Rational)0), MultivariatePolynomial<Rational>((Rational)1*y), MultivariatePolynomial<Rational>((Rational)-1), MultivariatePolynomial<Rational>((Rational)3)});
		ASSERT_EQ(res, carl::to_univariate_polynomial(p, x));
	}
}

TYPED_TEST(MultivariatePolynomialTest, Addition)
{
    Variable v0 = freshRealVariable("v0");
    Term<TypeParam> t0(v0);
    MultivariatePolynomial<TypeParam> p0(v0);
    p0 += 3;
    EXPECT_EQ((unsigned)2, p0.nrTerms());
    p0 += 3;
    EXPECT_EQ((unsigned)2, p0.nrTerms());
    p0 += -6;
    EXPECT_EQ((unsigned)1, p0.nrTerms());

    Variable v1 = freshRealVariable("v1");
    Variable v2 = freshRealVariable("v2");
    p0 += v1;
    p0 += createMonomial(v2, (exponent) 1);
    EXPECT_EQ((unsigned)3,p0.nrTerms());
    p0 += createMonomial(v2, (exponent) 1);
    EXPECT_EQ((unsigned)3,p0.nrTerms());
    p0 += Term<TypeParam>(-2,v2,1);
    EXPECT_EQ((unsigned)2,p0.nrTerms());

    MultivariatePolynomial<TypeParam> p1(v0);
    p1 += v1;
    p0 += p1;
    EXPECT_EQ((unsigned)2,p0.nrTerms());
    MultivariatePolynomial<TypeParam> mp2(v1);
    mp2 += (TypeParam)2 * v0;
    EXPECT_EQ(v1, mp2.lterm());
    MultivariatePolynomial<TypeParam> p10a;
    p10a += v0*v0;
    p10a += v1;
    EXPECT_EQ(v0*v0, p10a.lterm());
    MultivariatePolynomial<TypeParam> p10b;
    p10b += v1;
    p10b += v0*v0;
    EXPECT_EQ(v0*v0, p10b.lterm());
	EXPECT_EQ(p10a, p10b);
    p10a += p10b;
	p10a += mp2;
	p10a += p0;
	p10b += mp2;
	p10b += p0;
	p10b += v0 * v0;
	p10b += v1;
	EXPECT_EQ(p10a, p10b);
}

TYPED_TEST(MultivariatePolynomialTest, Substraction)
{
    Variable v0 = freshRealVariable("v0");
    MultivariatePolynomial<TypeParam> p0(v0);
    p0 -= 3;
    EXPECT_EQ((unsigned)2, p0.nrTerms());
    p0 -= 3;
    EXPECT_EQ((unsigned)2, p0.nrTerms());
    p0 -= -6;
    EXPECT_EQ((unsigned)1, p0.nrTerms());
    Variable v1 = freshRealVariable("v1");
    Variable v2 = freshRealVariable("v2");
    p0 -= v1;
    EXPECT_EQ((unsigned)2,p0.nrTerms());
    p0 -= createMonomial(v2, (exponent) 1);
    EXPECT_EQ((unsigned)3,p0.nrTerms());
    p0 -= createMonomial(v2, (exponent) 1);

    EXPECT_EQ((unsigned)3,p0.nrTerms());
    p0 -= Term<TypeParam>(-2,v2,1);
    EXPECT_EQ((unsigned)2,p0.nrTerms());

    MultivariatePolynomial<TypeParam> p1(v0);
    p1 -= v1;
    p0 -= p1;
    EXPECT_EQ((unsigned)0,p0.nrTerms());
}

TYPED_TEST(MultivariatePolynomialTest, Multiplication)
{
    Variable v0 = freshRealVariable("v0");
    Variable v1 = freshRealVariable("v1");
    MultivariatePolynomial<TypeParam> p0(v0);

    p0 *= v0;
    EXPECT_EQ(Term<TypeParam>((unsigned)1,v0,2), p0.lterm());

    MultivariatePolynomial<TypeParam> q({TypeParam(2)*v0*v0, TypeParam(1)*v0*v1});
    Term<TypeParam> t(1, v0,2);
    MultivariatePolynomial<TypeParam> res = q * t;
    EXPECT_EQ(MultivariatePolynomial<TypeParam>({TypeParam(2)*v0*v0*v0*v0, TypeParam(1)*v0*v0*v0*v1}), res);

    p0 += v1;
    p0 += 1;
    //std::cout << p0 << std::endl;
    p0 *= p0;
    //std::cout << p0 << std::endl;
}

TEST(MultivariatePolynomial, toString)
{

}

TEST(MultivariatePolynomial, Normalize)
{
    Variable v0 = freshRealVariable("v0");
    Variable v1 = freshRealVariable("v1");
    MultivariatePolynomial<Rational> mp(v0);
    MultivariatePolynomial<Rational> mp2 = mp * Rational(2);
    EXPECT_EQ(mp, mp.normalize());
    EXPECT_EQ(mp, mp2.normalize());

    mp = Rational(2) * v0 + Rational(2) * v1;
    mp2 = MultivariatePolynomial<Rational>(v0) + v1;
    EXPECT_EQ(mp2, mp.normalize());

    mp = Rational(4) * v0 + Rational(2) * v1;
    mp2 = Rational(2) * v0 + v1;
    EXPECT_EQ(mp2, mp.normalize());
}

TEST(MultivariatePolynomial, Coprime)
{
    Variable v0 = freshRealVariable("v0");
    Variable v1 = freshRealVariable("v1");
    MultivariatePolynomial<Rational> mp;
    mp += v0;
    MultivariatePolynomial<Rational> mp2 = mp * (Rational)2;
    EXPECT_EQ(mp, mp.coprimeCoefficients());

    mp = MultivariatePolynomial<Rational>((Rational)2 * v0);
    mp += (Rational)4 * v1;
    mp2 = MultivariatePolynomial<Rational>(v0);
    mp2 += (Rational)2 * v1;
    EXPECT_EQ(mp2, mp.coprimeCoefficients());

    mp = MultivariatePolynomial<Rational>(((Rational)1)/(Rational)6 * v0);
    mp += (Rational)4 * v1;
    mp2 = MultivariatePolynomial<Rational>((Rational)1 * v0);
    mp2 += (Rational)24 * v1;
    EXPECT_EQ(mp2, mp.coprimeCoefficients());

    mp = MultivariatePolynomial<Rational>(((Rational)1)/(Rational)6 * v0);
    mp += ((Rational)1)/(Rational)4 * v1;
    mp2 = MultivariatePolynomial<Rational>((Rational)2 * v0);
    mp2 += (Rational)3 * v1;
    EXPECT_EQ(mp2, mp.coprimeCoefficients());

    MultivariatePolynomial<Rational> mpG = MultivariatePolynomial<Rational>(Rational(1)/Rational(6) * v0);
    mpG += Rational(1)/Rational(4) * v1;
    MultivariatePolynomial<Rational> mp2G = MultivariatePolynomial<Rational>(Rational(2) * v0);
    mp2G += Rational(3) * v1;
    EXPECT_EQ(mp2G, mpG.coprimeCoefficients());


}

TEST(MultivariatePolynomial, Substitute)
{
    Variable v0 = freshRealVariable("v0");
    Variable v1 = freshRealVariable("v1");
	{
		MultivariatePolynomial<Rational> p(v0);
		p.substituteIn(v0, MultivariatePolynomial<Rational>(-1));
		ASSERT_EQ(p, MultivariatePolynomial<Rational>(-1));
	}
    MultivariatePolynomial<Rational> mp((Rational)1/(Rational)6 * v0);
    mp += (Rational)4 * v1;
    MultivariatePolynomial<Rational> mp2((Rational)2);
    mp2 += (Rational)4 * v1;
    std::map<Variable, MultivariatePolynomial<Rational>> substitutions;
    substitutions[v0] = MultivariatePolynomial<Rational>((Rational)12);
    EXPECT_EQ(mp2, mp.substitute(substitutions));
    substitutions[v0] = MultivariatePolynomial<Rational>((Rational)0);
    EXPECT_EQ(MultivariatePolynomial<Rational>((Rational)4 * v1), mp.substitute(substitutions));
    Variable x = freshRealVariable("x");

    #ifdef USE_GINAC
	Variable v = freshRealVariable("v");
	Variable y = freshRealVariable("y");
	Variable z = freshRealVariable("z");
    std::map<GiNaC::ex, Variable, GiNaC::ex_is_less> vars;
    GiNaC::symbol vg("v"), xg("x"), yg("y"), zg("z");
    vars.insert(std::pair<GiNaC::ex, Variable>(vg, v));
    vars.insert(std::pair<GiNaC::ex, Variable>(xg, x));
    vars.insert(std::pair<GiNaC::ex, Variable>(yg, y));
    vars.insert(std::pair<GiNaC::ex, Variable>(zg, z));

    MultivariatePolynomial<Rational> f1({(Rational)1*x*x*v*x*y*y, (Rational)-2*x*x*y*y*y, (Rational)312347*v*v*x, (Rational)3*y*v*z*z*x, Term<Rational>((Rational)1)});
    GiNaC::ex f1g = xg*xg*vg*xg*yg*yg-2*xg*xg*yg*yg*yg+312347*vg*vg*xg+3*yg*vg*zg*zg*xg+1;
    EXPECT_EQ(f1, convertToCarl<MultivariatePolynomial<Rational>>(f1g, vars));
    MultivariatePolynomial<Rational> f2({(Rational)7*x*x*x*x*y*y, (Rational)191*x*x*x*x*z*z*z ,(Rational)-3*x*x*y, (Rational)1*z*z*x*v*v, (Rational)2*z*y*v*v, Term<Rational>((Rational)4)});
    GiNaC::ex f2g = 7*xg*xg*xg*xg*yg*yg+191*xg*xg*xg*xg*zg*zg*zg-3*xg*xg*yg+zg*zg*xg*vg*vg+2*zg*yg*vg*vg+4;
    EXPECT_EQ(f2, convertToCarl<MultivariatePolynomial<Rational>>(f2g, vars));

    MultivariatePolynomial<Rational> sy({(Rational)-2*y*y*y, (Rational)-9*y, Term<Rational>((Rational)15)});
    GiNaC::ex syg = -2*yg*yg*yg-9*yg+15;
    EXPECT_EQ(sy, convertToCarl<MultivariatePolynomial<Rational>>(syg, vars));
    MultivariatePolynomial<Rational> sxy1({(Rational)1*x*y*y, (Rational)-5*y*y*y, Term<Rational>((Rational)3377)});
    GiNaC::ex sxy1g = xg*yg*yg-5*yg*yg*yg+3377;
    EXPECT_EQ(sxy1, convertToCarl<MultivariatePolynomial<Rational>>(sxy1g, vars));
    MultivariatePolynomial<Rational> sx({(Rational)1*x, Term<Rational>((Rational)-1)});
    GiNaC::ex sxg = xg-1;
    EXPECT_EQ(sx, convertToCarl<MultivariatePolynomial<Rational>>(sxg, vars));
    MultivariatePolynomial<Rational> svyz({(Rational)8*v*v*y, (Rational)1*y*z*y, (Rational)29*z*z*z*z*z});
    GiNaC::ex svyzg = 8*vg*vg*yg+yg*zg*yg+29*zg*zg*zg*zg*zg;
    EXPECT_EQ(svyz, convertToCarl<MultivariatePolynomial<Rational>>(svyzg, vars));
    MultivariatePolynomial<Rational> svz({(Rational)1*v*v, (Rational)-1*z, (Rational)-2*v*z, Term<Rational>((Rational)10)});
    GiNaC::ex svzg = vg*vg-zg-2*vg*zg+10;
    EXPECT_EQ(svz, convertToCarl<MultivariatePolynomial<Rational>>(svzg, vars));
    MultivariatePolynomial<Rational> sxy2({(Rational)2*x*y*y});
    GiNaC::ex sxy2g = 2*xg*yg*yg;
    EXPECT_EQ(sxy2, convertToCarl<MultivariatePolynomial<Rational>>(sxy2g, vars));
    MultivariatePolynomial<Rational> sv({(Rational)1*v*v*v*v});
    GiNaC::ex svg = vg*vg*vg*vg;
    EXPECT_EQ(sv, convertToCarl<MultivariatePolynomial<Rational>>(svg, vars));
    MultivariatePolynomial<Rational> sz({(Rational)1*z});
    GiNaC::ex szg = zg;
    EXPECT_EQ(sz, convertToCarl<MultivariatePolynomial<Rational>>(szg, vars));
    MultivariatePolynomial<Rational> sxz({(Rational)8*x*z*z*z, (Rational)-5*x*x*x, Term<Rational>((Rational)7)});
    GiNaC::ex sxzg = 8*xg*zg*zg*zg-5*xg*xg*xg+7;
    EXPECT_EQ(sxz, convertToCarl<MultivariatePolynomial<Rational>>(sxzg, vars));
    MultivariatePolynomial<Rational> sxv({(Rational)3*x, (Rational)1*v*v, (Rational)2*v*v*v*v});
    GiNaC::ex sxvg = 3*xg+vg*vg+2*vg*vg*vg*vg;
    EXPECT_EQ(sxv, convertToCarl<MultivariatePolynomial<Rational>>(sxvg, vars));
    MultivariatePolynomial<Rational> svy({(Rational)13*v*y, (Rational)-3*y*y*y*v, Term<Rational>((Rational)100)});
    GiNaC::ex svyg = 13*vg*yg-3*yg*yg*yg*vg+100;
    EXPECT_EQ(svy, convertToCarl<MultivariatePolynomial<Rational>>(svyg, vars));

    std::map<Variable, MultivariatePolynomial<Rational>> substitutions0;
    substitutions0[v] = sy;
    substitutions0[x] = svy;
    substitutions0[y] = MultivariatePolynomial<Rational>((Rational)0);
    substitutions0[z] = sy;
    GiNaC::exmap substitutions0g;
    substitutions0g[vg] = syg;
    substitutions0g[xg] = svyg;
    substitutions0g[yg] = 0;
    substitutions0g[zg] = syg;
    std::map<Variable, MultivariatePolynomial<Rational>> substitutions1;
    substitutions1[v] = MultivariatePolynomial<Rational>((Rational)0);
    substitutions1[x] = svyz;
    substitutions1[y] = MultivariatePolynomial<Rational>((Rational)1289);
    substitutions1[z] = sxv;
    GiNaC::exmap substitutions1g;
    substitutions1g[vg] = 0;
    substitutions1g[xg] = svyzg;
    substitutions1g[yg] = 1289;
    substitutions1g[zg] = sxvg;
    std::map<Variable, MultivariatePolynomial<Rational>> substitutions2;
    substitutions2[v] = sx;
    substitutions2[x] = sv;
    substitutions2[y] = sxz;
    GiNaC::exmap substitutions2g;
    substitutions2g[vg] = sxg;
    substitutions2g[xg] = svg;
    substitutions2g[yg] = sxzg;
    std::map<Variable, MultivariatePolynomial<Rational>> substitutions3;
    substitutions3[v] = sxy1;
    substitutions3[x] = svz;
    substitutions3[y] = MultivariatePolynomial<Rational>((Rational)0);
    GiNaC::exmap substitutions3g;
    substitutions3g[vg] = sxy1g;
    substitutions3g[xg] = svzg;
    substitutions3g[yg] = 0;
    std::map<Variable, MultivariatePolynomial<Rational>> substitutions4;
    substitutions4[v] = sxy2;
    substitutions4[x] = MultivariatePolynomial<Rational>((Rational)12246789);
    GiNaC::exmap substitutions4g;
    substitutions4g[vg] = sxy2g;
    substitutions4g[xg] = 12246789;
    std::map<Variable, MultivariatePolynomial<Rational>> substitutions5;
    substitutions5[y] = sz;
    substitutions5[z] = svy;
    GiNaC::exmap substitutions5g;
    substitutions5g[yg] = szg;
    substitutions5g[zg] = svyg;

    EXPECT_EQ(f1.substitute(substitutions0), convertToCarl<MultivariatePolynomial<Rational>>(f1g.subs(substitutions0g), vars));
    EXPECT_EQ(f1.substitute(substitutions1), convertToCarl<MultivariatePolynomial<Rational>>(f1g.subs(substitutions1g), vars));
    EXPECT_EQ(f1.substitute(substitutions2), convertToCarl<MultivariatePolynomial<Rational>>(f1g.subs(substitutions2g), vars));
    EXPECT_EQ(f1.substitute(substitutions3), convertToCarl<MultivariatePolynomial<Rational>>(f1g.subs(substitutions3g), vars));
    EXPECT_EQ(f1.substitute(substitutions4), convertToCarl<MultivariatePolynomial<Rational>>(f1g.subs(substitutions4g), vars));
    EXPECT_EQ(f1.substitute(substitutions5), convertToCarl<MultivariatePolynomial<Rational>>(f1g.subs(substitutions5g), vars));

    EXPECT_EQ(f2.substitute(substitutions0), convertToCarl<MultivariatePolynomial<Rational>>(f2g.subs(substitutions0g), vars));
    EXPECT_EQ(f2.substitute(substitutions1), convertToCarl<MultivariatePolynomial<Rational>>(f2g.subs(substitutions1g), vars));
    EXPECT_EQ(f2.substitute(substitutions2), convertToCarl<MultivariatePolynomial<Rational>>(f2g.subs(substitutions2g), vars));
    EXPECT_EQ(f2.substitute(substitutions3), convertToCarl<MultivariatePolynomial<Rational>>(f2g.subs(substitutions3g), vars));
    EXPECT_EQ(f2.substitute(substitutions4), convertToCarl<MultivariatePolynomial<Rational>>(f2g.subs(substitutions4g), vars));
    EXPECT_EQ(f2.substitute(substitutions5), convertToCarl<MultivariatePolynomial<Rational>>(f2g.subs(substitutions5g), vars));
    #endif

    EvaluationMap<Rational> evMapA;
    evMapA[x] = carl::constant_zero<Rational>::get();
    MultivariatePolynomial<Rational> pxA({x});
    EXPECT_EQ(pxA.substitute(evMapA),carl::constant_zero<Rational>::get());
    
    EvaluationMap<Rational> evMapB;
    evMapB[x] = Rational(-1);
    MultivariatePolynomial<Rational> pxB = pxA + (Rational)1;
    EXPECT_EQ(pxB.substitute(evMapB),carl::constant_zero<Rational>::get());

	{
		Variable x = freshRealVariable("x");
		Variable y = freshRealVariable("y");
		Variable z = freshRealVariable("z");
		using P = MultivariatePolynomial<Rational>;
		P p = P(x)*x*x*y + P(y)*y*y + P(x)*x*x + P(x)*x*y + P(x)*y*y + P(x)*y*y*y;
		std::map<Variable,P> repl;
		repl.emplace(z, P());
		std::cout << p << std::endl;
		P res = p.substitute(repl);
		std::cout << "-> " << p << std::endl;
		std::cout << "-> " << res << std::endl;
		p.substituteIn(z, P());
		std::cout << "-> " << p << std::endl;
	}
}

//typedef Rational Rat;
//typedef MultivariatePolynomial<Rat> Pol;
//
//TEST(MultivariatePolynomial, VSRules)
//{
//    carl::VariablePool::getInstance().clear();
//    StringParser sp;
//    sp.setVariables({"v", "w", "x", "y", "z"});
//    Variable v = sp.variables().at("v");
//    Variable w = sp.variables().at("w");
//    Variable x = sp.variables().at("x");
//    Variable y = sp.variables().at("y");
//    Variable z = sp.variables().at("z");
//
//    Pol pz = sp.parseMultivariatePolynomial<Rat>("(-1)*x^2*z*y+(-1)*x*z*y^2+(-1)*z*y+(-1)*x^2*z+(-1)*x*w*y+(-1)*x*w+(-1)*w+1");
//    Pol ztNum = sp.parseMultivariatePolynomial<Rat>("(-1)*x^2*w*y+w+x*w*y+(-1)*x^2*w");
//    Pol ztDenom = sp.parseMultivariatePolynomial<Rat>("x^3*y+(-1)*x^2*y+(-1)*x^2+x^3+x*y+(-1)*x*y^2+(-1)*y+x^2*y^2+1");
//    Pol pzSub = pz.substitute( z, ztNum );
//    std::cout << pzSub << std::endl;
//    Pol py1 = sp.parseMultivariatePolynomial<Rat>("x^3*y+(-1)*x^2+(-1)*x^2*y+(-1)*x*w*y+(-1)*y+(-1)*x*y^2+(-1)*x*w+x*y+x^3+(-1)*w+x^2*y^2+1");
//    std::cout << py1 << std::endl;
//    pzSub *= ztDenom;
//    EXPECT_EQ( pzSub, py1 );
//    Pol py2 = sp.parseMultivariatePolynomial<Rat>("(-1)*x^3*y+y+x^2+(-1)*x^3+x*y^2+(-1)*x*y+(-1)*x^2*y^2+x^2*y");
//    Pol ytNum = sp.parseMultivariatePolynomial<Rat>("(-1)*x");
//    Pol ytDenom = sp.parseMultivariatePolynomial<Rat>("x+(-1)");
//    Pol px = sp.parseMultivariatePolynomial<Rat>("2*x+w+(-1)");
//    Pol xt = Pol(Rat(0));
//    Pol pv = sp.parseMultivariatePolynomial<Rat>("w+(-1)");
//    Pol vt = Pol(Rat(1));
//    Pol pw = sp.parseMultivariatePolynomial<Rat>("w+(-1)");
//    Pol wt = Pol(Rat(0));
//}

TEST(MultivariatePolynomial, SPolynomial)
{
    Variable x = freshRealVariable("x");
    Variable y = freshRealVariable("y");
    MultivariatePolynomial<Rational> f1({(Rational)1*x*x*x*y*y, (Rational)-1*x*x*y*y*y, (Rational)1*x});
    MultivariatePolynomial<Rational> g1({(Rational)3*x*x*x*x*y, (Rational)1*y*y});
    EXPECT_EQ((unsigned)3, carl::SPolynomial(f1.normalize(), g1.normalize()).nrTerms());
    //MultivariatePolynomial<Rational> s1({(Rational)-1*x*x});
    //EXPECT_EQ(s1, MultivariatePolynomial::SPolynomial(f1.normalize(), g1.normalize()));


    MultivariatePolynomial<Rational> f2({(Rational)1*x*x*x, (Rational)-2*x*y} );
	MultivariatePolynomial<Rational> g2({(Rational)1*x*x*y, (Rational)-2*y*y, (Rational)1*x});
	MultivariatePolynomial<Rational> s2({(Rational)-1*x*x});
	EXPECT_EQ(s2, carl::SPolynomial(f2, g2));
}

TEST(MultivariatePolynomial, GatherVariables)
{
    Variable x = freshRealVariable("x");
    Variable y = freshRealVariable("y");
    MultivariatePolynomial<Rational> f1({(Rational)1*x*x*x*y*y, (Rational)-1*x*x*y*y*y, (Rational)1*x});
    std::set<Variable> vars;
    f1.gatherVariables(vars);
    EXPECT_EQ(x, *vars.begin());
    EXPECT_EQ((unsigned)2, vars.size());

	{
		carlVariables vars;
		f1.gatherVariables(vars);
		EXPECT_EQ(vars, carlVariables({x, y}));
	}
}

TEST(MultivariatePolynomial, Derivative)
{
    Variable x = freshRealVariable("x");
    Variable y = freshRealVariable("y");
    MultivariatePolynomial<Rational> fx({x});
    EXPECT_EQ((Rational)1, carl::derivative(fx, x));
    EXPECT_EQ((Rational)0, carl::derivative(fx, y));
	MultivariatePolynomial<Rational> f2x({ (Rational)2 * x });
	EXPECT_EQ((Rational)2, carl::derivative(f2x, x));
    MultivariatePolynomial<Rational> f1({(Rational)1*x*x*x*y*y, (Rational)-1*x*x*y*y*y, (Rational)1*x});
    MultivariatePolynomial<Rational> f1dx({(Rational)3*x*x*y*y, (Rational)-2*x*y*y*y, Term<Rational>((Rational)1)});
    MultivariatePolynomial<Rational> f1dy({(Rational)2*x*x*x*y, (Rational)-3*x*x*y*y});
    EXPECT_EQ(f1dx, carl::derivative(f1, x));
    EXPECT_EQ(f1dy, carl::derivative(f1, y));
    EXPECT_EQ(carl::derivative(carl::derivative(f1, x), y), carl::derivative(carl::derivative(f1, y), x));
}

TEST(MultivariatePolynomial, varInfo)
{
    Variable x = freshRealVariable("x");
    Variable y = freshRealVariable("y");
    Variable z = freshRealVariable("z");

    MultivariatePolynomial<Rational> f1({(Rational)1*x*x*x*y*y, (Rational)-1*x*x*y*y*y, (Rational)1*x});

    VariablesInformation<false, MultivariatePolynomial<Rational>> vi = f1.getVarInfo<false>();
    EXPECT_EQ((unsigned)3, vi.getVarInfo(x)->maxDegree());
    EXPECT_EQ((unsigned)1, vi.getVarInfo(x)->minDegree());
    EXPECT_EQ((unsigned)3, vi.getVarInfo(x)->occurence());
    EXPECT_EQ((unsigned)3, vi.getVarInfo(y)->maxDegree());
    EXPECT_EQ((unsigned)2, vi.getVarInfo(y)->minDegree());
    EXPECT_EQ((unsigned)2, vi.getVarInfo(y)->occurence());
    EXPECT_EQ(nullptr, vi.getVarInfo(z));

    MultivariatePolynomial<Rational> f2({(Rational)1*x*x*x*x*y*y, (Rational)1*x*x*x*x*z*z ,(Rational)-1*x*y, (Rational)1*z});
    VariablesInformation<true, MultivariatePolynomial<Rational>> vi2 = f2.getVarInfo<true>();
    EXPECT_EQ((unsigned)4, vi2.getVarInfo(x)->maxDegree());
    EXPECT_EQ((unsigned)1, vi2.getVarInfo(x)->minDegree());
    EXPECT_EQ((unsigned)3, vi2.getVarInfo(x)->occurence());
    EXPECT_EQ((unsigned)0, vi2.getVarInfo(x)->coeffs().count(0));
    EXPECT_EQ((unsigned)0, vi2.getVarInfo(x)->coeffs().count(2));
    EXPECT_EQ((unsigned)0, vi2.getVarInfo(x)->coeffs().count(3));
    EXPECT_EQ(MultivariatePolynomial<Rational>({(Rational)1*y*y, (Rational)1*z*z}), vi2.getVarInfo(x)->coeffs().at(4));
    EXPECT_EQ((unsigned)2, vi2.getVarInfo(y)->maxDegree());
    EXPECT_EQ((unsigned)1, vi2.getVarInfo(y)->minDegree());
    EXPECT_EQ((unsigned)2, vi2.getVarInfo(y)->occurence());
    EXPECT_EQ((Rational)-1*x, vi2.getVarInfo(y)->coeffs().at(1));
    EXPECT_EQ((Rational)1*x*x*x*x, vi2.getVarInfo(y)->coeffs().at(2));
    EXPECT_EQ((unsigned)2, vi2.getVarInfo(z)->maxDegree());
    EXPECT_EQ((unsigned)1, vi2.getVarInfo(z)->minDegree());
    EXPECT_EQ((unsigned)2, vi2.getVarInfo(z)->occurence());
    EXPECT_EQ((Rational)1, vi2.getVarInfo(z)->coeffs().at(1));
}

TEST(MultivariatePolynomial, Quotient)
{
    Variable x = freshRealVariable("x");
    MultivariatePolynomial<Rational> one(Rational(1));
    MultivariatePolynomial<Rational> m1 = x*x - one;
    MultivariatePolynomial<Rational> m2 = x - one;
    MultivariatePolynomial<Rational> res = x + one;
    EXPECT_EQ(res, carl::quotient(m1, m2));
}

TEST(MultivariatePolynomial, Quotient2)
{
    StringParser sp;
    sp.setVariables({"t", "u"});
    Variable t = carl::VariablePool::getInstance().findVariableWithName("t");
    MultivariatePolynomial<Rational> p1 = sp.parseMultivariatePolynomial<Rational>("t^3*u^6+(-3)*t*u^5+(-1)*u^6+(-3)*t^2*u^6+3*t*u^6+(-1)*t^3*u^5+(-1)*t^3+(-3)*t+3*t^2+(-3)*u+3*t^3*u+9*t*u+(-9)*t^2*u+5*u^2+(-5)*t^3*u^2+(-15)*t*u^2+15*t^2*u^2+3*t^2*u^4+(-3)*t*u^4+(-1)*t^3*u^4+(-4)*u^3+u^4+(-12)*t^2*u^3+12*t*u^3+4*t^3*u^3+u^5+3*t^2*u^5+1");
    MultivariatePolynomial<Rational> p2 = sp.parseMultivariatePolynomial<Rational>("u^2+(-1)*u+1");
    EXPECT_TRUE( carl::quotient(p1, p2).has(t) );
    EXPECT_EQ( carl::quotient(p1, p2)*p2, p1 );
    MultivariatePolynomial<Rational> p3 = sp.parseMultivariatePolynomial<Rational>("t^2+(-1)*u+1");
    MultivariatePolynomial<Rational> p4 = p2*p3;
    EXPECT_EQ( carl::quotient(p4, p2)*p2, p4 );
    EXPECT_EQ( carl::quotient(p4, p3)*p3, p4 );
    MultivariatePolynomial<Rational> p5 = sp.parseMultivariatePolynomial<Rational>("u+1");
    MultivariatePolynomial<Rational> p6 = sp.parseMultivariatePolynomial<Rational>("t+1");
    MultivariatePolynomial<Rational> p7 = p5*p6;
    EXPECT_EQ( p7, carl::quotient(p7, p5)*p5 );
    EXPECT_EQ( p7, carl::quotient(p7, p6)*p6 );
}

TYPED_TEST(MultivariatePolynomialTest, MultivariatePolynomialMultiplication)
{
    Variable x = freshRealVariable("x");
    Variable y = freshRealVariable("y");

    EXPECT_EQ(MultivariatePolynomial<TypeParam>({(TypeParam)12*x*x*x*x*x*y, (TypeParam)6*x*x*x*y*y*y, (TypeParam)27*x*x*y*y, (TypeParam)28*x*x*x*x*y, (TypeParam)14*x*x*y*y*y, (TypeParam)63*x*y*y}),
            MultivariatePolynomial<TypeParam>({(TypeParam)3*x*x*y, (TypeParam)7*x*y}) * MultivariatePolynomial<TypeParam>({(TypeParam)4*x*x*x, (TypeParam)2*x*y*y, (TypeParam)9*y}));

    EXPECT_EQ(MultivariatePolynomial<TypeParam>({(TypeParam)30*x*y*y, (TypeParam)18*x*y*y*y, (TypeParam)8*x*y}),
            MultivariatePolynomial<TypeParam>({(TypeParam)3*x*y, (TypeParam)4*x}) * MultivariatePolynomial<TypeParam>({(TypeParam)2*y, (TypeParam)6*y*y}));

    EXPECT_EQ(MultivariatePolynomial<TypeParam>({(TypeParam)12*x*y, (TypeParam)21*x, (TypeParam)8*y, Term<TypeParam>(14)}),
            MultivariatePolynomial<TypeParam>({(TypeParam)3*x, Term<TypeParam>(2)}) * MultivariatePolynomial<TypeParam>({(TypeParam)4*y, Term<TypeParam>(7)}));

    EXPECT_EQ(MultivariatePolynomial<TypeParam>({(TypeParam)1*x*x, (TypeParam)-1*y*y}),
            MultivariatePolynomial<TypeParam>({(TypeParam)1*x, (TypeParam)1*y}) * MultivariatePolynomial<TypeParam>({(TypeParam)1*x, (TypeParam)-1*y}));

    EXPECT_EQ(MultivariatePolynomial<TypeParam>({(TypeParam)1*x*x, (TypeParam)2*x*y, (TypeParam)1*y*y}),
            MultivariatePolynomial<TypeParam>({(TypeParam)1*x, (TypeParam)1*y}) * MultivariatePolynomial<TypeParam>({(TypeParam)1*x, (TypeParam)1*y}));

    EXPECT_EQ(MultivariatePolynomial<TypeParam>({(TypeParam)1*x*x, (TypeParam)-2*x*y, (TypeParam)1*y*y}),
            MultivariatePolynomial<TypeParam>({(TypeParam)1*x, (TypeParam)-1*y}) * MultivariatePolynomial<TypeParam>({(TypeParam)1*x, (TypeParam)-1*y}));
    EXPECT_EQ(MultivariatePolynomial<TypeParam>({(TypeParam)12*x*x, (TypeParam)6*x*y*y}),
            MultivariatePolynomial<TypeParam>({(TypeParam)3*x}) * MultivariatePolynomial<TypeParam>({(TypeParam)4*x, (TypeParam)2*y*y}));

    EXPECT_EQ(MultivariatePolynomial<TypeParam>({(TypeParam)8*x*x*y, (TypeParam)6*x*y, (TypeParam)9*x*y*y}),
            MultivariatePolynomial<TypeParam>({(TypeParam)1*x*y}) * MultivariatePolynomial<TypeParam>({(TypeParam)8*x, Term<TypeParam>(6), (TypeParam)9*y}));
}

TYPED_TEST(MultivariatePolynomialTest, CreationViaOperators)
{
    Variable x = freshRealVariable("x");
    Variable y = freshRealVariable("y");

    EXPECT_EQ(MultivariatePolynomial<TypeParam>({(TypeParam)5*x*y, (TypeParam)7*y, (TypeParam)11*x*x}), (TypeParam)5*x*y+(TypeParam)7*y+(TypeParam)11*x*x);
    EXPECT_EQ(MultivariatePolynomial<TypeParam>({(TypeParam)1*x, (TypeParam)1*y}), (TypeParam)1*x + (TypeParam)1*y);
    EXPECT_EQ(MultivariatePolynomial<TypeParam>({(TypeParam)8*x*x*y, (TypeParam)6*x*y, (TypeParam)9*x*y*y}), (TypeParam)3*x*x*y + (TypeParam)4*x*y + (TypeParam)9*x*y*y + (TypeParam)2*x*y + (TypeParam)5*x*x*y);
}

TYPED_TEST(MultivariatePolynomialTest, Comparison)
{
    Variable x = freshRealVariable("x");
	Variable y = freshRealVariable("y");
	Variable z = freshRealVariable("z");

    MultivariatePolynomial<TypeParam> p0 = (TypeParam)3 * x * y * y + (TypeParam)7 * y * z;
    MultivariatePolynomial<TypeParam> p1 = (TypeParam)3 * x * y * y + (TypeParam)2 * x * x * y;
    MultivariatePolynomial<TypeParam> p2 = (TypeParam)5 * x * y * y + (TypeParam)3 * z;
    MultivariatePolynomial<TypeParam> p3 = (TypeParam)4 * x * x * z * z * z + (TypeParam)6 * y;

    ComparisonList<MultivariatePolynomial<TypeParam>> polynomials;
    polynomials.push_back(p0);
    polynomials.push_back(p1);
    polynomials.push_back(p2);
    polynomials.push_back(p3);

    expectRightOrder(polynomials);
}

TYPED_TEST(MultivariatePolynomialTest, OtherComparison)
{
    ComparisonList<Variable, Monomial::Arg, Term<TypeParam>, MultivariatePolynomial<TypeParam>> list;

    Variable x = freshRealVariable("x");
    Variable y = freshRealVariable("y");

    list.push_back(Term<TypeParam>((TypeParam)0));
    list.push_back(Term<TypeParam>((TypeParam)1));
    list.push_back(Term<TypeParam>((TypeParam)5));
    list.push_back(x);
    list.push_back(y);
    list.push_back((TypeParam)2 * x * x + y);
    list.push_back((TypeParam)3 * x * x);
    list.push_back((TypeParam)4 * x * y + (TypeParam)5 * x * x);
    list.push_back((TypeParam)8 * x * y + (TypeParam)2 * x * x);
    list.push_back((TypeParam)6 * x * x + y * y);
    list.push_back(x * x * y);
    list.push_back((TypeParam)7 * x * x * y);
    list.push_back((TypeParam)7 * x * x * y + (TypeParam)2);

    expectRightOrder(list);
}

#include "../benchmarks/framework/BenchmarkConversions.h"
#include "../benchmarks/framework/Common.h"
#ifdef USE_Z3_NUMBERS
#include <carl/numbers/adaption_z3/include.h>
#endif

TEST(MultivariatePolynomialTest, Resultant)
{
    Variable x = freshRealVariable("x0");
    Variable y = freshRealVariable("x1");
	typedef Rational T;
	carl::CIPtr ci = carl::CIPtr(new ConversionInformation({x,y}));

	MultivariatePolynomial<Rational> p = x*x + x*(y*y+T(4)) + y*y + T(18);
	MultivariatePolynomial<Rational> q = x*x + x*(T(4)*y+T(8)) + T(2)*y*y+y+T(19);
	//_r_1^2 + (_r_2^2+4)*_r_1^1 + _r_2^2+18
	//_r_1^2 + (4*_r_2+8)*_r_1^1 + 2*_r_2^3+_r_2+19

#ifdef USE_Z3_NUMBERS
	auto pz3 = carl::Conversion::convert<ZMP>(p, ci);
	auto qz3 = carl::Conversion::convert<ZMP>(q, ci);
	auto xz3 = carl::Conversion::convert<ZVAR>(x, ci);
#endif
	std::cout << p << ", " << q << std::endl;
#ifdef USE_Z3_NUMBERS
	std::cout << "#####  Z3  #####" << std::endl;
	auto resz3 = resultant(pz3, qz3, xz3);
	std::cout << "Result: " << resz3 << std::endl;
#endif

//	std::cout << "##### CArL #####" << std::endl;
//	auto res = carl::to_univariate_polynomial(p, x).resultant_z3(carl::to_univariate_polynomial(q, x));
//	std::cout << "Result: " << res << std::endl;
}

TEST(MultivariatePolynomialTest, Definiteness)
{
    Variable w = freshRealVariable("w");
    Variable x = freshRealVariable("x");
    Variable y = freshRealVariable("y");
    Variable z = freshRealVariable("z");
    MultivariatePolynomial<Rational> p1({(Rational)6*x*x, (Rational)49*y*y, (Rational)51*z*z, (Rational)-82*y*z, (Rational)20*x*z, (Rational)-4*x*y});
    EXPECT_TRUE(p1.definiteness() == Definiteness::POSITIVE_SEMI);
    MultivariatePolynomial<Rational> p2 = p1 + Rational(1);
    EXPECT_TRUE(p2.definiteness() == Definiteness::POSITIVE);
    MultivariatePolynomial<Rational> p3 = -p1 - Rational(1);
    EXPECT_TRUE(p3.definiteness() == Definiteness::NEGATIVE);
    MultivariatePolynomial<Rational> p4 = -p1;
    EXPECT_TRUE(p4.definiteness() == Definiteness::NEGATIVE_SEMI);
    MultivariatePolynomial<Rational> p5({(Rational)30156*w*w,
                                         (Rational)-79766*w*x,
                                         (Rational)62266*w*y,
                                         (Rational)61172*w*z,
                                         (Rational)100000*x*x,
                                         (Rational)-80312*x*y,
                                         (Rational)-100000*x*z,
                                         (Rational)33867*y*y,
                                         (Rational)55312*y*z,
                                         (Rational)100000*z*z});
    EXPECT_TRUE(p5.definiteness() == Definiteness::POSITIVE_SEMI);
}
