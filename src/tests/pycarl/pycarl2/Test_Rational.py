
from pycarl import *
import unittest

class TestRational(TestCarl):

	__r1 = Rational('-1/2')
	__r2 = Rational(10)
		

	def testToString(self):
		self.assertEqual(r1.toString(),'(-1/2)')
		self.assertEqual(r2.toString(),'10')

	def testGetters(self):
		self.assertEqual(r1.toDouble(),-0.5,0)
		self.assertEqual(r1.toInt(),0)
		self.assertEqual(r1.numerator(),-1)
		self.assertEqual(r1.nominator(),-1)
		self.assertEqual(r1.denominator(),2)


	def testComparisons(self):
		self.assertEqual(r1,r1)
		self.assertTrue(r1.equals(Rational(-0.5)))
		self.assertEqual(r1.equals(r1),true)
		self.assertEqual(r1.equals(r2),false)
		self.assertEqual(r1.notEquals(r1),false)
		self.assertEqual(r1.notEquals(r2),true)
		self.assertEqual(r1.less(r1),false)
		self.assertEqual(r1.less(r2),true)
		self.assertEqual(r1.lessEquals(r1),true)
		self.assertEqual(r2.lessEquals(r1),false)
		self.assertEqual(r1.greater(r1),false)
		self.assertEqual(r1.greater(r2),false)
		self.assertEqual(r1.greaterEquals(r1),true)
		self.assertEqual(r2.greaterEquals(r1),true)



	def testOperators(self):
		self.assertEqual(r2.add(v1).toString(),'x+10')
		self.assertEqual(r2.add(m).toString(),'x^3+10')
		self.assertEqual(r2.add(t).toString(),'11/2*x^3+10')
		self.assertEqual(r2.add(p1).toString(),'x+10')
		self.assertEqual(r2.add(Rational(-5.5)).toString(),'9/2')

		self.assertEqual(r2.sub(v1).toString(),'(-1)*x+10')
		self.assertEqual(r2.sub(m).toString(),'(-1)*x^3+10')
		self.assertEqual(r2.sub(t).toString(),'(-11/2)*x^3+10')
		self.assertEqual(r2.sub(p1).toString(),'(-1)*x+10')
		self.assertEqual(r2.sub(Rational(-5.5)).toString(),'31/2')

		self.assertEqual(r2.mul(v1).toString(),'10*x')
		self.assertEqual(r2.mul(m).toString(),'10*x^3')
		self.assertEqual(r2.mul(t).toString(),'55*x^3')
		self.assertEqual(r2.mul(p1).toString(),'10*x')
		self.assertEqual(r2.mul(Rational(-5.5)).toString(),'(-55)')

		self.assertEqual(r2.div(v1).toString(),'(10)/(x)')
		self.assertEqual(r2.div(m).toString(),'(10)/(x^3)')
		self.assertEqual(r2.div(t).toString(),'(20)/(11*x^3)')
		self.assertEqual(r2.div(p1).toString(),'(10)/(x)')
		self.assertEqual(r2.div(Rational(-5.5)).toString(),'(-20/11)')
		self.assertEqual(r2.div(f).toString(),'((-10))/(x)')

		self.assertEqual(r2.neg().toString(),'(-10)')
		self.assertEqual(r2.pow(20).toString(),'100000000000000000000')
		self.assertEqual(r1.abs().toString(),'1/2')


}






