find_package(PythonInterp)

if (PYTHONINTERP_FOUND)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/setup.cfg.in ${CMAKE_CURRENT_SOURCE_DIR}/setup.cfg)
	add_custom_target(PythonBindingsBuild
		COMMAND ${PYTHON_EXECUTABLE} setup.py build
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		DEPENDS lib_carl
	)
	add_custom_target(PythonBindingsDevelop
		COMMAND ${PYTHON_EXECUTABLE} setup.py develop
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		DEPENDS lib_carl
	)
	add_custom_target(PythonBindingsInstall
		COMMAND ${PYTHON_EXECUTABLE} setup.py install --user
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		DEPENDS lib_carl
	)
else()
	status(WARNING "Did not find a python interpreter, disabling pycarl tests.")
endif()
