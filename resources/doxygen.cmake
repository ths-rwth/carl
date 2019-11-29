string(REPLACE "." "_" DOXYGEN_TAG ${DOXYGEN_VERSION})

ExternalProject_Add(
	Doxygen-EP
	GIT_REPOSITORY "https://github.com/doxygen/doxygen.git"
	GIT_TAG Release_${DOXYGEN_TAG}
	UPDATE_COMMAND ""
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -Duse_libclang=ON
)

ExternalProject_Get_Property(Doxygen-EP INSTALL_DIR)

# If find_package succeeded but found an old version, the target is already created.
if(NOT TARGET Doxygen::doxygen)
	add_executable(Doxygen::doxygen IMPORTED GLOBAL)
endif()
set_target_properties(Doxygen::doxygen PROPERTIES IMPORTED_LOCATION "${INSTALL_DIR}/bin/doxygen")

add_dependencies(Doxygen::doxygen Doxygen-EP)
add_dependencies(resources Doxygen-EP)
