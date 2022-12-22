Building with CMake {#build_cmake}
===========

We use [CMake](http://www.cmake.org) to support the building process. CMake is a command line tool available for all major platforms. 
To simplify the building process on Unix, we suggest using  [CCMake](http://www.vtk.org/Wiki/CCMake_2.8.11_Docs). 

CMake generates a Makefile likewise to Autotools' configure.
We suggest initiating this procedure from a separate build directory, called 'out-of-source' building. 
This keeps the source directory free from files created during the building process.

CMake Options for building CArL.
------------

Run `ccmake` to obtain a list of all available options or change them.
@code
$ cd build/
$ ccmake ../
@endcode

Using `[t]`, you can enable the _advanced mode_ that shows all options. Most of these should not be changed by the average user.

### General 

- *CMAKE_BUILD_TYPE* [Release, Debug]
  - *Release*
  - *Debug*
  
- *CMAKE_CXX_COMPILER* \<compiler command\>
  - `/usr/bin/c++`: Default for most linux distributions, will probably be an alias for `g++`.
  - `/usr/bin/g++`: Uses `g++`.
  - `/usr/bin/clang++`: Uses `clang`.

- *USE_CLN_NUMBERS* [ON, OFF] <br>
  If set to *ON*, CLN number types can be used in addition to GMP number types.

- *USE_COCOA* [ON, OFF] <br />
  If set to *ON*, CoCoALib can be used for advanced polynomial operations, for example multivariate gcd or factorization.

- *USE_GINAC* [ON, OFF] <br>
  If set to *ON*, GiNaC can be used for some polynomial operations. Note that this implies *USE_CLN_NUMBERS = ON*.

### Debugging

- *DEVELOPER* <br>
  Enables additional compiler warnings.

- *LOGGING* [ON, OFF] <br>
  Setting *LOGGING* to *OFF* disables all logging output. 
  It is recommended if the performance should be maximized, but notice that this also prevents important warnings and error messages to be generated.

CMake Targets
------------

There are a few important targets in the CArL CMakeLists:

- `doc`: Builds the doxygen documentation.
- `libs`: Builds all libraries.
- `runXTests`: Builds the tests for the `X` module.
- `test`: Build and run all tests.
