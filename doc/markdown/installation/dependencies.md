Dependencies {#dependencies}
==========

To build and use CArL, you need the following other software:

- `git` to checkout the git repository.
- `cmake` to generate the make files.
- `cln` and `gmp` for calculations with large numbers.
- `Eigen3` for numerical computations.
- `g++` or `clang` to compile.
- `boost` for several additional libraries.

Optional dependencies
- `ccmake` to set cmake flags.
- `doxygen` to build the documentation.
- `gtest` to build the test cases.

To simplify the installation process, most libraries (currently all except `boost`) can be built by CArL automatically.
If a library is not available on your system it is built automatically, or you can build them explicitly with
@code
make resources
@endcode

When installing the dependencies, make sure that you meet the following version requirements:
- `g++` >= 5
- `clang` >= 3.6
