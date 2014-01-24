Building Carl {#building_carl}
===========

Procedure
------------------
We use [CMake](www.cmake.org) to support the building process. [CMake](www.cmake.org) is a command line tool available for all major platforms. 
To simplify the building process on Unix, we suggest using the GUI for [CMake](www.cmake.org), called [CCMake](http://www.vtk.org/Wiki/CCMake_2.8.11_Docs). 

[CMake](www.cmake.org) generates a Makefile likewise to Autotools' configure.
We suggest initiating this procedure from a separate build directory, called 'out-of-source' building. 
This keeps the source directory free from files created during the building process.

To do so, switch to Carl's root directory. Then, call:

	mkdir build && cd build

Then, call CMake (or CCMake) on the root directory:

	cmake ..


CMake Options for building Carl.
------------------


### General 

- *CMAKE_BUILD_TYPE* [Release, Debug]
  - **Release**
  - **Debug**
- *CMAKE_CXX_COMPILER* \<compiler command\>
- *LOGGING*  [ON, OFF]
  Setting Logging to **OFF** disables all logging output. 
  It is recommended if the performance should be maximized, but notice that this also prevents important warnings and error messages to be generated.

### Debugging

- *DEVELOPER*

