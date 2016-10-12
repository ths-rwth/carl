import os
import unittest

basedir = os.path.dirname(os.path.realpath(__file__))
loader = unittest.TestLoader()
tests = loader.discover(basedir, pattern = "Test_*.py")

runner = unittest.TextTestRunner()
runner.run(tests)
