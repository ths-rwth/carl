#!/usr/bin/env python
from setuptools import setup
from distutils.core import Extension
from distutils.command.build_ext import build_ext
import os.path
import platform
from glob import glob

PROJECT_DIR = os.path.abspath(os.path.dirname(os.path.realpath(__file__)))

# Glob source files for modules
core_sources = glob(os.path.join('src', 'core', '*.cpp'))
formula_sources = glob(os.path.join('src', 'formula', '*.cpp'))
parse_sources = glob(os.path.join('src', 'parse', '*.cpp'))

# Configuration shared between external modules follows
include_dirs = ['.', 'src', 'resources/pybind11/include']
libraries = ['carl']
extra_compile_args = ['-std=c++11']

# TODO: Hardcoded rpath for Darwin
extra_link_args = []
if platform.system() == 'Darwin':
    extra_link_args.append('-Wl,-rpath,' + os.path.join(PROJECT_DIR, '..', 'build'))

ext_core = Extension(
    name='core',
    sources=['src/mod_core.cpp'] + core_sources,
    include_dirs=include_dirs,
    libraries=libraries,
    extra_compile_args=extra_compile_args,
    extra_link_args=extra_link_args
)

ext_formula = Extension(
    name='formula.formula',
    sources=['src/mod_formula.cpp'] + formula_sources,
    include_dirs=include_dirs,
    libraries=libraries,
    extra_compile_args=extra_compile_args,
    extra_link_args=extra_link_args
)

ext_parse = Extension(
    name='parse.parse',
    sources=['src/mod_parse.cpp'] + parse_sources,
    include_dirs=include_dirs,
    libraries=libraries,
    extra_compile_args=extra_compile_args,
    extra_link_args=extra_link_args
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
      package_dir={'':'lib'},
      ext_package='pycarl',
      ext_modules=[ext_core, ext_formula, ext_parse],
)
