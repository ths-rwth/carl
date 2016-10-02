


from pycarl import *
import unittest

class TestFormula(TestCarl):

	def testToString(self):
		self.assertEqual(f3.toString(),'(not (= (+ v1 (- 10)) 0))')


	def testGetters(self):
		self.assertEqual(f4.size(),2)
		self.assertEqual(f4.getType(),FormulaType.AND)

	def testGetItem(self):
		self.assertTrue(f4.getItem(1).equals(f2))


	def testOperators(self):
		self.assertEqual(f2.not_().toString(),'(not (= (+ v1 (- 10)) 0))')
		self.assertEqual(f2.and_(c1).toString(),'(= (+ v1 (- 10)) 0)')
		self.assertEqual(f2.and_(f4).toString(),'(and y (= (+ v1 (- 10)) 0))')
		self.assertEqual(f2.or_(c1).toString(),'true')
		self.assertEqual(f2.or_(f4).toString(),'(or (= (+ v1 (- 10)) 0) (and y (= (+ v1 (- 10)) 0)))')
		self.assertEqual(f2.xor_(c1).toString(),'(not (= (+ v1 (- 10)) 0))')
		self.assertEqual(f2.xor_(f4).toString(),'(xor (= (+ v1 (- 10)) 0) (and y (= (+ v1 (- 10)) 0)))')
}









