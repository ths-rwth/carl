Getting Started {#getting_started}
=======

Download
--------
We mirror our master branch to github.com. If you want to use the newest bleeding edge version, you can checkout from https://github.com/ths-rwth/carl.
Although we try to keep the master branch stable, there is a chance that the current revision is broken.
You can check [here](https://travis-ci.org/smtrat/carl/builds) if the current revision compiles and all the unit tests work.

We regularly tag reasonably stable versions. You can find them at https://github.com/ths-rwth/carl/releases.

Quick installation guide
--------------------------------------------
- Make sure all @subpage dependencies "dependencies" are available.
- Download the latest release or clone the git repository from https://github.com/ths-rwth/carl.
- Prepare the build.
@code{.sh}
$ mkdir build && cd build && cmake ../
@endcode
- Build carl (with tests and documentation).
@code{.sh}
$ make
$ make test doc
@endcode
 
Using CArL
--------------------------------------------
CArL registers itself in the CMake system, hence to include CArL in any other CMake project, just use `find_package(carl)`.

To use CArL in other projects, link against the shared or static library created in `build/`.

Supported platforms
--------------------------------------------
We test carl on the following platforms:

- Ubuntu 22.04 LTS with several compilers

We usually support at least all `clang` and `gcc` versions starting from those shipped with the latest Ubuntu LTS or Debian stable releases.
As of now, this is `clang-13` and newer and `gcc-11` and newer.

Advanced building topics
--------------------------------------------
- @subpage build_cmake

Troubleshooting
--------------------------------------------
If you're experiencing problems, take a look at our @subpage troubleshooting section. If that doesn't help you, feel free to contact us.
