Getting Started {#getting_started}
=======

Download
--------
Here are archived versions of CArL for download:
- [latest](http://ths.informatik.rwth-aachen.de/releases/carl-latest.tgz)

We mirror our master branch to github.com. If you want to use the newest bleeding edge version, you can checkout from [https://github.com/nafur/carl].
Although we try to keep the master branch stable, there is a chance that the current revision is broken.
You can check [here](https://travis-ci.org/nafur/carl/builds) if the current revision compiles and all the unit tests work.

Quick installation guide
--------------------------------------------
- Make sure all @subpage dependencies "dependencies" are available.
- Download the latest release or clone the git repository from https://srv-i2.informatik.rwth-aachen.de:8443/git/carl.git
- Prepare the build.
@code{.sh}
$ mkdir build && cd build && cmake ../
@endcode
- Build shipped dependencies if necessary.
@code{.sh}
$ make resources
@endcode
- Build carl (with tests and documentation).
@code{.sh}
$ make
$ make test doc
@endcode

Sample installations:
- @subpage ubuntu_1404_lts
 
Using CArL
--------------------------------------------
CArL registers itself in the CMake system, hence to include CArL in any other CMake project, just use `find_package(carl)`.

To use CArL in other projects, link against the shared or static library created in `build/`.

Supported platforms
--------------------------------------------
We have tested carl successfully on the following platforms:

- @if Gereon @endif     Arch Linux (Kernel 3.17.4) with Clang 3.5.0 and GCC 4.9.2
- @if Florian @endif    Ubuntu 12.04 LTS with GCC 4.8.1
- @if Florian @endif    MacOSX 10.9 with Clang 3.3
- @if Stefan @endif		MacOSX 10.9.1 with Clang 3.3

Advanced building topics
--------------------------------------------
- @subpage build_cmake

Troubleshooting
--------------------------------------------
If you're experiencing problems, take a look at our @subpage troubleshooting section. If that doesn't help you, feel free to contact us.
