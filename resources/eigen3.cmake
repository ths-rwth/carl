ExternalProject_Add(
    Eigen3
    URL "https://bitbucket.org/eigen/eigen/get/${EIGEN3_VERSION}.zip"
	URL_MD5 e368e3dd35e9a5f46a118029cabc494d
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=.
	BUILD_COMMAND ""
)

ExternalProject_Get_Property(Eigen3 binary_dir)

add_imported_library(Eigen3 "" "" "${binary_dir}/include")

add_dependencies(resources Eigen3)
