#!/usr/bin/env python
from setuptools import setup
from distutils.core import Extension
from distutils.command.build_ext import build_ext
import os.path
from glob import glob

PROJECT_DIR = os.path.abspath(os.path.dirname(os.path.realpath(__file__)))

# Glob source files for modules
core_sources = glob(os.path.join('src', 'core', '*.cpp'))
formula_sources = glob(os.path.join('src', 'formula', '*.cpp'))
parse_sources = glob(os.path.join('src', 'parse', '*.cpp'))

# Configuration shared between external modules follows

# To help along, if carl and/or pybind is not system installed, retrieve from carl distribution
include_dirs = ['.', 'src']
local_carl_path = os.path.join(PROJECT_DIR, '..', 'src')
if os.path.exists(local_carl_path):
    include_dirs.append(local_carl_path)
local_pybind_path = os.path.join(PROJECT_DIR, '..', 'resources', 'pybind11', 'include')
if os.path.exists(local_pybind_path):
    include_dirs.append(local_pybind_path)

# Like includes, also add local path for library, assuming made in 'build'
library_dirs = []
local_carl_lib_path = os.path.join(PROJECT_DIR, '..', 'build')
if os.path.exists(local_carl_lib_path):
    library_dirs.append(local_carl_lib_path)

libraries = ['carl']
extra_compile_args = ['-std=c++11']
define_macros = []

ext_core = Extension(
    name='core',
    sources=['src/mod_core.cpp'] + core_sources,
    include_dirs=include_dirs,
    libraries=libraries,
    library_dirs=library_dirs,
    extra_compile_args=extra_compile_args,
    define_macros=define_macros
)

ext_formula = Extension(
    name='formula.formula',
    sources=['src/mod_formula.cpp'] + formula_sources,
    include_dirs=include_dirs,
    libraries=libraries,
    library_dirs=library_dirs,
    extra_compile_args=extra_compile_args,
    define_macros=define_macros
)

ext_parse = Extension(
    name='parse.parse',
    sources=['src/mod_parse.cpp'] + parse_sources,
    include_dirs=include_dirs,
    libraries=libraries,
    library_dirs=library_dirs,
    extra_compile_args=extra_compile_args,
    define_macros=define_macros
)

class pycarl_build_ext(build_ext):
    """Extend build_ext to provide CLN toggle option
    """
    user_options = build_ext.user_options + [
        ('use-cln', None,
         "use cln numbers instead of gmpxx")
        ]

    def __init__(self, *args, **kwargs):
        build_ext.__init__(self, *args, **kwargs)

    def initialize_options (self):
        build_ext.initialize_options(self)
        self.use_cln = None

    def finalize_options(self):
        build_ext.finalize_options(self)

        if self.use_cln:
            self.libraries += ['cln']
            if not self.define:
                self.define = []
            else:
                self.define = list(self.define)
            self.define += [('PYCARL_USE_CLN', 1)]
        else:
            self.libraries += ['gmp', 'gmpxx']
            if not self.undef:
                self.undef = []
            self.undef += ['PYCARL_USE_CLN']

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
      ext_modules=[ext_core, ext_formula #, ext_parse
                   ],
      cmdclass={
        'build_ext': pycarl_build_ext,
      }
)
