#include "gtest/gtest.h"
#include "carl/core/MultivariatePolynomial.h"
#include "carl/core/UnivariatePolynomial.h"
#include "carl/converter/GinacConverter.h"
#include <cln/cln.h>
#include <gmpxx.h>


using namespace carl;

TEST(MultivariatePolynomial, Constructor)
{
    Variable v0(1);
    Term<mpz_class> t0(v0);
    MultivariatePolynomial<mpz_class> p0(t0);
    
    MultivariatePolynomial<mpz_class> p1(3);
    EXPECT_EQ((unsigned)1, p1.nrTerms());
    EXPECT_TRUE(p1.isLinear());
}

TEST(MultivariatePolynomial, Operators)
{
    Variable v0(1);
    Term<mpz_class> t0(v0);
    MultivariatePolynomial<mpz_class> p0a(t0);
    MultivariatePolynomial<mpz_class> p0b(v0);
    EXPECT_EQ(p0a, p0b);
    
    EXPECT_TRUE(p0a.isUnivariate());
}

TEST(MultivariatePolynomial, toUnivariatePolynomial)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
	Variable y = vpool.getFreshVariable();
	{
		MultivariatePolynomial<cln::cl_RA> p({(cln::cl_RA)1*x, (cln::cl_RA)-1*x*x, (cln::cl_RA)3*x*x*x});
		UnivariatePolynomial<cln::cl_RA> res(x, {(cln::cl_RA)0, (cln::cl_RA)1, (cln::cl_RA)-1, (cln::cl_RA)3});
		ASSERT_EQ(p.toUnivariatePolynomial(), res);
	}
	{
		MultivariatePolynomial<cln::cl_RA> p({(cln::cl_RA)1*x, (cln::cl_RA)-1*x*x, (cln::cl_RA)3*x*x*x});
		UnivariatePolynomial<MultivariatePolynomial<cln::cl_RA>> res(x, {(cln::cl_RA)0, (cln::cl_RA)1, (cln::cl_RA)-1, (cln::cl_RA)3});
		ASSERT_EQ(p.toUnivariatePolynomial(x), res);
	}
	{
		MultivariatePolynomial<cln::cl_RA> p({(cln::cl_RA)1*x*y, (cln::cl_RA)-1*x*x, (cln::cl_RA)3*x*x*x});
		UnivariatePolynomial<MultivariatePolynomial<cln::cl_RA>> res(x, {MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)0), MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)1*y), MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)-1), MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)3)});
		ASSERT_EQ(res, p.toUnivariatePolynomial(x));
	}
}

TEST(MultivariatePolynomial, Addition)
{
    Variable v0((unsigned)1);
    Term<mpz_class> t0(v0);
    MultivariatePolynomial<mpz_class> p0(v0);
    p0 += 3;
    EXPECT_EQ((unsigned)2, p0.nrTerms());
    p0 += 3;
    EXPECT_EQ((unsigned)2, p0.nrTerms());
    p0 += -6;
    EXPECT_EQ((unsigned)1, p0.nrTerms());
    
    Variable v1((unsigned)2);
    Variable v2((unsigned)3);
    p0 += v1;
    p0 += Monomial(v2);
    EXPECT_EQ((unsigned)3,p0.nrTerms());
    p0 += Monomial(v2);
    EXPECT_EQ((unsigned)3,p0.nrTerms());
    p0 += Term<mpz_class>(-2,v2);
    EXPECT_EQ((unsigned)2,p0.nrTerms());
    
    MultivariatePolynomial<mpz_class> p1(v0);
    p1 += v1;
    p0 += p1;
    EXPECT_EQ((unsigned)2,p0.nrTerms());   
    MultivariatePolynomial<mpz_class> mp2(v0);
    mp2 += (mpz_class)2 * v1;
    EXPECT_EQ(v0, *mp2.lterm());
    MultivariatePolynomial<mpz_class> p10a;
    p10a += v0*v0;
    p10a += v1;
    EXPECT_EQ(v0*v0, *p10a.lterm());
    MultivariatePolynomial<mpz_class> p10b;
    p10b += v1;
    p10b += v0*v0;
    EXPECT_EQ(v0*v0, *p10b.lterm());
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

TEST(MultivariatePolynomial, Substraction)
{
    Variable v0((unsigned)1);
    MultivariatePolynomial<mpz_class> p0(v0);
    p0 -= 3;
    EXPECT_EQ((unsigned)2, p0.nrTerms());
    p0 -= 3;
    EXPECT_EQ((unsigned)2, p0.nrTerms());
    p0 -= -6;
    EXPECT_EQ((unsigned)1, p0.nrTerms());
    Variable v1((unsigned)2);
    Variable v2((unsigned)3);
    p0 -= v1;
    EXPECT_EQ((unsigned)2,p0.nrTerms());
    p0 -= Monomial(v2);
    EXPECT_EQ((unsigned)3,p0.nrTerms());
    p0 -= Monomial(v2);
    
    EXPECT_EQ((unsigned)3,p0.nrTerms());
    p0 -= Term<mpz_class>(-2,v2);
    EXPECT_EQ((unsigned)2,p0.nrTerms());
    
    MultivariatePolynomial<mpz_class> p1(v0);
    p1 -= v1;
    p0 -= p1;
    EXPECT_EQ((unsigned)0,p0.nrTerms());
}

TEST(MultivariatePolynomial, Multiplication)
{
    Variable v0(1);
    Variable v1(2);
    MultivariatePolynomial<mpz_class> p0(v0);
    
    p0 *= v0;
    EXPECT_EQ(Term<mpz_class>((unsigned)1,v0,2), *(p0.lterm()));
    
    MultivariatePolynomial<mpz_class> q({mpz_class(2)*v0*v0, mpz_class(1)*v0*v1});
    Term<mpz_class> t(1, v0,2);
    MultivariatePolynomial<mpz_class> res = q * t;
    EXPECT_EQ(MultivariatePolynomial<mpz_class>({mpz_class(2)*v0*v0*v0*v0, mpz_class(1)*v0*v0*v0*v1}), res);
    
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
    Variable v0(1);
    Variable v1(2);
    Variable v2(3);
    MultivariatePolynomial<cln::cl_RA> mp;
    mp += v0;
    MultivariatePolynomial<cln::cl_RA> mp2 = mp * (cln::cl_RA)2;
    EXPECT_EQ(mp, mp.normalize());
    EXPECT_EQ(mp, mp2.normalize());
    
    mp = MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)2 * v0);
    mp += (cln::cl_RA)2 * v1;
    mp2 = MultivariatePolynomial<cln::cl_RA>(v0);
    mp2 += v1;
    EXPECT_EQ(mp2, mp.normalize());
    
    mp = MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)2 * v0);
    mp += (cln::cl_RA)4 * v1;
    mp2 = MultivariatePolynomial<cln::cl_RA>(v0);
    mp2 += (cln::cl_RA)2 * v1;
    EXPECT_EQ(mp2, mp.normalize());
}

TEST(MultivariatePolynomial, Coprime)
{
    Variable v0(1);
    Variable v1(2);
    MultivariatePolynomial<cln::cl_RA> mp;
    mp += v0;
    MultivariatePolynomial<cln::cl_RA> mp2 = mp * (cln::cl_RA)2;
    EXPECT_EQ(mp, mp.coprimeCoefficients());
    
    mp = MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)2 * v0);
    mp += (cln::cl_RA)4 * v1;
    mp2 = MultivariatePolynomial<cln::cl_RA>(v0);
    mp2 += (cln::cl_RA)2 * v1;
    EXPECT_EQ(mp2, mp.coprimeCoefficients());
    
    mp = MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)1/6 * v0);
    mp += (cln::cl_RA)4 * v1;
    mp2 = MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)1 * v0);
    mp2 += (cln::cl_RA)24 * v1;
    EXPECT_EQ(mp2, mp.coprimeCoefficients());
    
    mp = MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)1/6 * v0);
    mp += (cln::cl_RA)1/4 * v1;
    mp2 = MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)2 * v0);
    mp2 += (cln::cl_RA)3 * v1;
    EXPECT_EQ(mp2, mp.coprimeCoefficients());
    
    MultivariatePolynomial<mpq_class> mpG = MultivariatePolynomial<mpq_class>(mpq_class(1,6) * v0);
    mpG += mpq_class(1,4) * v1;
    MultivariatePolynomial<mpq_class> mp2G = MultivariatePolynomial<mpq_class>(mpq_class(2) * v0);
    mp2G += mpq_class(3) * v1;
    EXPECT_EQ(mp2G, mpG.coprimeCoefficients());
    
    
}
        
TEST(MultivariatePolynomial, Substitute)
{
    Variable v0(1);
    Variable v1(2);
	{
		MultivariatePolynomial<cln::cl_RA> p(v0);
		p.substituteIn(v0, MultivariatePolynomial<cln::cl_RA>(-1));
		ASSERT_EQ(p, MultivariatePolynomial<cln::cl_RA>(-1));
	}
    MultivariatePolynomial<cln::cl_RA> mp((cln::cl_RA)1/6 * v0);
    mp += (cln::cl_RA)4 * v1;
    MultivariatePolynomial<cln::cl_RA> mp2((cln::cl_RA)2);
    mp2 += (cln::cl_RA)4 * v1;
    std::map<Variable, MultivariatePolynomial<cln::cl_RA>> substitutions;
    substitutions[v0] = MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)12);
    EXPECT_EQ(mp2, mp.substitute(substitutions));
    substitutions[v0] = MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)0);
    EXPECT_EQ(MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)4 * v1), mp.substitute(substitutions));
    #ifdef COMPARE_WITH_GINAC
    VariablePool& vpool = VariablePool::getInstance();
    Variable v = vpool.getFreshVariable();
    vpool.setVariableName(v, "v");
    Variable x = vpool.getFreshVariable();
    vpool.setVariableName(x, "x");
    Variable y = vpool.getFreshVariable();
    vpool.setVariableName(y, "y");
    Variable z = vpool.getFreshVariable();
    vpool.setVariableName(z, "z");
    
    std::map<GiNaC::ex, Variable, GiNaC::ex_is_less> vars;
    GiNaC::symbol vg("v"), xg("x"), yg("y"), zg("z");
    vars.insert(std::pair<GiNaC::ex, Variable>(vg, v));
    vars.insert(std::pair<GiNaC::ex, Variable>(xg, x));
    vars.insert(std::pair<GiNaC::ex, Variable>(yg, y));
    vars.insert(std::pair<GiNaC::ex, Variable>(zg, z));
    
    MultivariatePolynomial<cln::cl_RA> f1({(cln::cl_RA)1*x*x*v*x*y*y, (cln::cl_RA)-2*x*x*y*y*y, (cln::cl_RA)312347*v*v*x, (cln::cl_RA)3*y*v*z*z*x, Term<cln::cl_RA>((cln::cl_RA)1)});
    GiNaC::ex f1g = xg*xg*vg*xg*yg*yg-2*xg*xg*yg*yg*yg+312347*vg*vg*xg+3*yg*vg*zg*zg*xg+1;
    EXPECT_EQ(f1, convertToCarl(f1g, vars));
    MultivariatePolynomial<cln::cl_RA> f2({(cln::cl_RA)7*x*x*x*x*y*y, (cln::cl_RA)191*x*x*x*x*z*z*z ,(cln::cl_RA)-3*x*x*y, (cln::cl_RA)1*z*z*x*v*v, (cln::cl_RA)2*z*y*v*v, Term<cln::cl_RA>((cln::cl_RA)4)});
    GiNaC::ex f2g = 7*xg*xg*xg*xg*yg*yg+191*xg*xg*xg*xg*zg*zg*zg-3*xg*xg*yg+zg*zg*xg*vg*vg+2*zg*yg*vg*vg+4;
    EXPECT_EQ(f2, convertToCarl(f2g, vars));
    
    MultivariatePolynomial<cln::cl_RA> sy({(cln::cl_RA)-2*y*y*y, (cln::cl_RA)-9*y, Term<cln::cl_RA>((cln::cl_RA)15)});
    GiNaC::ex syg = -2*yg*yg*yg-9*yg+15;
    EXPECT_EQ(sy, convertToCarl(syg, vars));
    MultivariatePolynomial<cln::cl_RA> sxy1({(cln::cl_RA)1*x*y*y, (cln::cl_RA)-5*y*y*y, Term<cln::cl_RA>((cln::cl_RA)3377)});
    GiNaC::ex sxy1g = xg*yg*yg-5*yg*yg*yg+3377;
    EXPECT_EQ(sxy1, convertToCarl(sxy1g, vars));
    MultivariatePolynomial<cln::cl_RA> sx({(cln::cl_RA)1*x, Term<cln::cl_RA>((cln::cl_RA)-1)});
    GiNaC::ex sxg = xg-1;
    EXPECT_EQ(sx, convertToCarl(sxg, vars));
    MultivariatePolynomial<cln::cl_RA> svyz({(cln::cl_RA)8*v*v*y, (cln::cl_RA)1*y*z*y, (cln::cl_RA)29*z*z*z*z*z});
    GiNaC::ex svyzg = 8*vg*vg*yg+yg*zg*yg+29*zg*zg*zg*zg*zg;
    EXPECT_EQ(svyz, convertToCarl(svyzg, vars));
    MultivariatePolynomial<cln::cl_RA> svz({(cln::cl_RA)1*v*v, (cln::cl_RA)-1*z, (cln::cl_RA)-2*v*z, Term<cln::cl_RA>((cln::cl_RA)10)});
    GiNaC::ex svzg = vg*vg-zg-2*vg*zg+10;
    EXPECT_EQ(svz, convertToCarl(svzg, vars));
    MultivariatePolynomial<cln::cl_RA> sxy2({(cln::cl_RA)2*x*y*y});
    GiNaC::ex sxy2g = 2*xg*yg*yg;
    EXPECT_EQ(sxy2, convertToCarl(sxy2g, vars));
    MultivariatePolynomial<cln::cl_RA> sv({(cln::cl_RA)1*v*v*v*v});
    GiNaC::ex svg = vg*vg*vg*vg;
    EXPECT_EQ(sv, convertToCarl(svg, vars));
    MultivariatePolynomial<cln::cl_RA> sz({(cln::cl_RA)1*z});
    GiNaC::ex szg = zg;
    EXPECT_EQ(sz, convertToCarl(szg, vars));
    MultivariatePolynomial<cln::cl_RA> sxz({(cln::cl_RA)8*x*z*z*z, (cln::cl_RA)-5*x*x*x, Term<cln::cl_RA>((cln::cl_RA)7)});
    GiNaC::ex sxzg = 8*xg*zg*zg*zg-5*xg*xg*xg+7;
    EXPECT_EQ(sxz, convertToCarl(sxzg, vars));
    MultivariatePolynomial<cln::cl_RA> sxv({(cln::cl_RA)3*x, (cln::cl_RA)1*v*v, (cln::cl_RA)2*v*v*v*v});
    GiNaC::ex sxvg = 3*xg+vg*vg+2*vg*vg*vg*vg;
    EXPECT_EQ(sxv, convertToCarl(sxvg, vars));
    MultivariatePolynomial<cln::cl_RA> svy({(cln::cl_RA)13*v*y, (cln::cl_RA)-3*y*y*y*v, Term<cln::cl_RA>((cln::cl_RA)100)});
    GiNaC::ex svyg = 13*vg*yg-3*yg*yg*yg*vg+100;
    EXPECT_EQ(svy, convertToCarl(svyg, vars));
    
    std::map<Variable, MultivariatePolynomial<cln::cl_RA>> substitutions0;
    substitutions0[v] = sy;
    substitutions0[x] = svy;
    substitutions0[y] = MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)0);
    substitutions0[z] = sy;
    GiNaC::exmap substitutions0g;
    substitutions0g[vg] = syg;
    substitutions0g[xg] = svyg;
    substitutions0g[yg] = 0;
    substitutions0g[zg] = syg;
    std::map<Variable, MultivariatePolynomial<cln::cl_RA>> substitutions1;
    substitutions1[v] = MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)0);
    substitutions1[x] = svyz;
    substitutions1[y] = MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)1289);
    substitutions1[z] = sxv;
    GiNaC::exmap substitutions1g;
    substitutions1g[vg] = 0;
    substitutions1g[xg] = svyzg;
    substitutions1g[yg] = 1289;
    substitutions1g[zg] = sxvg;
    std::map<Variable, MultivariatePolynomial<cln::cl_RA>> substitutions2;
    substitutions2[v] = sx;
    substitutions2[x] = sv;
    substitutions2[y] = sxz;
    GiNaC::exmap substitutions2g;
    substitutions2g[vg] = sxg;
    substitutions2g[xg] = svg;
    substitutions2g[yg] = sxzg;
    std::map<Variable, MultivariatePolynomial<cln::cl_RA>> substitutions3;
    substitutions3[v] = sxy1;
    substitutions3[x] = svz;
    substitutions3[y] = MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)0);
    GiNaC::exmap substitutions3g;
    substitutions3g[vg] = sxy1g;
    substitutions3g[xg] = svzg;
    substitutions3g[yg] = 0;
    std::map<Variable, MultivariatePolynomial<cln::cl_RA>> substitutions4;
    substitutions4[v] = sxy2;
    substitutions4[x] = MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)12246789);
    GiNaC::exmap substitutions4g;
    substitutions4g[vg] = sxy2g;
    substitutions4g[xg] = 12246789;
    std::map<Variable, MultivariatePolynomial<cln::cl_RA>> substitutions5;
    substitutions5[y] = sz;
    substitutions5[z] = svy;
    GiNaC::exmap substitutions5g;
    substitutions5g[yg] = szg;
    substitutions5g[zg] = svyg;
    
    EXPECT_EQ(f1.substitute(substitutions0), convertToCarl(f1g.subs(substitutions0g), vars));
    EXPECT_EQ(f1.substitute(substitutions1), convertToCarl(f1g.subs(substitutions1g), vars));
    EXPECT_EQ(f1.substitute(substitutions2), convertToCarl(f1g.subs(substitutions2g), vars));
    EXPECT_EQ(f1.substitute(substitutions3), convertToCarl(f1g.subs(substitutions3g), vars));
    EXPECT_EQ(f1.substitute(substitutions4), convertToCarl(f1g.subs(substitutions4g), vars));
    EXPECT_EQ(f1.substitute(substitutions5), convertToCarl(f1g.subs(substitutions5g), vars));
    
    EXPECT_EQ(f2.substitute(substitutions0), convertToCarl(f2g.subs(substitutions0g), vars));
    EXPECT_EQ(f2.substitute(substitutions1), convertToCarl(f2g.subs(substitutions1g), vars));
    EXPECT_EQ(f2.substitute(substitutions2), convertToCarl(f2g.subs(substitutions2g), vars));
    EXPECT_EQ(f2.substitute(substitutions3), convertToCarl(f2g.subs(substitutions3g), vars));
    EXPECT_EQ(f2.substitute(substitutions4), convertToCarl(f2g.subs(substitutions4g), vars));
    EXPECT_EQ(f2.substitute(substitutions5), convertToCarl(f2g.subs(substitutions5g), vars));
    #endif
}

TEST(MultivariatePolynomial, SPolynomial)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setName(x, "x");
    Variable y = vpool.getFreshVariable();
    vpool.setName(y, "y");
    Variable z = vpool.getFreshVariable();
    vpool.setName(z, "z");
    MultivariatePolynomial<cln::cl_RA> f1({(cln::cl_RA)1*x*x*x*y*y, (cln::cl_RA)-1*x*x*y*y*y, (cln::cl_RA)1*x});
    MultivariatePolynomial<cln::cl_RA> g1({(cln::cl_RA)3*x*x*x*x*y, (cln::cl_RA)1*y*y});
    EXPECT_EQ((unsigned)3,MultivariatePolynomial<cln::cl_RA>::SPolynomial(f1.normalize(), g1.normalize()).nrTerms());
    //MultivariatePolynomial<cln::cl_RA> s1({(cln::cl_RA)-1*x*x});
    //EXPECT_EQ(s1, MultivariatePolynomial::SPolynomial(f1.normalize(), g1.normalize()));
    
    
    MultivariatePolynomial<cln::cl_RA> f2({(cln::cl_RA)1*x*x*x, (cln::cl_RA)-2*x*y} );
    MultivariatePolynomial<cln::cl_RA> g2({(cln::cl_RA)1*x*x*y, (cln::cl_RA)-2*y*y, (cln::cl_RA)1*x});
    MultivariatePolynomial<cln::cl_RA> s2({(cln::cl_RA)-1*x*x});
    EXPECT_EQ(s2, MultivariatePolynomial<cln::cl_RA>::SPolynomial(f2, g2));
}

TEST(MultivariatePolynomial, GatherVariables)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setName(x, "x");
    Variable y = vpool.getFreshVariable();
    vpool.setName(y, "y");
    Variable z = vpool.getFreshVariable();
    vpool.setName(z, "z");
    MultivariatePolynomial<cln::cl_RA> f1({(cln::cl_RA)1*x*x*x*y*y, (cln::cl_RA)-1*x*x*y*y*y, (cln::cl_RA)1*x});
    std::set<Variable> vars;
    f1.gatherVariables(vars);
    EXPECT_EQ(x, *vars.begin());
    EXPECT_EQ((unsigned)2, vars.size());
}

TEST(MultivariatePolynomial, Derivative)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setName(x, "x");
    Variable y = vpool.getFreshVariable();
    vpool.setName(y, "y");
    Variable z = vpool.getFreshVariable();
    vpool.setName(z, "z");
    MultivariatePolynomial<cln::cl_RA> fx({x});
    EXPECT_EQ((cln::cl_RA)1, fx.derivative(x));
    EXPECT_EQ((cln::cl_RA)0, fx.derivative(y));
    MultivariatePolynomial<cln::cl_RA> f2x({(cln::cl_RA)2*x});
    EXPECT_EQ((cln::cl_RA)2, f2x.derivative(x));
    MultivariatePolynomial<cln::cl_RA> f1({(cln::cl_RA)1*x*x*x*y*y, (cln::cl_RA)-1*x*x*y*y*y, (cln::cl_RA)1*x});
    MultivariatePolynomial<cln::cl_RA> f1dx({(cln::cl_RA)3*x*x*y*y, (cln::cl_RA)-2*x*y*y*y, Term<cln::cl_RA>((cln::cl_RA)1)});
    MultivariatePolynomial<cln::cl_RA> f1dy({(cln::cl_RA)2*x*x*x*y, (cln::cl_RA)-3*x*x*y*y});
    EXPECT_EQ(f1dx, f1.derivative(x));
    EXPECT_EQ(f1dy, f1.derivative(y));
    EXPECT_EQ(f1.derivative(x).derivative(y), f1.derivative(y).derivative(x));
}

TEST(MultivariatePolynomial, varInfo)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setName(x, "x");
    Variable y = vpool.getFreshVariable();
    vpool.setName(y, "y");
    Variable z = vpool.getFreshVariable();
    vpool.setName(z, "z");
    
    MultivariatePolynomial<cln::cl_RA> f1({(cln::cl_RA)1*x*x*x*y*y, (cln::cl_RA)-1*x*x*y*y*y, (cln::cl_RA)1*x});
    
    VariablesInformation<false, MultivariatePolynomial<cln::cl_RA>> vi = f1.getVarInfo<false>();
    EXPECT_EQ((unsigned)3, vi.getVarInfo(x)->maxDegree());
    EXPECT_EQ((unsigned)1, vi.getVarInfo(x)->minDegree());
    EXPECT_EQ((unsigned)3, vi.getVarInfo(x)->occurence());
    EXPECT_EQ((unsigned)3, vi.getVarInfo(y)->maxDegree());
    EXPECT_EQ((unsigned)2, vi.getVarInfo(y)->minDegree());
    EXPECT_EQ((unsigned)2, vi.getVarInfo(y)->occurence());
    EXPECT_EQ(nullptr, vi.getVarInfo(z));
    
    MultivariatePolynomial<cln::cl_RA> f2({(cln::cl_RA)1*x*x*x*x*y*y, (cln::cl_RA)1*x*x*x*x*z*z ,(cln::cl_RA)-1*x*y, (cln::cl_RA)1*z});
    VariablesInformation<true, MultivariatePolynomial<cln::cl_RA>> vi2 = f2.getVarInfo<true>();
    EXPECT_EQ((unsigned)4, vi2.getVarInfo(x)->maxDegree());
    EXPECT_EQ((unsigned)1, vi2.getVarInfo(x)->minDegree());
    EXPECT_EQ((unsigned)3, vi2.getVarInfo(x)->occurence());
    EXPECT_EQ((unsigned)0, vi2.getVarInfo(x)->coeffs().count(0));
    EXPECT_EQ((unsigned)0, vi2.getVarInfo(x)->coeffs().count(2));
    EXPECT_EQ((unsigned)0, vi2.getVarInfo(x)->coeffs().count(3));
    EXPECT_EQ(MultivariatePolynomial<cln::cl_RA>({(cln::cl_RA)1*y*y, (cln::cl_RA)1*z*z}), vi2.getVarInfo(x)->coeffs().at(4)); 
    EXPECT_EQ((unsigned)2, vi2.getVarInfo(y)->maxDegree());
    EXPECT_EQ((unsigned)1, vi2.getVarInfo(y)->minDegree());
    EXPECT_EQ((unsigned)2, vi2.getVarInfo(y)->occurence());
    EXPECT_EQ((cln::cl_RA)-1*x, vi2.getVarInfo(y)->coeffs().at(1)); 
    EXPECT_EQ((cln::cl_RA)1*x*x*x*x, vi2.getVarInfo(y)->coeffs().at(2)); 
    EXPECT_EQ((unsigned)2, vi2.getVarInfo(z)->maxDegree());
    EXPECT_EQ((unsigned)1, vi2.getVarInfo(z)->minDegree());
    EXPECT_EQ((unsigned)2, vi2.getVarInfo(z)->occurence());
    EXPECT_EQ((cln::cl_RA)1, vi2.getVarInfo(z)->coeffs().at(1));
}

TEST(MultivariatePolynomial, cauchyBounds)
{
    EXPECT_TRUE(false);
}

TEST(MultivariatePolyonomial, factorization)
{
    EXPECT_TRUE(false);
}
