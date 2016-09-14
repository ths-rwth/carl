
import jcarl.*;
import org.junit.*;

import static org.junit.Assert.*;

public class TestConstraint extends TestCarl {


private static SimpleConstraintRational sc1,sc2;
private static SimpleConstraintFunc scf1, scf2;
public static FactorizedRationalFunction frf;

@BeforeClass
public static void initConstraint() {
    sc1 = new SimpleConstraintRational(true);
    sc2 = new SimpleConstraintRational(p1,Relation.LESS);
    scf1 = new SimpleConstraintFunc(true);

    FactorizationCache cache = new FactorizationCache();
    frf = new FactorizedRationalFunction(new FactorizedPolynomialRational(p1,cache),new FactorizedPolynomialRational(p2,cache));
    scf2 = new SimpleConstraintFunc(frf,Relation.NEQ);
}


@Test
public void testToString() {
	assertEquals(sc1.toString(),"0 = 0");
	assertEquals(scf1.toString(),"0 = 0");
	assertEquals(c1.toString(),"0=0");
}




@Test
public void testGetters() {
	assertTrue(sc2.lhs().equals(p1));
	assertEquals(sc2.rel(),Relation.LESS);
	assertTrue(scf2.lhs().equals(frf));
	assertEquals(scf2.rel(),Relation.NEQ);
	assertTrue(c1.lhs().equals(new Polynomial(new Rational(0))));
	assertEquals(c2.relation(),Relation.EQ);
}


@Test
public void testEvaluate() {
	VarRationalMap map = new VarRationalMap();
	map.set(v11,new Rational(10));
	assertEquals(c2.satisfiedBy(map),1);
}


@Test
public void testOperators() {

	assertEquals(c2.not_().toString(),"(not (= (+ v1 (- 10)) 0))");
	assertEquals(c2.and_(c1).toString(),"(= (+ v1 (- 10)) 0)");
	assertEquals(c2.and_(f4).toString(),"(and y (= (+ v1 (- 10)) 0))");
	assertEquals(c2.or_(c1).toString(),"true");
	assertEquals(c2.or_(f4).toString(),"(or (= (+ v1 (- 10)) 0) (and y (= (+ v1 (- 10)) 0)))");
	assertEquals(c2.xor_(c1).toString(),"(not (= (+ v1 (- 10)) 0))");
	assertEquals(c2.xor_(f4).toString(),"(xor (= (+ v1 (- 10)) 0) (and y (= (+ v1 (- 10)) 0)))");
	
}




}


