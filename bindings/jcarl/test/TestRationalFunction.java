
import jcarl.*;
import org.junit.*;

import static org.junit.Assert.*;

public class TestRationalFunction extends TestCarl {


private static FactorizedRationalFunction frf1;


@BeforeClass
public static void initRF() {
frf1 = new FactorizedRationalFunction(fp1,fp2);
}

@Test
public void testToString() {

	assertEquals(f.toString(),"((-1)*x)/(1)");
	assertEquals(frf1.toString(),"(2)/(x)");

}



@Test
public void testEvaluate() {
	VarRationalMap map = new VarRationalMap();
	map.set(v1,new Rational(2));
	assertTrue(f.evaluate(map).equals(new Rational(-2)));
	assertTrue(frf1.evaluate(map).equals(new Rational(1)));
}


@Test
public void testGetters() {
	assertTrue(f.nominator().equals(new Polynomial(v1.neg())));
	assertTrue(f.numerator().equals(new Polynomial(v1.neg())));
	assertTrue(f.denominator().equals(new Polynomial (new Rational(1))));
	assertTrue(frf1.nominator().equals(fp1));
	assertTrue(frf1.numerator().equals(fp1));
	assertTrue(frf1.denominator().equals(fp2));
}

@Test
public void testGatherVariables() {
	VarVector vars = f.gatherVariables();
	assertTrue(vars.get(0).equals(v1));
	assertEquals(vars.size(),1);
	vars = frf1.gatherVariables();
	assertTrue(vars.get(0).equals(v1));
	assertEquals(vars.size(),1);
}


@Test
public void testComparisonOperators() {
	RationalFunction f2 = new RationalFunction(p4,p1);

	assertEquals(f.equals(f),true);
	assertEquals(f.equals(f2),false);
	assertEquals(f.notEquals(f),false);	
	assertEquals(f.notEquals(f2),true);

}


@Test
public void testOperators() {


	assertEquals(f.add(v1).toString(),"0");
	assertEquals(f.add(m).toString(),"(x^3+(-1)*x)/(1)");
	assertEquals(f.add(t).toString(),"(11*x^3+(-2)*x)/(2)");
	assertEquals(f.add(p1).toString(),"0");
	assertEquals(f.add(new Rational(-5.5)).toString(),"((-2)*x+(-11))/(2)");

	assertEquals(f.sub(v1).toString(),"((-2)*x)/(1)");
	assertEquals(f.sub(m).toString(),"((-1)*x^3+(-1)*x)/(1)");
	assertEquals(f.sub(t).toString(),"((-11)*x^3+(-2)*x)/(2)");
	assertEquals(f.sub(p1).toString(),"((-2)*x)/(1)");
	assertEquals(f.sub(new Rational(-5.5)).toString(),"((-2)*x+11)/(2)");

	assertEquals(f.mul(v1).toString(),"((-1)*x^2)/(1)");
	assertEquals(f.mul(m).toString(),"((-1)*x^4)/(1)");
	assertEquals(f.mul(t).toString(),"((-11)*x^4)/(2)");
	assertEquals(f.mul(p1).toString(),"((-1)*x^2)/(1)");
	assertEquals(f.mul(new Rational(-5.5)).toString(),"(11*x)/(2)");

	assertEquals(f.div(v1).toString(),"((-1)*x)/(x)");
	assertEquals(f.div(m).toString(),"((-1)*x)/(x^3)");
	assertEquals(f.div(t).toString(),"((-2)*x)/(11*x^3)");
	assertEquals(f.div(p1).toString(),"((-1)*x)/(x)");
	assertEquals(f.div(new Rational(-5.5)).toString(),"(2/11*x)/(1)");
	assertEquals(f.div(f).toString(),"1");

	assertEquals(f.neg().toString(),"(x)/(1)");
	assertEquals(f.pow(20).toString(),"(x^20)/(1)");

}




}


