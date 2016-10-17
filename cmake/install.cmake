
if(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} VERSION_GREATER 3.0)
	install(
		DIRECTORY ${CMAKE_SOURCE_DIR}/src/carl/
		DESTINATION include/carl
		FILES_MATCHING REGEX ".*\.(h|tpp)$"
	)

	install(
		TARGETS lib_carl
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
	)

	if(BUILD_STATIC)
		install(
			TARGETS lib_carl_static
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib
		)
	endif()
else()
	message(STATUS "Disabled install target due to cmake version less than 3.1")
endif()
