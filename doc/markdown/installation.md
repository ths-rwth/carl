Getting Started {#getting_started}
=======

Download
--------
We mirror our master branch to github.com. If you want to use the newest bleeding edge version, you can checkout from https://github.com/smtrat/carl.
Although we try to keep the master branch stable, there is a chance that the current revision is broken.
You can check [here](https://travis-ci.org/smtrat/carl/builds) if the current revision compiles and all the unit tests work.

We regularly tag reasonably stable versions. You can find them at https://github.com/smtrat/carl/releases.

Quick installation guide
--------------------------------------------
- Make sure all @subpage dependencies "dependencies" are available.
- Download the latest release or clone the git repository from https://github.com/smtrat/carl.
- Prepare the build.
@code{.sh}
$ mkdir build && cd build && cmake ../
@endcode
- Build carl (with tests and documentation).
@code{.sh}
$ make
$ make test doc
@endcode

Sample installations:
- @subpage ubuntu_1404_lts
- @subpage windows_7_msvc2014
 
Using CArL
--------------------------------------------
CArL registers itself in the CMake system, hence to include CArL in any other CMake project, just use `find_package(carl)`.

To use CArL in other projects, link against the shared or static library created in `build/`.

Supported platforms
--------------------------------------------
We have tested carl successfully on the following platforms:

- Arch Linux with Clang 3.9.1 and GCC 6.2.1
- Ubuntu 14.04 LTS with several compilers on [Travis CI](https://travis-ci.org/smtrat/carl)
- OS X 10.11 with several compilers on [Travis CI](https://travis-ci.org/smtrat/carl)

Advanced building topics
--------------------------------------------
- @subpage build_cmake

Troubleshooting
--------------------------------------------
If you're experiencing problems, take a look at our @subpage troubleshooting section. If that doesn't help you, feel free to contact us.
