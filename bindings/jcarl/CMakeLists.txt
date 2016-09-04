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


#compile the test
ADD_CUSTOM_COMMAND(
OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/test/*.class
COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/test
COMMAND ${Java_JAVAC_EXECUTABLE} -d ${CMAKE_CURRENT_BINARY_DIR}/test -cp "${CMAKE_CURRENT_SOURCE_DIR}/test:${CMAKE_CURRENT_BINARY_DIR}/jcarl.jar"
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





set(CMAKE_JNI_TARGET TRUE)




#this is for swig
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/lib/jvm/java-7-openjdk-amd64/include -I/usr/lib/jvm/java-7-openjdk-amd64/include")

swig_add_module(jcarl java ${i_files})
swig_link_libraries(jcarl lib_carl)


ADD_CUSTOM_TARGET(jcarl.jar
ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/jcarl.jar jcarl)

ADD_CUSTOM_TARGET(tests
ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/test/*.class ${CMAKE_CURRENT_BINARY_DIR}/jcarl.jar jcarl)

#DD_DEPENDENCIES(tests jcarl.jar)











# include dir: bindings/jcarl/example/
# libraries: bindings/jcarl/*.jar / *.so
# javac test.java
# java -cp <jarfile>:. test

#execute test from within test directory: java -Djava.library.path=../ -cp ../:. TestBindings
