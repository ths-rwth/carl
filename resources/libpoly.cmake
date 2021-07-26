ExternalProject_Add(
    LIBPOLY-EP
	  GIT_REPOSITORY https://github.com/SRI-CSL/libpoly
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release
               -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
               -DLIBPOLY_BUILD_PYTHON_API=OFF
	 BUILD_COMMAND ${CMAKE_COMMAND} <SOURCE_DIR>
   COMMAND ${CMAKE_MAKE_PROGRAM} static_poly poly
   COMMAND ${CMAKE_MAKE_PROGRAM} static_polyxx polyxx
   COMMAND ${CMAKE_MAKE_PROGRAM} install
   INSTALL_COMMAND ""
  )

ExternalProject_Get_Property(LIBPOLY-EP INSTALL_DIR)
ExternalProject_Get_Property(LIBPOLY-EP SOURCE_DIR)

add_imported_library(LIBPOLY SHARED "${INSTALL_DIR}/lib/libpoly${DYNAMIC_EXT}" "${SOURCE_DIR}/src")
add_imported_library(LIBPOLY STATIC "${INSTALL_DIR}/lib/libpoly${STATIC_EXT}" "${SOURCE_DIR}/src")
add_imported_library(LIBPOLYXX SHARED "${INSTALL_DIR}/lib/libpolyxx${DYNAMIC_EXT}" "${SOURCE_DIR}/src")
add_imported_library(LIBPOLYXX STATIC "${INSTALL_DIR}/lib/libpolyxx${STATIC_EXT}" "${SOURCE_DIR}/src")


add_dependencies(LIBPOLY-EP GMP_STATIC)

add_dependencies(LIBPOLY_SHARED LIBPOLY-EP GMP_SHARED)
add_dependencies(LIBPOLY_STATIC LIBPOLY-EP GMP_STATIC)
add_dependencies(LIBPOLYXX_SHARED LIBPOLY-EP GMPXX_SHARED)
add_dependencies(LIBPOLYXX_STATIC LIBPOLY-EP GMPXX_STATIC)

add_dependencies(resources LIBPOLY_SHARED LIBPOLY_STATIC)
add_dependencies(resources LIBPOLYXX_SHARED LIBPOLYXX_STATIC)
