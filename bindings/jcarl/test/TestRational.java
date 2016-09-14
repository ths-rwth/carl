
import jcarl.*;
import org.junit.*;

import static org.junit.Assert.*;

public class TestRational extends TestCarl {


private static Rational r1,r2;



@BeforeClass
public static void initRational() {
	r1 = new Rational("-1/2");
	r2 = new Rational(10);
}


@Test
public void testToString() {
	assertEquals(r1.toString(),"(-1/2)");
	assertEquals(r2.toString(),"10");
}




@Test
public void testGetters() {
	assertEquals(r1.toDouble(),-0.5,0);
	assertEquals(r1.toInt(),0);
	assertEquals(r1.numerator(),-1);
	assertEquals(r1.nominator(),-1);
	assertEquals(r1.denominator(),2);
}


@Test
public void testComparisonOperators() {
	assertEquals(r1,r1);
	assertTrue(r1.equals(new Rational(-0.5)));
	assertEquals(r1.equals(r1),true);
	assertEquals(r1.equals(r2),false);
	assertEquals(r1.notEquals(r1),false);
	assertEquals(r1.notEquals(r2),true);
	assertEquals(r1.less(r1),false);
	assertEquals(r1.less(r2),true);
	assertEquals(r1.lessEquals(r1),true);
	assertEquals(r2.lessEquals(r1),false);
	assertEquals(r1.greater(r1),false);
	assertEquals(r1.greater(r2),false);
	assertEquals(r1.greaterEquals(r1),true);
	assertEquals(r2.greaterEquals(r1),true);
}



@Test
public void testOperators() {


	assertEquals(r2.add(v1).toString(),"x+10");
	assertEquals(r2.add(m).toString(),"x^3+10");
	assertEquals(r2.add(t).toString(),"11/2*x^3+10");
	assertEquals(r2.add(p1).toString(),"x+10");
	assertEquals(r2.add(new Rational(-5.5)).toString(),"9/2");

	assertEquals(r2.sub(v1).toString(),"(-1)*x+10");
	assertEquals(r2.sub(m).toString(),"(-1)*x^3+10");
	assertEquals(r2.sub(t).toString(),"(-11/2)*x^3+10");
	assertEquals(r2.sub(p1).toString(),"(-1)*x+10");
	assertEquals(r2.sub(new Rational(-5.5)).toString(),"31/2");

	assertEquals(r2.mul(v1).toString(),"10*x");
	assertEquals(r2.mul(m).toString(),"10*x^3");
	assertEquals(r2.mul(t).toString(),"55*x^3");
	assertEquals(r2.mul(p1).toString(),"10*x");
	assertEquals(r2.mul(new Rational(-5.5)).toString(),"(-55)");

	assertEquals(r2.div(v1).toString(),"(10)/(x)");
	assertEquals(r2.div(m).toString(),"(10)/(x^3)");
	assertEquals(r2.div(t).toString(),"(20)/(11*x^3)");
	assertEquals(r2.div(p1).toString(),"(10)/(x)");
	assertEquals(r2.div(new Rational(-5.5)).toString(),"(-20/11)");
	assertEquals(r2.div(f).toString(),"((-10))/(x)");

	assertEquals(r2.neg().toString(),"(-10)");
	assertEquals(r2.pow(20).toString(),"100000000000000000000");
	assertEquals(r1.abs().toString(),"1/2");

}




}


