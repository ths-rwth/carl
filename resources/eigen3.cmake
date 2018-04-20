ExternalProject_Add(
    Eigen3-EP
	HG_REPOSITORY "https://bitbucket.org/eigen/eigen"
	HG_TAG ${EIGEN3_VERSION}
	DOWNLOAD_NO_PROGRESS 1
	PATCH_COMMAND hg graft dbab66d00651bf050d1426334a39b627abe7216e
	COMMAND hg graft ba14974d054ae9ae4ba88e5e58012fa6c2729c32
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DPKGCONFIG_INSTALL_DIR=<INSTALL_DIR>/lib/pkgconfig
	LOG_INSTALL 1
)

ExternalProject_Get_Property(Eigen3-EP INSTALL_DIR)

add_imported_library(EIGEN3 SHARED "" "${INSTALL_DIR}/include")

add_dependencies(resources Eigen3-EP)
