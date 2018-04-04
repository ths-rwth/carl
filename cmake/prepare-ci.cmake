# Misc stuff to setup ci tools

configure_file( ${CMAKE_SOURCE_DIR}/.ci/sonarcloud.properties.in ${CMAKE_BINARY_DIR}/sonarcloud.properties)



find_package(PythonInterp 3 REQUIRED)
if(PYTHONINTERP_FOUND)
	add_custom_target(.travis.yml)
	add_custom_command(
		TARGET .travis.yml
		BYPRODUCTS ${CMAKE_SOURCE_DIR}/.travis.yml
		COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/.ci/travis_generate.py
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/.ci/
	)
else()
	message(STATUS "Did not find python3, target .travis.yml is not available.")
endif()
