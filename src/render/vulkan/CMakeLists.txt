add_library(render.vulkan.commandbufferbuilder "CommandBufferBuilder.hpp" "CommandBufferBuilder.cpp")
target_link_libraries(render.vulkan.commandbufferbuilder
	render.vulkan.uniquehandle)

add_library(render.vulkan.fencebuilder "FenceBuilder.hpp" "FenceBuilder.cpp")
target_link_libraries(render.vulkan.fencebuilder
	render.vulkan.uniquehandle)

add_library(render.vulkan.semaphorebuilder "SemaphoreBuilder.hpp" "SemaphoreBuilder.cpp")
target_link_libraries(render.vulkan.semaphorebuilder
	render.vulkan.uniquehandle)

add_library(render.vulkan.uniquehandle "UniqueHandle.hpp" "UniqueHandleDeleters.hpp" "UniqueHandleDeleters.cpp")
target_link_libraries(render.vulkan.uniquehandle
	util.debug)
