set(eigen3_VERSION "3.2.8")

ExternalProject_Add(
    eigen3
    URL "https://bitbucket.org/eigen/eigen/get/${eigen3_VERSION}.zip"
	URL_MD5 e368e3dd35e9a5f46a118029cabc494d
	SOURCE_DIR "src/eigen3/eigen3"
	INSTALL_COMMAND ""
)

ExternalProject_Get_Property(eigen3 source_dir)
set(eigen3_INCLUDE_DIR ${source_dir})

add_dependencies(resources eigen3)
