# Misc stuff to setup ci tools

configure_file( ${CMAKE_SOURCE_DIR}/.ci/sonarcloud.properties.in ${CMAKE_BINARY_DIR}/sonarcloud.properties)

unset(PYTHON_EXECUTABLE CACHE)
find_package(PythonInterp 3 QUIET)

if(PYTHONINTERP_FOUND)
	add_custom_target(.travis.yml)
	add_custom_command(
		TARGET .travis.yml
		BYPRODUCTS ${CMAKE_SOURCE_DIR}/.travis.yml
		COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/.ci/travis_generate.py
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/.ci/
	)
	message(STATUS "Run \"make .travis.yml\" to rebuild the travis file.")
else()
	message(STATUS "Did not find python3, target .travis.yml is not available.")
endif()
