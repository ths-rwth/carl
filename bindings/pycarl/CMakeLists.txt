find_package(PythonInterp)

if (PYTHONINTERP_FOUND)

	# Collect include directories
	set(PYCARL_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src)
	set(PYCARL_DEFINE_CLN "#define=PYCARL_USE_CLN")
	set(PYCARL_LIBRARIES "gmp,gmpxx")
	if(USE_CLN_NUMBERS)
		get_include_dir(PYCARL_INCLUDE_DIRS CLN_SHARED)
		get_include_dir(PYCARL_INCLUDE_DIRS CLN_STATIC)
		get_include_dir(PYCARL_INCLUDE_DIRS GMP_STATIC)
		set(PYCARL_DEFINE_CLN "define=PYCARL_USE_CLN")
		set(PYCARL_LIBRARIES "cln")
	endif()
	if(USE_GINAC)
		get_include_dir(PYCARL_INCLUDE_DIRS GINAC_SHARED)
		get_include_dir(PYCARL_INCLUDE_DIRS GINAC_STATIC)
		get_include_dir(PYCARL_INCLUDE_DIRS CLN_STATIC)
	endif()
	get_include_dir(PYCARL_INCLUDE_DIRS GMP_SHARED)
	get_include_dir(PYCARL_INCLUDE_DIRS GMP_STATIC)
	get_include_dir(PYCARL_INCLUDE_DIRS GMPXX_STATIC)
	get_include_dir(PYCARL_INCLUDE_DIRS EIGEN3)
	#get_include_dir(PYCARL_INCLUDE_DIRS GTESTCORE_STATIC)
	#get_include_dir(PYCARL_INCLUDE_DIRS GTESTMAIN_STATIC)

	# Set configuration file
	set(PYCARL_LIBRARY_DIRS "${CMAKE_BINARY_DIR}")
	set(PYCARL_RPATH "${CMAKE_BINARY_DIR}")

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
