
import jcarl.*;
import org.junit.*;

import static org.junit.Assert.*;

public class TestInterval extends TestCarl {


private static IntervalRational i1,i2;


@BeforeClass
public static void initInterval() {
	i1 = new IntervalRational(new Rational(0));
	i2 = new IntervalRational(new Rational(-100),BoundType.WEAK,new Rational(100),BoundType.INFTY);

}



@Test
public void testToString() {
	assertEquals(i1.toString(),"[0, 0]");
}





@Test
public void testGetters() {
	assertTrue(i1.lower().equals(new Rational(0)));
	assertTrue(i1.upper().equals(new Rational(0)));
}


@Test
public void testIsMethods() {
	assertEquals(i1.isInfinite(),false);
	assertEquals(i1.isUnbounded(),false);
	assertEquals(i1.isHalfBounded(),false);
	assertEquals(i1.isEmpty(),false);
	assertEquals(i1.isPointInterval(),true);
	assertEquals(i1.isOpenInterval(),false);
	assertEquals(i1.isClosedInterval(),true);
	assertEquals(i1.isZero(),true);
	assertEquals(i1.isOne(),false);
	assertEquals(i1.isPositive(),false);
	assertEquals(i1.isNegative(),false);
	assertEquals(i1.isSemiPositive(),true);
	assertEquals(i1.isSemiNegative(),true);
}

@Test
public void testSetBounds() {
	IntervalRational i = new IntervalRational(new Rational(-5),new Rational(5));
	i.setLower(new Rational(-1));
	i.setUpper(new Rational(1));
	assertTrue(i.lower().equals(new Rational(-1)));
	assertTrue(i.upper().equals(new Rational(1)));
}




@Test
public void testSomeMoreMethods() {
	assertTrue(i1.integralPart().lower().equals(i1.lower()));
	assertTrue(i1.integralPart().upper().equals(i1.upper()));
	assertTrue(i1.diameter().equals(new Rational(0)));
	assertTrue(i1.center().equals(new Rational(0)));
	assertTrue(i1.sample().equals(new Rational(0)));
}



@Test
public void testRelationalOperators() {


	assertEquals(i1.contains(new Rational(1000)),false);
	assertEquals(i2.contains(i1),true);
	assertEquals(i1.meets(new Rational(1)),false);
	assertEquals(i1.isSubset(i2),true);
	assertEquals(i1.isProperSubset(i2),true);
	assertEquals(i1.intersectsWith(i2),true);

}


@Test
public void testSomeMoreOperators() {

	IntervalRational result = i2.div(i1);
	assertTrue(result.lower().equals(new Rational(0)));
	assertTrue(result.upper().equals(new Rational(0)));
	result = i2.inverse();
	assertTrue(result.upper().equals(new Rational(100)));
	assertEquals(result.isHalfBounded(),true);
	result = i2.intersect(i1);
	assertTrue(result.lower().equals(new Rational(0)));
	assertTrue(result.upper().equals(new Rational(0)));
	result = i2.abs();
	assertTrue(result.lower().equals(new Rational(0)));
	assertTrue(result.upper().equals(new Rational(0)));
	result = i1.pow(2);
	assertTrue(result.lower().equals(new Rational(0)));
	assertTrue(result.upper().equals(new Rational(0)));
}


@Test
public void test2ResultsMethods() {

	IntervalRational res1 = new IntervalRational(new Rational(0));
	IntervalRational res2 = new IntervalRational(new Rational(0));
	i1.unite(i2,res1,res2);
	assertTrue(res1.lower().equals(i2.lower()));
	assertTrue(res1.upper().equals(i2.upper()));
	i2.difference(i1,res1,res2);
	assertTrue(res1.lower().equals(i2.lower()));
	assertTrue(res1.upper().equals(new Rational(0)));
	assertTrue(res2.isHalfBounded());
	assertTrue(res2.lower().equals(new Rational(0)));
	i1.complement(res1,res2);
	assertTrue(res1.upper().equals(new Rational(0)));
	assertTrue(res2.lower().equals(new Rational(0)));
	assertEquals(res1.isHalfBounded(),true);
	assertEquals(res2.isHalfBounded(),true);
	i2.symmetricDifference(i1,res1,res2);
	assertTrue(res1.lower().equals(i2.lower()));
	assertTrue(res1.upper().equals(new Rational(0)));
	assertTrue(res2.isHalfBounded());
	assertTrue(res2.lower().equals(new Rational(0)));
	
}


@Test
public void testStaticMethods() {
	IntervalRational result = IntervalRational.floor(i1);
	assertTrue(result.lower().equals(new Rational(0)));
	assertTrue(result.upper().equals(new Rational(0)));
	result = IntervalRational.ceil(i1);
	assertTrue(result.lower().equals(new Rational(0)));
	assertTrue(result.upper().equals(new Rational(0)));
	result = IntervalRational.quotient(i2,i1);
	assertTrue(result.lower().equals(new Rational(0)));
	assertTrue(result.upper().equals(new Rational(0)));
	assertEquals(IntervalRational.isInteger(i2),false);

}







}


