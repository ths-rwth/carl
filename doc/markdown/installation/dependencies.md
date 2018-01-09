Dependencies {#dependencies}
==========

To build and use CArL, you need the following other software:
- `git` to checkout the git repository.
- `cmake` to generate the make files.
- `g++` or `clang` to compile.

Optional dependencies
- `ccmake` to set cmake flags.
- `doxygen` to build the documentation.
- `gtest` to build the test cases.

Additionally, CArL requires a few external libraries:
- `gmp` for calculations with large numbers.
- `Eigen3` for numerical computations.
- `boost` for several additional libraries.

To simplify the installation process, all these libraries can be built by CArL automatically if it is not available on your system. You can do this manually by running
@code
make resources
@endcode
