
import jcarl.*;
import org.junit.*;

import static org.junit.Assert.*;

public class TestTerm extends TestCarl {


@Test
public void testToString() {

	assertEquals(t.toString(),"11/2*x^3");

}




@Test
public void testGetters() {
	assertTrue(t.coeff().equals(new Rational(5.5)));
	assertEquals(t.monomial().toString(),"x^3");
}



@Test
public void testOperators() {


	assertEquals(t.add(v1).toString(),"11/2*x^3+x");
	assertEquals(t.add(m).toString(),"13/2*x^3");
	assertEquals(t.add(t).toString(),"11*x^3");
	assertEquals(t.add(p1).toString(),"11/2*x^3+x");
	assertEquals(t.add(new Rational(-5.5)).toString(),"11/2*x^3+(-11/2)");

	assertEquals(t.sub(v1).toString(),"11/2*x^3+(-1)*x");
	assertEquals(t.sub(m).toString(),"9/2*x^3");
	assertEquals(t.sub(t).toString(),"0");
	assertEquals(t.sub(p1).toString(),"11/2*x^3+(-1)*x");
	assertEquals(t.sub(new Rational(-5.5)).toString(),"11/2*x^3+11/2");

	assertEquals(t.mul(v1).toString(),"11/2*x^4");
	assertEquals(t.mul(m).toString(),"11/2*x^6");
	assertEquals(t.mul(t).toString(),"121/4*x^6");
	assertEquals(t.mul(p1).toString(),"11/2*x^4");
	assertEquals(t.mul(new Rational(-5.5)).toString(),"(-121/4)*x^3");

	assertEquals(t.div(v1).toString(),"(11*x^3)/(2*x)");
	assertEquals(t.div(m).toString(),"(11*x^3)/(2*x^3)");
	assertEquals(t.div(t).toString(),"1");
	assertEquals(t.div(p1).toString(),"(11*x^3)/(2*x)");
	assertEquals(t.div(new Rational(-5.5)).toString(),"(-1)*x^3");
	assertEquals(t.div(f).toString(),"((-11)*x^3)/(2*x)");

	assertEquals(t.neg().toString(),"(-11/2)*x^3");
	assertEquals(t.pow(20).toString(),"672749994932560009201/1048576*x^60");

}




}


