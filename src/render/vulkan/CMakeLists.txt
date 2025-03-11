add_library(render.vulkan.uniquehandle "UniqueHandle.hpp" "UniqueHandleDeleters.hpp" "UniqueHandleDeleters.cpp")
target_link_libraries(render.vulkan.uniquehandle
	util.debug)