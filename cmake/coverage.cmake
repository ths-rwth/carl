if (COVERAGE)
	if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -g -O0")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")

		get_target_property(lcov-bin lcov IMPORTED_LOCATION)
		add_custom_target(coverage-collect
			COMMAND ${lcov-bin} --version
			COMMAND cp ${CMAKE_SOURCE_DIR}/cmake/lcov-wrapper.sh .
			COMMAND chmod +x lcov-wrapper.sh
			COMMAND ${lcov-bin} --directory . --zerocounters
			COMMAND make test
			COMMAND ${lcov-bin} --directory . --base-directory . --gcov-tool ./lcov-wrapper.sh --capture -o coverage.info
			COMMAND ${lcov-bin} --remove coverage.info '/usr/*' 'build/resources/*' 'src/tests/*' --output-file coverage.info
			COMMAND genhtml coverage.info -o "coverage/"
			COMMAND rm lcov-wrapper.sh
			WORKING_DIRECTORY ${CMAKE_BUILD_DIR}
		)
		add_dependencies(coverage-collect lcov)
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage --coverage -g -O0")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgcov")
		
		add_custom_target(coverage-collect
			COMMAND lcov --directory . --zerocounters
			COMMAND make test
			COMMAND lcov --directory . --base-directory . --capture -o coverage.info
			COMMAND lcov --remove coverage.info '/usr/*' 'build/resources/*' 'src/tests/*' --output-file coverage.info
			COMMAND genhtml coverage.info -o "coverage/"
			WORKING_DIRECTORY ${CMAKE_BUILD_DIR}
		)
	endif()

endif()
