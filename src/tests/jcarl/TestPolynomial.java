
import jcarl.*;
import org.junit.*;

import static org.junit.Assert.*;

public class TestPolynomial extends TestCarl {



@Test
public void testToString() {
	assertEquals(p1.toString(),"x");
}





@Test
public void testGetters() {
	assertTrue(p1.constantPart().equals(new Rational(0)));
	assertEquals(p1.totalDegree(),1);
	assertEquals(p1.degree(v1),1);
	assertEquals(p1.degree(v2),0);
	assertEquals(p1.nrTerms(),1);
	assertEquals(p1.size(),1);
}

@Test
public void testEvaluate() {
	VarRationalMap map = new VarRationalMap();
	map.set(v1,new Rational(2));
	assertTrue(p3.evaluate(map).equals(new Rational(2)));
}


@Test
public void testComparisonOperators() {
	assertEquals(p1.equals(p1),true);
	assertEquals(p1.equals(p2),false);
	assertEquals(p1.notEquals(p1),false);
	assertEquals(p1.notEquals(p2),true);
}



@Test
public void testOperators() {


	assertEquals(p1.add(v1).toString(),"2*x");
	assertEquals(p1.add(m).toString(),"x^3+x");
	assertEquals(p1.add(t).toString(),"11/2*x^3+x");
	assertEquals(p1.add(p1).toString(),"2*x");
	assertEquals(p1.add(new Rational(-5.5)).toString(),"x+(-11/2)");

	assertEquals(p1.sub(v1).toString(),"0");
	assertEquals(p1.sub(m).toString(),"(-1)*x^3+x");
	assertEquals(p1.sub(t).toString(),"(-11/2)*x^3+x");
	assertEquals(p1.sub(p1).toString(),"0");
	assertEquals(p1.sub(new Rational(-5.5)).toString(),"x+11/2");

	assertEquals(p1.mul(v1).toString(),"x^2");
	assertEquals(p1.mul(m).toString(),"x^4");
	assertEquals(p1.mul(t).toString(),"11/2*x^4");
	assertEquals(p1.mul(p1).toString(),"x^2");
	assertEquals(p1.mul(new Rational(-5.5)).toString(),"(-11/2)*x");

	assertEquals(p1.div(v1).toString(),"1");
	assertEquals(p1.div(m).toString(),"(x)/(x^3)");
	assertEquals(p1.div(t).toString(),"(2*x)/(11*x^3)");
	assertEquals(p1.div(p1).toString(),"1");
	assertEquals(p1.div(new Rational(-5.5)).toString(),"(-2/11)*x");
	assertEquals(p1.div(f).toString(),"((-1)*x)/(x)");

	assertEquals(p1.neg().toString(),"(-1)*x");
	assertEquals(p1.pow(20).toString(),"x^20");

}




}


