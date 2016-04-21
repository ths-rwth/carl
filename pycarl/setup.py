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

BUILD_DIR = "setuppy_build"
if not os.path.exists(BUILD_DIR):
    os.makedirs(BUILD_DIR)

class MyEggInfo(egg_info):
    def run(self):
        try:
            src = os.path.join(PROJECT_DIR, "pycarl")
            dst = os.path.join(BUILD_DIR, "pycarl")
            distutils.dir_util.copy_tree(src, dst)
            egg_info.run(self)
            #shutil.rmtree(os.path.join(BUILD_DIR, "pycarl"))
        except Exception as e:
            print("Exception occurred:\n", str(e))
            #egg_info.run(self)
            raise e


class MyInstall(install):
    user_options = install.user_options + [
        ('cmake=', None, 'Additional cmake arguments'),
        ('make=', None, 'Additional make arguments'),
    ]

    def initialize_options(self):
        install.initialize_options(self)
        self.cmake = ""
        self.make = ""
    
    def run(self):
        # Call cmake
        cmake_args = ["cmake",  "-DCARL_PYTHON=ON", "-DBUILD_STATIC=OFF"]
        cmake_args.extend(self.cmake.split())
        cmake_args.append(PROJECT_DIR)
        ret = call(cmake_args, cwd=BUILD_DIR)
        if ret != 0:
            raise RuntimeError("Failure during cmake")
        
        # Call make
        make_args = ["make", "pycarl", "-j"+str(NO_COMPILE_CORES)]
        make_args.extend(self.make.split())
        ret = call(make_args, cwd=BUILD_DIR)
        if ret != 0:
            raise RuntimeError("Failure during make")
        install.run(self)

class MyDevelop(develop):
    user_options = develop.user_options + [
        ('cmake=', None, 'Additional cmake arguments'),
        ('make=', None, 'Additional make arguments'),
    ]

    def initialize_options(self):
        develop.initialize_options(self)
        self.cmake = ""
        self.make = ""

    def run(self):
        # Call cmake
        cmake_args = ["cmake",  "-DCARL_PYTHON=ON", "-DBUILD_STATIC=OFF", "-DDEVELOPER=ON"]
        cmake_args.extend(self.cmake.split())
        cmake_args.append(PROJECT_DIR)
        ret = call(cmake_args, cwd=BUILD_DIR)
        if ret != 0:
            raise RuntimeError("Failure during cmake")
        
        # Call make
        make_args = ["make", "pycarl", "-j"+str(NO_COMPILE_CORES)]
        make_args.extend(self.make.split())
        ret = call(make_args, cwd=BUILD_DIR)
        if ret != 0:
            raise RuntimeError("Failure during make")
        develop.run(self)

core_sources = glob(os.path.join(PROJECT_DIR, 'src', 'core', '*.cpp'))
formula_sources = glob(os.path.join(PROJECT_DIR, 'src', 'formula', '*.cpp'))
parse_sources = glob(os.path.join(PROJECT_DIR, 'src', 'parse', '*.cpp'))

ext_core = Extension(
    name='_core',
    sources=['src/mod_core.cpp'] + core_sources,
    include_dirs=['src', '../resources/pybind11/include'],
    libraries=['carl'],
    extra_compile_args=['-std=c++11'],
)

ext_formula = Extension(
    name='_formula',
    sources=['src/mod_formula.cpp'] + formula_sources,
    include_dirs=['src', '../resources/pybind11/include'],
    libraries=['carl'],
    extra_compile_args=['-std=c++11'],
)

ext_parse = Extension(
    name='_parse',
    sources=['src/mod_parse.cpp'] + parse_sources,
    include_dirs=['src', '../resources/pybind11/include'],
    libraries=['carl'],
    extra_compile_args=['-std=c++11'],
)

setup(cmdclass={'install': MyInstall, 'develop': MyDevelop, 'egg_info': MyEggInfo},
      name="pycarl",
      version="1.1",
      description="pycarl - Python Bindings for Carl",
      packages=['pycarl', 'pycarl.formula', 'pycarl.parse'],
      package_dir={
          'pycarl':'lib'
      },
      ext_modules=[ext_core, ext_formula],
      include_package_data=True)
