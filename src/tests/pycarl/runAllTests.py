import unittest

loader = unittest.TestLoader()
tests = loader.discover(".", pattern = "Test_*.py")

runner = unittest.TextTestRunner()
runner.run(tests)
