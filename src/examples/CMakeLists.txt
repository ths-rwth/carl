include_directories(${CMAKE_SOURCE_DIR}/src ${include_dirs})

# Write config.h 
configure_file( ${CMAKE_SOURCE_DIR}/src/examples/config.h.in 
                ${CMAKE_SOURCE_DIR}/src/examples/config.h
)  

add_executable( IntervalContractionExample
		IntervalContractionExample.cpp	  )
add_executable( FloatExample
		FloatExample.cpp	)
add_executable( GroebnerExample 
		GroebnerExample.cpp )
add_executable( IntervalCoefficientExample
		IntervalCoefficientExample.cpp		)
add_executable( IntervalExample
		IntervalExample.cpp		)
add_executable( MpqIntervalExample
		MpqIntervalExample.cpp		)
add_executable( example_smt2
		smt2example.cpp
)



target_link_libraries(IntervalContractionExample lib_carl ${libraries} )
target_link_libraries(FloatExample lib_carl ${libraries} )
target_link_libraries(GroebnerExample lib_carl ${libraries} )
target_link_libraries(IntervalExample lib_carl ${libraries} )
target_link_libraries(IntervalCoefficientExample lib_carl ${libraries} )
target_link_libraries(MpqIntervalExample lib_carl ${libraries} )
target_link_libraries(example_smt2 lib_carl ${libraries} )

add_subdirectory(tutorial EXCLUDE_FROM_ALL)