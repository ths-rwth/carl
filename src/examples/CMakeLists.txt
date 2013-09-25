include_directories(${CMAKE_SOURCE_DIR}/src ${include_dirs})

add_executable( IntervalContractionExample
		IntervalContractionExample.cpp	  )
target_link_libraries(IntervalContractionExample lib_carl ${libraries} )
