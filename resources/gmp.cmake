if(UNIX)
	message(STATUS "Trying to build GMP from source.")

	# GMP needs M4 to be installed
	find_program(M4 m4)
	if(NOT M4)
		message(FATAL_ERROR "Unable to build GMP from source. Could not find M4. Please install it.")
	endif ()
	mark_as_advanced(M4)

	# GMP needs makeinfo to be installed but is only necessary for the documentation
	# just disable it
	set(CONFIGURE_ENV env "MAKEINFO=true")

	ExternalProject_Add(
			GMP-EP
			URL https://ftp.gnu.org/gnu/gmp/gmp-${GMP_VERSION}.tar.bz2
			URL_HASH SHA1=db38c7b67f8eea9f2e5b8a48d219165b2fdab11f
			CONFIGURE_COMMAND ${CONFIGURE_ENV} <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> --with-pic --enable-cxx
			BUILD_COMMAND ${MAKE}
			BUILD_BYPRODUCTS ${GMP_LIBRARIES}
	)

elseif(WIN32)
	ExternalProject_Add(
		GMP-EP
		GIT_REPOSITORY "https://github.com/BrianGladman/mpir.git"
		DOWNLOAD_NO_PROGRESS 1
		CONFIGURE_COMMAND ""
		BUILD_IN_SOURCE YES
		BUILD_COMMAND cd build.vc15
		# First two calls fail, see for example https://travis-ci.org/smtrat/carl-windows/builds/451301207#L264
		# After that, it builds fine...
		COMMAND ./msbuild.bat gc DLL x64 RELEASE
		COMMAND ./msbuild.bat gc DLL x64 DEBUG
		COMMAND ./msbuild.bat gc LIB x64 RELEASE
		COMMAND ./msbuild.bat gc LIB x64 DEBUG
		COMMAND ./msbuild.bat gc DLL x64 RELEASE
		COMMAND ./msbuild.bat gc DLL x64 DEBUG
		INSTALL_COMMAND ${CMAKE_COMMAND} -E make_directory <INSTALL_DIR>/lib
		COMMAND ${CMAKE_COMMAND} -E make_directory <INSTALL_DIR>/include
		COMMAND cp <SOURCE_DIR>/dll/x64/${CMAKE_BUILD_TYPE}/mpir${DYNAMIC_EXT} <INSTALL_DIR>/lib/gmp${DYNAMIC_EXT}
		COMMAND cp <SOURCE_DIR>/dll/x64/${CMAKE_BUILD_TYPE}/mpir${DYNAMIC_EXT} <INSTALL_DIR>/lib/gmpxx${DYNAMIC_EXT}
		COMMAND cp <SOURCE_DIR>/dll/x64/${CMAKE_BUILD_TYPE}/mpir.h <INSTALL_DIR>/include/mpir.h
		COMMAND cp <SOURCE_DIR>/dll/x64/${CMAKE_BUILD_TYPE}/mpirxx.h <INSTALL_DIR>/include/gmpxx.h
		COMMAND cp <SOURCE_DIR>/lib/x64/${CMAKE_BUILD_TYPE}/mpir${STATIC_EXT} <INSTALL_DIR>/lib/gmp${STATIC_EXT}
		COMMAND cp <SOURCE_DIR>/lib/x64/${CMAKE_BUILD_TYPE}/mpirxx${STATIC_EXT} <INSTALL_DIR>/lib/gmpxx${STATIC_EXT}
	)
endif()

ExternalProject_Get_Property(GMP-EP INSTALL_DIR)

add_imported_library(GMP SHARED "${INSTALL_DIR}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}gmp${DYNAMIC_EXT}" "${INSTALL_DIR}/include")
add_imported_library(GMP STATIC "${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}gmp${STATIC_EXT}" "${INSTALL_DIR}/include")
add_imported_library(GMPXX SHARED "${INSTALL_DIR}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}gmpxx${DYNAMIC_EXT}" "${INSTALL_DIR}/include")
add_imported_library(GMPXX STATIC "${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}gmpxx${STATIC_EXT}" "${INSTALL_DIR}/include")

add_dependencies(GMP_SHARED GMP-EP)
add_dependencies(GMP_STATIC GMP-EP)
add_dependencies(GMPXX_SHARED GMP-EP)
add_dependencies(GMPXX_STATIC GMP-EP)
add_dependencies(resources GMP_SHARED GMP_STATIC GMPXX_SHARED GMPXX_STATIC)


