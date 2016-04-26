set(Eigen3_VERSION "3.2.8")

ExternalProject_Add(
    Eigen3
    URL "https://bitbucket.org/eigen/eigen/get/${Eigen3_VERSION}.zip"
	INSTALL_COMMAND ""
)

ExternalProject_Get_Property(Eigen3 source_dir)
set(Eigen3_INCLUDE_DIR ${source_dir}/include)

add_dependencies(resources Eigen3)
