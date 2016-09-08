

import jcarl.*;
import org.junit.*;

import static org.junit.Assert.*;

public class TestCarl {

protected static Variable v1;
protected static Variable v2;
protected static Monomial m;
protected static TermRational t;

protected static Polynomial p1; 
protected static Polynomial p2;
protected static Polynomial p3;
protected static Polynomial p4; 
protected static RationalFunction f; 


@BeforeClass
public static void init() {
    try {
        System.loadLibrary("jcarl");
    } catch (UnsatisfiedLinkError e) {
      System.err.println("Native code library failed to load. See the chapter on Dynamic Linking Problems in the SWIG Java documentation for help.\n" + e);
      System.exit(1);
    }

v1 = new Variable("x",VariableType.VT_BOOL);
v2 = new Variable(VariableType.VT_INT);
m = Monomial.createMonomial(new Variable("x",VariableType.VT_REAL),3);
t = new TermRational(new Rational(5.5),m);
p1 = new Polynomial(Monomial.createMonomial(v1));
p2 = new Polynomial(m);
p3 = new Polynomial(v1);
p4 = new Polynomial(new Rational(-1));
f = new RationalFunction(p1,p4);


}

}
