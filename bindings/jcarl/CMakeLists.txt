find_package(Java REQUIRED COMPONENTS Runtime Development)
find_package(JNI REQUIRED)
find_package(SWIG 3.0 REQUIRED)
include(UseJava)
include(UseSWIG)

set(CMAKE_SWIG_FLAGS -package jcarl)
set(CMAKE_SWIG_OUTDIR "jcarl")

file(GLOB_RECURSE i_files "swig/*.i")
foreach(file ${i_files})
	SET_SOURCE_FILES_PROPERTIES(${file} PROPERTIES CPLUSPLUS 1)
endforeach()


enable_testing()

#compile the tests
ADD_CUSTOM_COMMAND(
OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/test/*.class
COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/test
COMMAND ${Java_JAVAC_EXECUTABLE} -d ${CMAKE_CURRENT_BINARY_DIR}/test -cp "${CMAKE_CURRENT_SOURCE_DIR}/test:${CMAKE_CURRENT_BINARY_DIR}/jcarl.jar:/usr/share/java/junit.jar"
${CMAKE_CURRENT_SOURCE_DIR}/test/*java
DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/test/*.java
)


#build the jar
ADD_CUSTOM_COMMAND(
OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/jcarl.jar
COMMAND ${Java_JAVAC_EXECUTABLE} ./${CMAKE_SWIG_OUTDIR}/*.java
COMMAND jar cvf jcarl.jar ${CMAKE_SWIG_OUTDIR}/*.class
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)


add_test(NAME jcarl COMMAND ${Java_JAVA_EXECUTABLE} -classpath .:/usr/share/java/junit.jar:/usr/share/java/hamcrest-core.jar:../ -Djava.library.path=../ org.junit.runner.JUnitCore TestVariable
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test )




set(CMAKE_JNI_TARGET TRUE)




#this is for swig
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${JAVA_AWT_INCLUDE_PATH} -I${JAVA_AWT_INCLUDE_PATH}/linux")

swig_add_module(jcarl java ${i_files})
swig_link_libraries(jcarl lib_carl)


ADD_CUSTOM_TARGET(jcarl.jar
ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/jcarl.jar jcarl)

ADD_CUSTOM_TARGET(tests
ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/test/*.class ${CMAKE_CURRENT_BINARY_DIR}/jcarl.jar jcarl)















# include dir: bindings/jcarl/example/
# libraries: bindings/jcarl/*.jar / *.so
# javac test.java
# java -cp <jarfile>:. test

#execute test from within test directory: java -Djava.library.path=../ -cp ../:. TestBindings
