#!/usr/bin/python
from setuptools import setup, find_packages
from setuptools.command.install import install
from setuptools.command.develop import develop
from setuptools.command.egg_info import egg_info
from subprocess import call, STDOUT
import distutils.sysconfig
import os
import os.path
import shutil
import distutils
import multiprocessing
from distutils.core import Extension
from glob import glob

NO_COMPILE_CORES = multiprocessing.cpu_count()

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
    name='formula',
    sources=['src/mod_formula.cpp'] + formula_sources,
    include_dirs=['.', 'src', '../resources/pybind11/include'],
    libraries=['carl'],
    extra_compile_args=['-std=c++11'],
)

ext_parse = Extension(
    name='parse',
    sources=['src/mod_parse.cpp'] + parse_sources,
    include_dirs=['.', 'src', '../resources/pybind11/include'],
    libraries=['carl'],
    extra_compile_args=['-std=c++11'],
)

setup(name="pycarl",
      version="1.1",
      description="pycarl - Python Bindings for Carl",
      packages=['pycarl', 'pycarl.formula', 'pycarl.parse'],
      package_dir={
          'pycarl':'lib'
      },
      ext_modules=[ext_core, ext_formula],
)
