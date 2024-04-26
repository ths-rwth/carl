# Configure all .in files
configure_everything()

file(GLOB_RECURSE carl_sources "*.cpp")
add_new_libraries(carl-arith "${PROJECT_VERSION_LIB}" "${carl_sources}" "resources")

set(BOOST_LIBS_SHARED "Boost_system_SHARED")
set(BOOST_LIBS_STATIC "Boost_system_STATIC")
target_include_dirs_from(carl-arith-objects SYSTEM PUBLIC carl-common-shared carl-logging-shared EIGEN3 GMPXX_SHARED GMP_SHARED ${BOOST_LIBS_SHARED} ${CMAKE_DL_LIBS} pthread)
target_link_libraries(carl-arith-shared PUBLIC carl-common-shared carl-logging-shared EIGEN3 GMPXX_SHARED GMP_SHARED ${BOOST_LIBS_SHARED} ${CMAKE_DL_LIBS} ${LIB_FILESYSTEM} pthread)
target_link_libraries(carl-arith-static PUBLIC carl-common-static carl-logging-static EIGEN3 GMPXX_STATIC GMP_STATIC ${BOOST_LIBS_STATIC} ${CMAKE_DL_LIBS} ${LIB_FILESYSTEM} pthread)
if(USE_CLN_NUMBERS)
	target_include_dirs_from(carl-arith-objects SYSTEM PUBLIC CLN_SHARED)
	target_link_libraries(carl-arith-shared PUBLIC CLN_SHARED)
	target_link_libraries(carl-arith-static PUBLIC CLN_STATIC)
endif()
if(USE_COCOA)
	target_include_dirs_from(carl-arith-objects SYSTEM PUBLIC COCOA_SHARED Boost_system_SHARED)
	target_link_libraries(carl-arith-shared PUBLIC COCOA_SHARED ${BOOST_LIBS_SHARED})
	target_link_libraries(carl-arith-static PUBLIC COCOA_STATIC ${BOOST_LIBS_STATIC})
endif()
if(USE_GINAC)
	target_include_dirs_from(carl-arith-objects SYSTEM PUBLIC GINAC_SHARED)
	target_link_libraries(carl-arith-shared PUBLIC GINAC_SHARED)
	target_link_libraries(carl-arith-static PUBLIC GINAC_STATIC)
endif()
if(USE_MPFR_FLOAT)
	target_include_dirs_from(carl-arith-objects SYSTEM PUBLIC MPFR_SHARED)
	target_link_libraries(carl-arith-shared PUBLIC MPFR_SHARED)
	target_link_libraries(carl-arith-static PUBLIC MPFR_STATIC)
endif()
if(USE_LIBPOLY)
target_include_dirs_from(carl-arith-objects SYSTEM PUBLIC  LIBPOLY_SHARED LIBPOLY_STATIC) # LIBPOLYXX_SHARED LIBPOLYXX_STATIC
target_link_libraries(carl-arith-shared PUBLIC LIBPOLY_SHARED) # LIBPOLYXX_SHARED
target_link_libraries(carl-arith-static PUBLIC LIBPOLY_STATIC) # LIBPOLYXX_STATIC
endif()

include(install)

clang_tidy_recurse("${CMAKE_SOURCE_DIR}/src/carl-arith" "")

#install_libraries(carl carl-arith-shared carl-arith-static)

