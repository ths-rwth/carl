string(REPLACE "." "_" DOXYGEN_TAG ${DOXYGEN_VERSION})

ExternalProject_Add(
	Doxygen-EP
	GIT_REPOSITORY "https://github.com/doxygen/doxygen.git"
	GIT_TAG Release_${DOXYGEN_TAG}
	UPDATE_COMMAND ""
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
)

ExternalProject_Get_Property(Doxygen-EP INSTALL_DIR)

add_executable(Doxygen::doxygen IMPORTED GLOBAL)
set_target_properties(Doxygen::doxygen PROPERTIES IMPORTED_LOCATION "${INSTALL_DIR}/bin/doxygen")

add_dependencies(resources Doxygen-EP)
