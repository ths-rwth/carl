


from pycarl import *
import unittest

class TestFactorizedPolynomial(TestCarl):


	def testToString(self):
		self.assertEqual(fp1.toString(),'2')

	def testGetters(self):
		self.assertTrue(fp1.constant_part().equals(Rational(2)))

	def testEvaluate(self):
		map = VarRationalMap()
		map.set(v1,Rational(2))
		self.assertTrue(fp1.evaluate(map).equals(Rational(2)))


	def testComparisons(self):
		self.assertEqual(fp1.equals(fp1),true)
		self.assertEqual(fp1.equals(fp2),false)
		self.assertEqual(fp1.notEquals(fp1),false)
		self.assertEqual(fp1.notEquals(fp2),true)



	def testOperators(self):
		self.assertEqual(fp1.add(fp2).toString(),'x+2')
		self.assertEqual(fp1.sub(fp2).toString(),'-1 * (x+(-2))')
		self.assertEqual(fp1.mul(fp2).toString(),'2 * (x)')



}




