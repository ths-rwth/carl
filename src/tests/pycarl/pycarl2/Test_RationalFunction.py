

from pycarl import *
import unittest

class TestRationalFunction(TestCarl):

	__frf1 = FactorizedRationalFunction(fp1,fp2)


	def testToString(self):
		self.assertEqual(f.toString(),'((-1)*x)/(1)')
		self.assertEqual(frf1.toString(),'(2)/(x)')


	def testGetters(self):
		self.assertTrue(f.nominator().equals(Polynomial(v1.neg())))
		self.assertTrue(f.numerator().equals(Polynomial(v1.neg())))
		self.assertTrue(f.denominator().equals(Polynomial (Rational(1))))
		self.assertTrue(frf1.nominator().equals(fp1))
		self.assertTrue(frf1.numerator().equals(fp1))
		self.assertTrue(frf1.denominator().equals(fp2))

	def testEvaluate(self):
		map = VarRationalMap()
		map.set(v1,Rational(2))
		self.assertTrue(f.evaluate(map).equals(Rational(-2)))
		self.assertTrue(frf1.evaluate(map).equals(Rational(1)))

	def testGatherVariables(self):
		vars = f.gatherVariables()
		self.assertTrue(vars.get(0).equals(v1))
		self.assertEquals(vars.size(),1)
		vars = frf1.gatherVariables()
		self.assertTrue(vars.get(0).equals(v1))
		self.assertEquals(vars.size(),1)



	def testComparisons(self):
		f2 = RationalFunction(p4,p1)
		self.assertEquals(f.equals(f),true)
		self.assertEquals(f.equals(f2),false)
		self.assertEquals(f.notEquals(f),false)	
		self.assertEquals(f.notEquals(f2),true)



	def testOperators(self):
		self.assertEqual(f.add(v1).toString(),'0')
		self.assertEqual(f.add(m).toString(),'(x^3+(-1)*x)/(1)')
		self.assertEqual(f.add(t).toString(),'(11*x^3+(-2)*x)/(2)')
		self.assertEqual(f.add(p1).toString(),'0')
		self.assertEqual(f.add(Rational(-5.5)).toString(),'((-2)*x+(-11))/(2)')

		self.assertEqual(f.sub(v1).toString(),'((-2)*x)/(1)')
		self.assertEqual(f.sub(m).toString(),'((-1)*x^3+(-1)*x)/(1)')
		self.assertEqual(f.sub(t).toString(),'((-11)*x^3+(-2)*x)/(2)')
		self.assertEqual(f.sub(p1).toString(),'((-2)*x)/(1)')
		self.assertEqual(f.sub(Rational(-5.5)).toString(),'((-2)*x+11)/(2)')

		self.assertEquals(f.mul(v1).toString(),'((-1)*x^2)/(1)')
		self.assertEquals(f.mul(m).toString(),'((-1)*x^4)/(1)')
		self.assertEquals(f.mul(t).toString(),'((-11)*x^4)/(2)')
		self.assertEquals(f.mul(p1).toString(),'((-1)*x^2)/(1)')
		self.assertEquals(f.mul(Rational(-5.5)).toString(),'(11*x)/(2)')

		self.assertEquals(f.div(v1).toString(),'((-1)*x)/(x)')
		self.assertEquals(f.div(m).toString(),'((-1)*x)/(x^3)')
		self.assertEquals(f.div(t).toString(),'((-2)*x)/(11*x^3)')
		self.assertEquals(f.div(p1).toString(),'((-1)*x)/(x)')
		self.assertEquals(f.div(Rational(-5.5)).toString(),'(2/11*x)/(1)')
		self.assertEquals(f.div(f).toString(),'1')

		self.assertEquals(f.neg().toString(),'(x)/(1)')
		self.assertEquals(f.pow(20).toString(),'(x^20)/(1)')


}





