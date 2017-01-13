

import jcarl.*;
import org.junit.*;

import static org.junit.Assert.*;

public abstract class TestCarl {

protected static Variable v1,v11,v2,v21,x;
protected static Monomial m;
protected static TermRational t;

protected static Polynomial p1; 
protected static Polynomial p2;
protected static Polynomial p3;
protected static Polynomial p4; 
protected static RationalFunction f; 

protected static ConstraintRational c1,c2,c3;
protected static FormulaPolynomial f1,f2,f3,f4;

protected static FactorizationCache cache;
protected static FactorizedPolynomialRational fp1,fp2;




@BeforeClass
public static void init() {
    try {
        System.loadLibrary("jcarl");
    } catch (UnsatisfiedLinkError e) {
      System.err.println("Native code library failed to load. See the chapter on Dynamic Linking Problems in the SWIG Java documentation for help.\n" + e);
      System.exit(1);
    }

v1 = new Variable("x",VariableType.VT_REAL);
v2 = new Variable(VariableType.VT_INT);
m = Monomial.createMonomial(new Variable("x",VariableType.VT_REAL),3);
t = new TermRational(new Rational(5.5),m);
p1 = new Polynomial(Monomial.createMonomial(v1));
p2 = new Polynomial(m);
p3 = new Polynomial(v1);
p4 = new Polynomial(new Rational(-1));
f = new RationalFunction(p1,p4);


v11 = new Variable("v1",VariableType.VT_REAL);	
v21 = new Variable("v2",VariableType.VT_REAL);	


c1 = new ConstraintRational(true);
c2 = new ConstraintRational(v11,Relation.EQ,new Rational(10));
c3 = new ConstraintRational(v21,Relation.GEQ);

x = new Variable("y",VariableType.VT_BOOL);
f1 = new FormulaPolynomial(x);
f2 = new FormulaPolynomial(c2);
f3 = new FormulaPolynomial(FormulaType.NOT,f2);
FormulaVector subformulas = new FormulaVector();
subformulas.add(f1);	
subformulas.add(f2);
f4 = new FormulaPolynomial(FormulaType.AND,subformulas);

cache = new FactorizationCache();
fp1 = new FactorizedPolynomialRational(new Rational(2));
fp2 = new FactorizedPolynomialRational(p1,cache);

}

}
