find_package(Java REQUIRED COMPONENTS Runtime Development)
#find_package(JNI REQUIRED)
find_package(SWIG REQUIRED)
include(UseJava)
include(UseSWIG)

set(CMAKE_SWIG_FLAGS -package example)
set(CMAKE_SWIG_OUTDIR "example")
SET_SOURCE_FILES_PROPERTIES(src/core/variable.i PROPERTIES CPLUSPLUS 1)
SET_SOURCE_FILES_PROPERTIES(src/core/variable.i PROPERTIES SWIG_FLAGS "-includeall")

set(CMAKE_JNI_TARGET TRUE)

add_jar(example_swig_jar src/core/Variable.java src/core/VariableType.java src/core/ModVariable.java src/core/ModVariableJNI.java
       	VERSION 1.2.0
	ENTRY_POINT example)

# TODO: CMAKE-Variable für src-Folder nutzen (statt /home/gereon/carl_vanilla/src)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/lib/jvm/default/include/ -I/lib/jvm/default/include/linux/ -I/home/gereon/carl_vanilla/src/")

swig_add_module(example_swig java src/core/variable.i src/core/variable_wrap.cxx)
swig_link_libraries(example_swig lib_carl)


# include dir: bindings/jcarl/example/
# libraries: bindings/jcarl/*.jar / *.so
# javac test.java
# java -cp <jarfile>:. test