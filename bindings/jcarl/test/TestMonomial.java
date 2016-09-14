
import jcarl.*;
import org.junit.*;

import static org.junit.Assert.*;

public class TestMonomial extends TestCarl {



@Test
public void testToString() {
	assertEquals(m.toString(),"x^3");
}





@Test
public void testGetters() {
	assertEquals(m.nrVariables(),1);
	assertEquals(m.tdeg(),3);
}

@Test
public void testExponentsGetItem() {
	VarIntPairVector vector = m.exponents();
	assertTrue(vector.get(0).getFirst().equals(v1));
	assertEquals(vector.get(0).getSecond(),3);
	assertTrue(m.getItem(0).getFirst().equals(v1));
	assertEquals(m.getItem(0).getSecond(),3);
	
}


@Test
public void testOperators() {


	assertEquals(m.add(v1).toString(),"x^3+x");
	assertEquals(m.add(m).toString(),"2*x^3");
	assertEquals(m.add(t).toString(),"13/2*x^3");
	assertEquals(m.add(p1).toString(),"x^3+x");
	assertEquals(m.add(new Rational(-5.5)).toString(),"x^3+(-11/2)");

	assertEquals(m.sub(v1).toString(),"x^3+(-1)*x");
	assertEquals(m.sub(m).toString(),"(-1)*x^3+x^3");
	assertEquals(m.sub(t).toString(),"(-11/2)*x^3+x^3");
	assertEquals(m.sub(p1).toString(),"x^3+(-1)*x");
	assertEquals(m.sub(new Rational(-5.5)).toString(),"x^3+11/2");

	assertEquals(m.mul(v1).toString(),"x^4");
	assertEquals(m.mul(m).toString(),"x^6");
	assertEquals(m.mul(t).toString(),"11/2*x^6");
	assertEquals(m.mul(p1).toString(),"x^4");
	assertEquals(m.mul(new Rational(-5.5)).toString(),"(-11/2)*x^3");

	assertEquals(m.div(v1).toString(),"(x^3)/(x)");
	assertEquals(m.div(m).toString(),"(x^3)/(x^3)");
	assertEquals(m.div(t).toString(),"(2*x^3)/(11*x^3)");
	assertEquals(m.div(p1).toString(),"(x^3)/(x)");
	assertEquals(m.div(new Rational(-5.5)).toString(),"(-2/11)*x^3");
	assertEquals(m.div(f).toString(),"((-1)*x^3)/(x)");

	assertEquals(m.neg().toString(),"(-1)*x^3");
	assertEquals(m.pow(20).toString(),"x^60");

}




}


