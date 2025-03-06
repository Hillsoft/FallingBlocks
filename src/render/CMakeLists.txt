add_subdirectory(glfw_wrapper)

add_library(render.glfwapplication STATIC "GLFWApplication.cpp" "GLFWApplication.h")
target_link_libraries(render.glfwapplication
	render.glfw_wrapper.window
	render.vulkangraphicsdevice
	render.vulkaninstance)

add_library(render.validationlayers INTERFACE "validationLayers.hpp")

add_library(render.vulkangraphicsdevice STATIC "VulkanGraphicsDevice.cpp" "VulkanGraphicsDevice.h")
target_link_libraries(render.vulkangraphicsdevice
	render.vulkaninstance
	render.validationlayers
	util.debug)

add_library(render.vulkaninstance STATIC "VulkanInstance.cpp" "VulkanInstance.h")
target_link_libraries(render.vulkaninstance INTERFACE
	render.validationlayers)
