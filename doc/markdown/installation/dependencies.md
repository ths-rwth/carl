Dependencies {#dependencies}
==========

To build and use CArL, you need the following other software:
- `git` to checkout the git repository.
- `cmake` to generate the make files.
- `g++` or `clang` to compile.

We use C++17 and thus need at least `g++` 7 or `clang` 5.

Optional dependencies
- `ccmake` to set cmake flags.
- `doxygen` to build the documentation. If the documentation is built without a `doxygen` installation available, `doxygen` is built requiring `flex` and `bison` packages.
- `gtest` to build the test cases.

Additionally, CArL requires a few external libraries:
- `gmp` for calculations with large numbers.
- `Eigen3` for numerical computations.
- `boost` for several additional libraries.

To simplify the installation process, all these libraries can be built by CArL automatically if it is not available on your system. You can do this manually by running
@code
make resources
@endcode
