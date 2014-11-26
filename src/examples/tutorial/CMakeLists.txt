include_directories(${CMAKE_SOURCE_DIR}/src ${include_dirs})

file(GLOB files "${CMAKE_SOURCE_DIR}/src/examples/tutorial/*.cpp")
set(targets "")
foreach(filename ${files})
	get_filename_component(filename ${filename} NAME_WE)
	add_executable(Tutorial_${filename} ${filename}.cpp)
	target_link_libraries(Tutorial_${filename} lib_carl ${libraries} )
	set(targets ${targets} Tutorial_${filename})
endforeach()

add_custom_target(tutorial DEPENDS ${targets})