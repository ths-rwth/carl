ExternalProject_Add(
    eigen3
    URL "https://bitbucket.org/eigen/eigen/get/${EIGEN3_VERSION}.zip"
	URL_MD5 e368e3dd35e9a5f46a118029cabc494d
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=.
	BUILD_COMMAND ""
)

ExternalProject_Get_Property(eigen3 binary_dir)
set(eigen3_INCLUDE_DIR ${binary_dir}/include)

add_dependencies(resources eigen3)
