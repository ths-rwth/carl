option( COVERAGE "Enable collection of coverage statistics" OFF )

if (COVERAGE)
	if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
		#message(SEND_ERROR "Coverage collection with clang is not supported yet.")
		#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -g -O0")
		#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping -mllvm -enable-name-compression=false")

		add_custom_target(coverage-collect
			COMMAND ${CMAKE_SOURCE_DIR}/cmake/coverage-clang.sh
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		)
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
