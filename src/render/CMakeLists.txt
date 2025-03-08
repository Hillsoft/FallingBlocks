add_subdirectory(glfw_wrapper)

add_library(render.glfwapplication STATIC "GLFWApplication.cpp" "GLFWApplication.hpp")
target_link_libraries(render.glfwapplication
	render.glfw_wrapper.window
	render.vulkancommandpool
	render.vulkandebugmessenger
	render.vulkanframebuffer
	render.vulkangraphicsdevice
	render.vulkangraphicspipeline
	render.vulkanimageview
	render.vulkaninstance
	render.vulkanshader
	render.vulkansurface
	render.vulkanswapchain)

add_library(render.validationlayers INTERFACE "validationLayers.hpp")

add_library(render.vulkancommandpool STATIC "VulkanCommandPool.cpp" "VulkanCommandPool.hpp")
target_link_libraries(render.vulkancommandpool
	render.vulkangraphicsdevice
	util.debug)

add_library(render.vulkandebugmessenger STATIC "VulkanDebugMessenger.cpp" "VulkanDebugMessenger.hpp")
target_link_libraries(render.vulkandebugmessenger
	render.vulkaninstance
	util.debug)

add_library(render.vulkanframebuffer STATIC "VulkanFrameBuffer.cpp" "VulkanFrameBuffer.hpp")
target_link_libraries(render.vulkanframebuffer
	render.vulkangraphicsdevice
	render.vulkanimageview
	render.vulkanrenderpass)

add_library(render.vulkangraphicsdevice STATIC "VulkanGraphicsDevice.cpp" "VulkanGraphicsDevice.hpp")
target_link_libraries(render.vulkangraphicsdevice
	render.vulkaninstance
	render.vulkansurface
	render.validationlayers
	util.debug)

add_library(render.vulkangraphicspipeline STATIC "VulkanGraphicsPipeline.cpp" "VulkanGraphicsPipeline.hpp")
target_link_libraries(render.vulkangraphicspipeline
	render.vulkangraphicsdevice
	render.vulkanpipelinelayout
	render.vulkanrenderpass
	render.vulkanshader)

add_library(render.vulkanimageview STATIC "VulkanImageView.cpp" "VulkanImageView.hpp")
target_link_libraries(render.vulkanimageview
	render.vulkangraphicsdevice)

add_library(render.vulkaninstance STATIC "VulkanInstance.cpp" "VulkanInstance.hpp")
target_link_libraries(render.vulkaninstance INTERFACE
	render.validationlayers)

add_library(render.vulkanpipelinelayout STATIC "VulkanPipelineLayout.cpp" "VulkanPipelineLayout.hpp")
target_link_libraries(render.vulkanpipelinelayout
	render.vulkangraphicsdevice)

add_library(render.vulkanrenderpass STATIC "VulkanRenderPass.cpp" "VulkanRenderPass.hpp")
target_link_libraries(render.vulkanrenderpass
	render.vulkangraphicsdevice)

add_library(render.vulkanshader STATIC "VulkanShader.cpp" "VulkanShader.hpp")
target_link_libraries(render.vulkanshader
	render.vulkangraphicsdevice
	util.file)

add_library(render.vulkansurface STATIC "VulkanSurface.cpp" "VulkanSurface.hpp")
target_link_libraries(render.vulkansurface
	render.glfw_wrapper.window
	render.vulkaninstance)

add_library(render.vulkanswapchain STATIC "VulkanSwapChain.cpp" "VulkanSwapChain.hpp")
target_link_libraries(render.vulkanswapchain
	render.glfw_wrapper.window
	render.vulkangraphicsdevice
	render.vulkanimageview
	render.vulkansurface
	util.debug)
