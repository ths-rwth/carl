
import jcarl.*;
import org.junit.*;

import static org.junit.Assert.*;

public class TestFormula extends TestCarl {



@Test
public void testToString() {
	assertEquals(f3.toString(),"(not (= (+ v1 (- 10)) 0))");
}


@Test
public void testGetters() {
	assertEquals(f4.size(),2);
	assertEquals(f4.getType(),FormulaType.AND);
}

@Test
public void testGetItem() {
	assertTrue(f4.getItem(1).equals(f2));
}


@Test
public void testOperators() {

	assertEquals(f2.not_().toString(),"(not (= (+ v1 (- 10)) 0))");
	assertEquals(f2.and_(c1).toString(),"(= (+ v1 (- 10)) 0)");
	assertEquals(f2.and_(f4).toString(),"(and y (= (+ v1 (- 10)) 0))");
	assertEquals(f2.or_(c1).toString(),"true");
	assertEquals(f2.or_(f4).toString(),"(or (= (+ v1 (- 10)) 0) (and y (= (+ v1 (- 10)) 0)))");
	assertEquals(f2.xor_(c1).toString(),"(not (= (+ v1 (- 10)) 0))");
	assertEquals(f2.xor_(f4).toString(),"(xor (= (+ v1 (- 10)) 0) (and y (= (+ v1 (- 10)) 0)))");
	

}




}


