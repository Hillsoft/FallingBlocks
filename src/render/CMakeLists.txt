add_subdirectory(glfw_wrapper)

add_library(render.glfwapplication STATIC "GLFWApplication.cpp" "GLFWApplication.h")
target_link_libraries(render.glfwapplication
	render.glfw_wrapper.window
	render.vulkangraphicsdevice
	render.vulkaninstance)

add_library(render.vulkangraphicsdevice STATIC "VulkanGraphicsDevice.cpp" "VulkanGraphicsDevice.h")
target_link_libraries(render.vulkangraphicsdevice
	render.vulkaninstance)

add_library(render.vulkaninstance STATIC "VulkanInstance.cpp" "VulkanInstance.h")
