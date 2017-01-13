
import jcarl.*;
import org.junit.*;

import static org.junit.Assert.*;

public class TestVariable extends TestCarl {



@Test
public void testToString() {

	assertEquals(v1.toString(),"x");
	assertEquals(v2.toString(),"_i_1");

}




@Test
public void testIdTypeRank() {
	assertEquals(v1.getId(),1);
	assertEquals(v1.getType(),VariableType.VT_REAL);
	assertEquals(v1.getRank(),0);

}

@Test
public void testComparisons() {
	assertEquals(v1.equals(v1),true);
	assertEquals(v1.equals(v2),false);
	assertEquals(v1.notEquals(v1),false);
	assertEquals(v1.notEquals(v2),true);
	assertEquals(v1.less(v1),false);
	assertEquals(v1.less(v2),true);
	assertEquals(v2.less(v1),false);
	assertEquals(v1.lessEquals(v1),true);
	assertEquals(v1.lessEquals(v2),true);
	assertEquals(v2.lessEquals(v1),false);
	assertEquals(v1.greater(v1),false);
	assertEquals(v1.greater(v2),false);
	assertEquals(v2.greater(v1),true);
	assertEquals(v1.greaterEquals(v1),true);
	assertEquals(v1.greaterEquals(v2),false);
	assertEquals(v2.greaterEquals(v1),true);
}


@Test
public void testOperators() {


	assertEquals(v1.add(v1).toString(),"2*x");
	assertEquals(v1.add(m).toString(),"x^3+x");
	assertEquals(v1.add(t).toString(),"11/2*x^3+x");
	assertEquals(v1.add(p1).toString(),"2*x");
	assertEquals(v1.add(new Rational(-5.5)).toString(),"x+(-11/2)");

	assertEquals(v1.sub(v1).toString(),"0");
	assertEquals(v1.sub(m).toString(),"(-1)*x^3+x");
	assertEquals(v1.sub(t).toString(),"(-11/2)*x^3+x");
	assertEquals(v1.sub(p1).toString(),"0");
	assertEquals(v1.sub(new Rational(-5.5)).toString(),"x+11/2");

	assertEquals(v1.mul(v1).toString(),"x^2");
	assertEquals(v1.mul(m).toString(),"x^4");
	assertEquals(v1.mul(t).toString(),"11/2*x^4");
	assertEquals(v1.mul(p1).toString(),"x^2");
	assertEquals(v1.mul(new Rational(-5.5)).toString(),"(-11/2)*x");

	assertEquals(v1.div(v1).toString(),"1");
	assertEquals(v1.div(m).toString(),"(x)/(x^3)");
	assertEquals(v1.div(t).toString(),"(2*x)/(11*x^3)");
	assertEquals(v1.div(p1).toString(),"1");
	assertEquals(v1.div(new Rational(-5.5)).toString(),"(-2/11)*x");
	assertEquals(v1.div(f).toString(),"((-1)*x)/(x)");

	assertEquals(v1.neg().toString(),"(-1)*x");
	assertEquals(v1.pow(20).toString(),"x^20");

}




}


