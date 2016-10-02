


from pycarl import *
import unittest

class TestConstraint(TestCarl):


	__sc1 = SimpleConstraintRational(true)
	__sc2 = SimpleConstraintRational(p1,Relation.LESS)
	__scf1 = SimpleConstraintFunc(true)

	cache = FactorizationCache()
	frf = FactorizedRationalFunction(new FactorizedPolynomialRational(p1,cache),new FactorizedPolynomialRational(p2,cache))
	scf2 = SimpleConstraintFunc(frf,Relation.NEQ)


	def testToString(self):
		self.assertEqual(sc1.toString(),'0 = 0')
		self.assertEqual(scf1.toString(),'0 = 0')
		self.assertEqual(c1.toString(),'0=0')


	def testGetters(self):
		self.assertTrue(sc2.lhs().equals(p1))
		self.assertEqual(sc2.rel(),Relation.LESS)
		self.assertTrue(scf2.lhs().equals(frf))
		self.assertEqual(scf2.rel(),Relation.NEQ)
		self.assertTrue(c1.lhs().equals(Polynomial(new Rational(0))))
		self.assertEqual(c2.relation(),Relation.EQ)


	def testEvaluate(self):
		map = VarRationalMap()
		map.set(v11,Rational(10))
		self.assertEqual(c2.satisfiedBy(map),0)
	


	def testExponentsGetItem(self): 
		vector = m.exponents()
		self.assertTrue(vector.get(0).getFirst().equals(v1))
		self.assertEqual(vector.get(0).getSecond(),3)
		self.assertTrue(m.getItem(0).getFirst().equals(v1))
		self.assertEqual(m.getItem(0).getSecond(),3)
	
	


	def testOperators(self):
		self.assertEquals(c2.not_().toString(),'(not (= (+ v1 (- 10)) 0))')
		self.assertEquals(c2.and_(c1).toString(),'(= (+ v1 (- 10)) 0)')
		self.assertEquals(c2.and_(f4).toString(),'(and y (= (+ v1 (- 10)) 0))')
		self.assertEquals(c2.or_(c1).toString(),'true')
		self.assertEquals(c2.or_(f4).toString(),'(or (= (+ v1 (- 10)) 0) (and y (= (+ v1 (- 10)) 0)))')
		self.assertEquals(c2.xor_(c1).toString(),'(not (= (+ v1 (- 10)) 0))')
		self.assertEquals(c2.xor_(f4).toString(),'(xor (= (+ v1 (- 10)) 0) (and y (= (+ v1 (- 10)) 0)))')
}





































}


