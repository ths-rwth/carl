#!/usr/bin/python
from setuptools import setup
from distutils.core import Extension
import os.path
from glob import glob

PROJECT_DIR = os.path.abspath(os.path.dirname(os.path.realpath(__file__)))

core_sources = glob(os.path.join('src', 'core', '*.cpp'))
formula_sources = glob(os.path.join('src', 'formula', '*.cpp'))
parse_sources = glob(os.path.join('src', 'parse', '*.cpp'))

ext_core = Extension(
    name='core',
    sources=['src/mod_core.cpp'] + core_sources,
    include_dirs=['.', 'src', '../resources/pybind11/include'],
    libraries=['carl'],
    extra_compile_args=['-std=c++11'],
)

ext_formula = Extension(
    name='formula.formula',
    sources=['src/mod_formula.cpp'] + formula_sources,
    include_dirs=['.', 'src', '../resources/pybind11/include'],
    libraries=['carl'],
    extra_compile_args=['-std=c++11'],
)

ext_parse = Extension(
    name='parse.parse',
    sources=['src/mod_parse.cpp'] + parse_sources,
    include_dirs=['.', 'src', '../resources/pybind11/include'],
    libraries=['carl'],
    extra_compile_args=['-std=c++11'],
)

setup(name="pycarl",
      version="1.1",
      author="H. Bruintjes",
      author_email="h.bruintjes@cs.rwth-aachen.de",
      maintainer="S. Junges",
      maintainer_email="sebastian.junges@cs.rwth-aachen.de",
      url="http://moves.rwth-aachen.de",
      description="pycarl - Python Bindings for Carl",
      packages=['pycarl', 'pycarl.formula', 'pycarl.parse'],
      package_dir={
          'pycarl':'lib'
      },
      ext_package='pycarl',
      ext_modules=[ext_core, ext_formula],
)
