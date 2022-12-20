ExternalProject_Add(
    Eigen3-EP
	GIT_REPOSITORY "https://gitlab.com/libeigen/eigen.git"
	GIT_TAG ${EIGEN3_VERSION}
	DOWNLOAD_NO_PROGRESS 1
	UPDATE_COMMAND "" # due to https://gitlab.kitware.com/cmake/cmake/issues/17229
	#PATCH_COMMAND git cherry-pick eb592735746d935612caf658ab800e929c4ca6f4
	#COMMAND git cherry-pick f65188960c52da20392d88296cbaeb341d635834
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DPKGCONFIG_INSTALL_DIR=<INSTALL_DIR>/lib/pkgconfig
	LOG_INSTALL 1
)

ExternalProject_Get_Property(Eigen3-EP INSTALL_DIR)

add_imported_library(EIGEN3 SHARED "" "${INSTALL_DIR}/include")

add_dependencies(resources Eigen3-EP)
