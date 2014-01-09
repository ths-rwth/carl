if( ${TESTING} STREQUAL "ON" )
	include_directories(${GTEST_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/src ${include_dirs})

	################################
	# Unit Tests
	################################
	add_subdirectory(util)
	add_subdirectory(numbers)
	add_subdirectory(cad)
	add_subdirectory(core)
	add_subdirectory(groebner)
	add_subdirectory(interval)
endif()
