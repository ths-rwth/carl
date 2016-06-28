ExternalProject_Add(
    EIGEN3
    URL "https://bitbucket.org/eigen/eigen/get/${EIGEN3_VERSION}.zip"
	URL_MD5 e368e3dd35e9a5f46a118029cabc494d
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
)

ExternalProject_Get_Property(EIGEN3 INSTALL_DIR)

add_imported_library(EIGEN3 SHARED "" "${INSTALL_DIR}/include")

add_dependencies(resources EIGEN3)
