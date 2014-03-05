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
You can find more about them in [External Resources].

Installation 
--------------------------------------------
Start with:

    $ mkdir build && cd build && cmake ..

To compile dependencies (1) not found in the system AND (2) which are included in the resources:

    $ make resources

To compile the arithmetic library:

    $ make

To run some unittests:

    $ make test

To compile examples:

    $ make examples

To install:

	make install
 
Including carl
--------------------------------------------
Notice that for other CMake-projects, carl registers itself in the CMake system, 
which simplifies finding carl to a simple find_package(carl), no matter if carl is installed or not.


Supported platforms
--------------------------------------------
We have tested carl successfully on the following platforms:

- @if Sebastian @endif  Arch Linux (Kernel 3.10) with GCC 4.8.2
- @if Gereon @endif     Arch Linux (Kernel 3.12) with Clang 3.4 and GCC 4.8.2
- @if Florian @endif    Ubuntu 12.04 LTS with GCC 4.8.1
-                       Ubuntu 13.10 with GCC 4.8.1
- @if Florian @endif    MacOSX 10.9 with Clang 3.3
- @if Stefan @endif	MacOSX 10.9.1 with Clang 3.3


Troubleshooting
--------------------------------------------

If you're experiencing problems, take a look at our [troubleshooting] section. If that doesn't help you, feel free to contact us.
