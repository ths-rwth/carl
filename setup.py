from setuptools import setup, find_packages
from setuptools.command.install import install
from setuptools.command.develop import develop
from setuptools.command.egg_info import egg_info
from subprocess import call, STDOUT
import distutils.sysconfig
import os
import os.path
import tempfile
import glob
import shutil
import distutils
import multiprocessing
print(os.getcwd())

PYTHONINC = distutils.sysconfig.get_python_inc()
PYTHONLIB = distutils.sysconfig.get_python_lib(plat_specific=True, standard_lib=True)
PYTHONLIBDIR = distutils.sysconfig.get_config_var("LIBDIR")
PYTHONLIBS = glob.glob(os.path.join(PYTHONLIBDIR, "*.dylib"))
PYTHONLIBS.extend(glob.glob(os.path.join(PYTHONLIBDIR, "*.so")))
PYTHONLIB = PYTHONLIBS[0]
#print(os.path.abspath(os.path.join(os.path.dirname(os.path.realpath(__file__)), os.pardir)))
print(PYTHONINC)
print(PYTHONLIB)

NO_COMPILE_CORES = multiprocessing.cpu_count()

d = "setuppy_build"
print(d)
if not os.path.exists(d):
    os.makedirs(d)

class MyEggInfo(egg_info):
    def run(self):
        try:
            src = os.path.join(d, "../pycarl")
            dst = os.path.join(d, "pycarl/")
            distutils.dir_util.copy_tree(src, dst)
            egg_info.run(self)
            #shutil.rmtree(os.path.join(d, "pycarl"))
        except:
            print("Exception occurred")
            egg_info.run(self)


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
        cmake_args = ["cmake",  "-DUSE_GINAC=ON", "-DCARL_PYTHON=ON", "-DBUILD_STATIC=OFF",  "-DPYTHON_LIBRARY="+PYTHONLIB, "-DPYTHON_INCLUDE_DIR="+PYTHONINC]
        cmake_args.extend(self.cmake.split())
        cmake_args.append(os.path.abspath(os.path.dirname(os.path.realpath(__file__))))
        ret = call(cmake_args, cwd=d)
        if ret != 0:
            raise RuntimeError("Failure during cmake")
        
        # Call make
        make_args = ["make", "pycarl", "-j"+str(NO_COMPILE_CORES)]
        make_args.extend(self.make.split())
        ret = call(make_args, cwd=d)
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
        cmake_args = ["cmake",  "-DUSE_GINAC=ON", "-DCARL_PYTHON=ON", "-DBUILD_STATIC=OFF",  "-DPYTHON_LIBRARY="+PYTHONLIB, "-DPYTHON_INCLUDE_DIR="+PYTHONINC]
        cmake_args.extend(self.cmake.split())
        cmake_args.append(os.path.abspath(os.path.dirname(os.path.realpath(__file__))))
        ret = call(cmake_args, cwd=d)
        if ret != 0:
            raise RuntimeError("Failure during cmake")
        
        # Call make
        make_args = ["make", "pycarl", "-j"+str(NO_COMPILE_CORES)]
        make_args.extend(self.make.split())
        ret = call(make_args, cwd=d)
        if ret != 0:
            raise RuntimeError("Failure during make")
        develop.run(self)


setup(cmdclass={'install': MyInstall, 'develop': MyDevelop, 'egg_info': MyEggInfo},
      name="pycarl",
      version="1.1",
      description="pycarl - Python Bindings for Carl",
      package_dir={'':d},
      packages=['pycarl', 'pycarl.core', 'pycarl.numbers'],
      package_data={'pycarl.core': ['_core.so'], 'pycarl.numbers' : ['_numbers.so'], 'pycarl': ['*.so', '*.dylib', '*.a']},

      include_package_data=True)
