Getting Started {#getting_started}
=======

Download
--------
We mirror our master branch to github.com. If you want to use the newest bleeding edge version, you can checkout from https://github.com/smtrat/carl.
Although we try to keep the master branch stable, there is a chance that the current revision is broken.
You can check [here](https://travis-ci.org/nafur/carl/builds) if the current revision compiles and all the unit tests work.

You can download specific versions we tagged at https://github.com/smtrat/carl/releases.

Quick installation guide
--------------------------------------------
- Make sure all @subpage dependencies "dependencies" are available.
- Download the latest release or clone the git repository from https://github.com/smtrat/carl.
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
- @subpage windows_7_msvc2014
 
Using CArL
--------------------------------------------
CArL registers itself in the CMake system, hence to include CArL in any other CMake project, just use `find_package(carl)`.

To use CArL in other projects, link against the shared or static library created in `build/`.

Supported platforms
--------------------------------------------
We have tested carl successfully on the following platforms:

- @if Gereon @endif     Arch Linux (Kernel 4.2.1) with Clang 3.7.0 and GCC 5.2.0
- @if Florian @endif    Ubuntu 12.04 LTS with GCC 4.8.1
- @if Florian @endif    MacOSX 10.9 with Clang 3.3
- @if Stefan @endif		MacOSX 10.9.1 with Clang 3.3
@if Travis @endif
- Ubuntu 12.04 LTS with GCC 4.8.1 (Travis)
- Ubuntu 12.04 LTS with GCC 4.9.2 (Travis)
- Ubuntu 12.04 LTS with GCC 5.1.0 (Travis)
- Ubuntu 12.04 LTS with Clang 3.4.2 (Travis)
- Ubuntu 12.04 LTS with Clang 3.5.0 (Travis)
- Ubuntu 12.04 LTS with Clang 3.6.2 (Travis)
- Ubuntu 12.04 LTS with Clang 3.7.0 (Travis)

Advanced building topics
--------------------------------------------
- @subpage build_cmake

Troubleshooting
--------------------------------------------
If you're experiencing problems, take a look at our @subpage troubleshooting section. If that doesn't help you, feel free to contact us.
