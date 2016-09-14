
import jcarl.*;
import org.junit.*;

import static org.junit.Assert.*;

public class TestFactorizedPolynomial extends TestCarl {





@Test
public void testToString() {
	assertEquals(fp1.toString(),"2");
}





@Test
public void testGetters() {
	assertTrue(fp1.constantPart().equals(new Rational(2)));
}

@Test
public void testEvaluate() {
	VarRationalMap map = new VarRationalMap();
	map.set(v1,new Rational(2));
	assertTrue(fp1.evaluate(map).equals(new Rational(2)));
}


@Test
public void testComparisonOperators() {
	assertEquals(fp1.equals(fp1),true);
	assertEquals(fp1.equals(fp2),false);
	assertEquals(fp1.notEquals(fp1),false);
	assertEquals(fp1.notEquals(fp2),true);
}



@Test
public void testOperators() {

	assertEquals(fp1.add(fp2).toString(),"x+2");

	assertEquals(fp1.sub(fp2).toString(),"-1 * (x+(-2))");

	assertEquals(fp1.mul(fp2).toString(),"2 * (x)");


}




}


