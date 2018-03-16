ExternalProject_Add(
    BLISS-EP
    URL "http://www.tcs.hut.fi/Software/bliss/bliss-${BLISS_VERSION}.zip"
	DOWNLOAD_NO_PROGRESS 1
	BUILD_IN_SOURCE 1
	PATCH_COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/resources/bliss/CMakeLists.txt <SOURCE_DIR>
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
)

ExternalProject_Get_Property(BLISS-EP INSTALL_DIR)

add_imported_library(BLISS SHARED "${INSTALL_DIR}/lib/libbliss.so" "${INSTALL_DIR}/include")
add_imported_library(BLISS STATIC "${INSTALL_DIR}/lib/libbliss.a" "${INSTALL_DIR}/include")

add_dependencies(BLISS_SHARED BLISS-EP)
add_dependencies(BLISS_STATIC BLISS-EP)
add_dependencies(resources BLISS_SHARED BLISS_STATIC)
