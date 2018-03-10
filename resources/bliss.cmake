ExternalProject_Add(
    BLISS-EP
    URL "http://www.tcs.hut.fi/Software/bliss/bliss-${BLISS_VERSION}.zip"
	DOWNLOAD_NO_PROGRESS 1
	BUILD_IN_SOURCE 1
	CONFIGURE_COMMAND ""
	INSTALL_COMMAND ""
)

ExternalProject_Get_Property(BLISS-EP SOURCE_DIR)

message(STATUS "Bliss at ${SOURCE_DIR}")

add_imported_library(BLISS SHARED "${SOURCE_DIR}/libbliss.a" "${SOURCE_DIR}")
add_imported_library(BLISS STATIC "${SOURCE_DIR}/libbliss.a" "${SOURCE_DIR}")

add_dependencies(BLISS_SHARED BLISS-EP)
add_dependencies(BLISS_STATIC BLISS-EP)
add_dependencies(resources BLISS_SHARED BLISS_STATIC)
