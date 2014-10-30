set(carl_lib_util_headers 
	BitVector.h
	CompactTree.h
	debug.h
	Heap.h
	SFINAE.h
	tree.h
	Cache.h
        TermAdditionManager.h
) 
# Source files in here
set(carl_lib_util_sources 
	util/BitVector.cpp
	util/debug.cpp
	
PARENT_SCOPE)

# Installation of header files
install(FILES			${carl_lib_util_headers}
		DESTINATION		include/carl/util
)

