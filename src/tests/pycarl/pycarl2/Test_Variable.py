from pycarl import *
import unittest

class TestVariable(TestCarl):


	def testToString(self):
		self.assertEqual(v1.toString(),'x')
		self.assertEqual(v2.toString(),'_i_1')


	def testGetters(self):
		self.assertEqual(v1.getId(),1)
		self.assertEqual(v1.getType(),VariableType.VT_REAL)
		self.assertEqual(v1.getRank(),0)

	def testComparisons(self):
		self.assertTrue(v1.equals(v1))
		self.assertFalse(v1.equals(v2))
		self.assertFalse(v1.notEquals(v1))
		self.assertTrue(v1.notEquals(v2))
		self.assertFalse(v1.less(v1))
		self.assertTrue(v1.less(v2)
		self.assertFalse(v2.less(v1))
		self.assertTrue(v1.lessEquals(v1))
		self.assertTrue(v1.lessEquals(v2))
		self.assertFalse(v2.lessEquals(v1))
		self.assertFalse(v1.greater(v1))
		self.assertFalse(v1.greater(v2))
		self.assertTrue(v2.greater(v1))
		self.assertTrue(v1.greaterEquals(v1))
		self.assertFalse(v1.greaterEquals(v2))
		self.assertTrue(v2.greaterEquals(v1))



	def testOperators(self):
		self.assertEquals(v1.add(v1).toString(),'2*x')
		self.assertEquals(v1.add(m).toString(),'x^3+x')
		self.assertEquals(v1.add(t).toString(),'11/2*x^3+x')
		self.assertEquals(v1.add(p1).toString(),'2*x')
		self.assertEquals(v1.add(Rational(-5.5)).toString(),'x+(-11/2)')

		self.assertEquals(v1.sub(v1).toString(),'0')
		self.assertEquals(v1.sub(m).toString(),'(-1)*x^3+x')
		self.assertEquals(v1.sub(t).toString(),'(-11/2)*x^3+x')
		self.assertEquals(v1.sub(p1).toString(),'0')
		self.assertEquals(v1.sub(Rational(-5.5)).toString(),'x+11/2')

		self.assertEquals(v1.mul(v1).toString(),'x^2')
		self.assertEquals(v1.mul(m).toString(),'x^4')
		self.assertEquals(v1.mul(t).toString(),'11/2*x^4')
		self.assertEquals(v1.mul(p1).toString(),'x^2')
		self.assertEquals(v1.mul(Rational(-5.5)).toString(),'(-11/2)*x')

		self.assertEquals(v1.div(v1).toString(),'1')
		self.assertEquals(v1.div(m).toString(),'(x)/(x^3)')
		self.assertEquals(v1.div(t).toString(),'(2*x)/(11*x^3)')
		self.assertEquals(v1.div(p1).toString(),'1')
		self.assertEquals(v1.div(Rational(-5.5)).toString(),'(-2/11)*x')
		self.assertEquals(v1.div(f).toString(),'((-1)*x)/(x)')

		self.assertEquals(v1.neg().toString(),'(-1)*x')
		self.assertEquals(v1.pow(20).toString(),'x^20')

}








