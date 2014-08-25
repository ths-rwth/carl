Ubuntu 14.04 LTS 64Bit {#ubuntu_1404_lts}
==========

This is an example how the installation on Ubuntu 14.04 LTS 64Bit looks like.
We assume that start with a fresh installation and know how to install software packages.

- Install the following packages:
 - `git` to checkout the git repository.
 - `cmake` and `cmake-curses-gui` to generate the make files.
 - `libcln6` and `libgmp-dev` for calculations with large numbers.
 - `libeigen3-dev` for numerical computations.
 - `g++` or `clang` to compile.
 - `libboost-dev` for several additional libraries.
 - `doxygen` to build the documentation. 

- Checkout CArL:
@code
git clone https://<user>@srv-i2.informatik.rwth-aachen.de:8443/git/carl.git
@endcode

- Create `build` folder:
@code 
cd carl/
mkdir build
cd build/
@endcode

- Run CMake:
@code
cmake ../
@endcode
By default, `c++` will be used as compiler which points to `g++`. This, and many other options, can be configured using `ccmake`.

- Build `libcarl`:
@code
make lib_carl
@endcode
This builds the shared library `build/libcarl.so`.

- Build and run tests:
@code
make googletest
make
make test
@endcode
The individual tests are stored in `build/bin/`.

- Build documentation:
@code
make doc
@endcode
The documentation is stored in `build/doc/`.
