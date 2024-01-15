find_package(Python COMPONENTS Interpreter)

if (Python_Interpreter_FOUND)
	add_test( NAME pycarl COMMAND  ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/src/tests/pycarl/runAllTests.py)
else()
	message(WARNING "Did not find a python interpreter, disabling pycarl tests.")
endif()
