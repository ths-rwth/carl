set(eigen3_VERSION "3.2.8")

ExternalProject_Add(
    eigen3
    URL "https://bitbucket.org/eigen/eigen/get/${eigen3_VERSION}.zip"
	URL_MD5 e368e3dd35e9a5f46a118029cabc494d
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=.
	BUILD_COMMAND ""
)

ExternalProject_Get_Property(eigen3 binary_dir)
set(eigen3_INCLUDE_DIR ${binary_dir}/include)
message(STATUS "eigen3 include ${eigen3_INCLUDE_DIR}")

add_dependencies(resources eigen3)
