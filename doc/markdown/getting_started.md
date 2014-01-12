Getting Started {#getting_started}
=======

Dependencies
--------------------------------------------
This library has the following dependencies:

REQUIRED:
- boost +

OPTIONAL:
- cln
- gmp

- log4cplus +
- googletest +
- doxygen

Libraries marked with a plus (+) are included in the resources directory.
You can find more about them in [External Resources](@ref extres).

Installation 
--------------------------------------------
Start with:

	mkdir build && cd build && cmake ..

To compile dependencies (1) not found in the system AND (2) which are included in the resources:

	make resources

To compile the arithmetic library:

	make

To run some unittests:

	make test

To compile examples:
	
	make examples

To install:

	make install
 
Including carl
--------------------------------------------
Notice that for other CMake-projects, carl registers itself in the CMake system, 
which simplifies finding carl to a simple find_package(carl), no matter if carl is installed or not.


Supported platforms
--------------------------------------------
We have tested carl on the following platforms:

- Arch Linux (Kernel 3.10) with GCC 4.8.2
- Arch Linux (Kernel 3.10) with Clang 3.3
- Ubuntu 12.04 LTS with GCC 4.7.3
- Ubuntu 13.10 with GCC 4.8.1


Troubleshooting
--------------------------------------------

To be able to build and configure GTest under Mac OSX with clang and libc++ we adjust the CONFIGURE command for gtest in the file resources/CMakeLists.txt to:
~~~~~~
CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> CXX=/usr/bin/clang++ "CXXFLAGS=-stdlib=libc++ -std=c++11 -DGTEST_USE_OWN_TR1_TUPLE=1"
~~~~~~