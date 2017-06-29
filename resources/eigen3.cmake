ExternalProject_Add(
    EIGEN3_REPO
    URL "https://bitbucket.org/eigen/eigen/get/${EIGEN3_VERSION}.zip"
	URL_MD5 ${EIGEN3_ZIPHASH}
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
)

ExternalProject_Get_Property(EIGEN3_REPO INSTALL_DIR)

add_imported_library(EIGEN3 SHARED "" "${INSTALL_DIR}/include")

add_dependencies(resources EIGEN3_REPO)
