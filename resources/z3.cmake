ExternalProject_Add(
	Z3-EP
	GIT_REPOSITORY "https://github.com/Z3Prover/z3.git"
	GIT_TAG z3-${Z3_VERSION}
	UPDATE_COMMAND ""
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
)

ExternalProject_Get_Property(Z3-EP INSTALL_DIR)
ExternalProject_Get_Property(Z3-EP SOURCE_DIR)


file(REMOVE_RECURSE "${INSTALL_DIR}/include/z3")
file(COPY "${SOURCE_DIR}" DESTINATION "${INSTALL_DIR}/include")
file(RENAME "${INSTALL_DIR}/include/Z3-EP" "${INSTALL_DIR}/include/z3")

add_imported_library(Z3 SHARED "" "${INSTALL_DIR}/include")

add_dependencies(resources Z3-EP)




# ---

#add_imported_library(Z3 SHARED "${INSTALL_DIR}/libz3${DYNAMIC_EXT}" "${INSTALL_DIR}/include")
#add_imported_library(Z3 STATIC "${INSTALL_DIR}/libz3${STATIC_EXT}" "${INSTALL_DIR}/include")

#add_dependencies(Z3_SHARED Z3-EP)
#add_dependencies(Z3_STATIC Z3-EP)

#add_dependencies(resources Z3_SHARED)
#add_dependencies(resources Z3_STATIC)