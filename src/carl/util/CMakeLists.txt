set(carl_lib_util_headers 
	BitVector.h
	CompactTree.h
	Heap.h
) 
# Source files in here
set(carl_lib_util_sources 
	util/BitVector.cpp
	
PARENT_SCOPE)

# Installation of header files
install(FILES			${carl_lib_util_headers}
		DESTINATION		include/carl/util
)

