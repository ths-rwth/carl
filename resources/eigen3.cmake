ExternalProject_Add(
    Eigen3-EP
    URL "https://bitbucket.org/eigen/eigen/get/${EIGEN3_VERSION}.zip"
	URL_MD5 ${EIGEN3_ZIPHASH}
	DOWNLOAD_NO_PROGRESS 1
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DCMAKE_Fortran_COMPILER=nofortran
	LOG_INSTALL 1
)

ExternalProject_Get_Property(Eigen3-EP INSTALL_DIR)

add_imported_library(EIGEN3 SHARED "" "${INSTALL_DIR}/include")

add_dependencies(resources Eigen3-EP)
