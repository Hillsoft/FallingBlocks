add_subdirectory(yaml)

add_library(serialization.serialization INTERFACE "Serialization.hpp")
target_link_libraries(serialization.serialization INTERFACE
	util.string
	util.taggedvariant)
