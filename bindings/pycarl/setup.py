#!/usr/bin/env python
from setuptools import setup
from distutils.core import Extension
from distutils.dist import Distribution
from distutils.command.build_ext import build_ext
import os.path
import platform
from glob import glob

# Glob source files for modules
core_sources = glob(os.path.join('src', 'core', '*.cpp'))
formula_sources = glob(os.path.join('src', 'formula', '*.cpp'))
parse_sources = glob(os.path.join('src', 'parse', '*.cpp'))

# Get rpath from either config file or command line
dist = Distribution()
dist.parse_config_files()
dist.parse_command_line()
rpath = dist.get_option_dict('build_ext')['rpath'][1]

# Configuration shared between external modules follows
include_dirs = ['.', 'src', 'resources/pybind11/include']
libraries = ['carl']
extra_compile_args = ['-std=c++14']
extra_link_args = []
# Mac OS X needs special treatment
if platform.system() == 'Darwin':
    extra_link_args.append('-Wl,-rpath,' + rpath)

ext_core = Extension(
    name='core',
    sources=['src/mod_core.cpp'] + core_sources,
    include_dirs=include_dirs,
    libraries=libraries,
    extra_compile_args=extra_compile_args,
    extra_link_args=extra_link_args,
    runtime_library_dirs=[rpath]
)

ext_formula = Extension(
    name='formula.formula',
    sources=['src/mod_formula.cpp'] + formula_sources,
    include_dirs=include_dirs,
    libraries=libraries,
    extra_compile_args=extra_compile_args,
    extra_link_args=extra_link_args,
    runtime_library_dirs=[rpath]
)

ext_parse = Extension(
    name='parse.parse',
    sources=['src/mod_parse.cpp'] + parse_sources,
    include_dirs=include_dirs,
    libraries=libraries,
    extra_compile_args=extra_compile_args,
    extra_link_args=extra_link_args,
    runtime_library_dirs=[rpath]
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
