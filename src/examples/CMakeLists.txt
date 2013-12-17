include_directories(${CMAKE_SOURCE_DIR}/src ${include_dirs})

add_executable( IntervalContractionExample
		IntervalContractionExample.cpp	  )
add_executable( FloatExample
		FloatExample.cpp	)
target_link_libraries(IntervalContractionExample lib_carl ${libraries} )
target_link_libraries(FloatExample lib_carl ${libraries} )
