add_library(math.vec INTERFACE "vec.hpp")
target_link_libraries(math.vec INTERFACE
	util.debug)
