option( COVERAGE "Enable collection of coverage statistics" OFF )

if (COVERAGE)
	if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
		#message(SEND_ERROR "Coverage collection with clang is not supported yet.")
		#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -g -O0")
		#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping")

		add_custom_target(coverage-collect
			COMMAND make
			COMMAND rm -f ${CMAKE_BINARY_DIR}/coverage/*
			COMMAND make test LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/coverage/%p.profraw
			COMMAND llvm-profdata merge -sparse ${CMAKE_BINARY_DIR}/coverage/*.profraw -o ${CMAKE_BINARY_DIR}/llvm.profdata
			COMMAND mkdir -p ${CMAKE_BINARY_DIR}/coverage-out/*
			COMMAND llvm-cov show -instr-profile llvm.profdata libcarl.so > coverage-out/coverage.txt
			WORKING_DIRECTORY ${CMAKE_BUILD_DIR}
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
