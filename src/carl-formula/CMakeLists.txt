include(${CMAKE_SOURCE_DIR}/cmake/carlmacros.cmake)

add_definitions(-fPIC)

configure_everything()

file(GLOB_RECURSE carl-formula_sources "*.cpp")

add_new_libraries(carl-formula "${PROJECT_VERSION_LIB}" "${carl-formula_sources}" "carl-arith-shared")
target_include_dirs_from(carl-formula-objects "" PUBLIC carl-arith-shared)
target_link_libraries(carl-formula-shared PUBLIC carl-arith-shared)
target_link_libraries(carl-formula-static PUBLIC carl-arith-static)
if(USE_BLISS)
	target_include_dirs_from(carl-formula-objects SYSTEM PUBLIC BLISS_SHARED)
	target_link_libraries(carl-formula-shared PUBLIC BLISS_SHARED)
	target_link_libraries(carl-formula-static PUBLIC BLISS_STATIC)
endif()

install_libraries(carl carl-formula-shared carl-formula-static)
