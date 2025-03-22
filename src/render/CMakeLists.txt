add_subdirectory(glfw_wrapper)
add_subdirectory(vulkan)

add_library(render.glfwapplication STATIC "GLFWApplication.cpp" "GLFWApplication.hpp")
target_link_libraries(render.glfwapplication
	render.glfw_wrapper.window
	render.vulkan.fencebuilder
	render.vulkan.semaphorebuilder
	render.vulkan.uniquehandle
	render.quad
	render.vulkanbuffer
	render.vulkancommandbuffer
	render.vulkancommandpool
	render.vulkandebugmessenger
	render.vulkandescriptorpool
	render.vulkandescriptorsetlayout
	render.vulkangraphicsdevice
	render.vulkangraphicspipeline
	render.vulkaninstance
	render.vulkanpresentstack
	render.vulkanshader
	render.vulkansurface
	render.vulkantexture)

add_library(render.quad STATIC "Quad.cpp" "Quad.hpp")
target_link_libraries(render.quad
	math.vec
	render.vulkanbuffer
	render.vulkangraphicsdevice
	render.vulkanshader)

add_library(render.validationlayers INTERFACE "validationLayers.hpp")

add_library(render.vulkanbuffer STATIC "VulkanBuffer.cpp" "VulkanBuffer.hpp")
target_link_libraries(render.vulkanbuffer
	render.vulkandevicememory
	render.vulkangraphicsdevice
	render.vulkanrawbuffer)

add_library(render.vulkancommandbuffer STATIC "VulkanCommandBuffer.cpp" "VulkanCommandBuffer.hpp")
target_link_libraries(render.vulkancommandbuffer
	render.vulkancommandpool
	render.vulkanframebuffer
	render.vulkangraphicsdevice
	render.vulkangraphicspipeline
	render.vulkanrenderpass)

add_library(render.vulkancommandpool STATIC "VulkanCommandPool.cpp" "VulkanCommandPool.hpp")
target_link_libraries(render.vulkancommandpool
	render.vulkangraphicsdevice
	render.vulkan.uniquehandle
	util.debug)

add_library(render.vulkandebugmessenger STATIC "VulkanDebugMessenger.cpp" "VulkanDebugMessenger.hpp")
target_link_libraries(render.vulkandebugmessenger
	render.vulkaninstance
	render.vulkan.uniquehandle)

add_library(render.vulkandescriptorpool STATIC "VulkanDescriptorPool.cpp" "VulkanDescriptorPool.hpp")
target_link_libraries(render.vulkandescriptorpool
	render.vulkandescriptorsetlayout
	render.vulkangraphicsdevice
	render.vulkan.uniquehandle)

add_library(render.vulkandescriptorsetlayout STATIC "VulkanDescriptorSetLayout.cpp" "VulkanDescriptorSetLayout.hpp")
target_link_libraries(render.vulkandescriptorsetlayout
	render.vulkangraphicsdevice
	render.vulkan.uniquehandle)

add_library(render.vulkandevicememory STATIC "VulkanDeviceMemory.cpp" "VulkanDeviceMemory.hpp")
target_link_libraries(render.vulkandevicememory
	render.vulkangraphicsdevice
	render.vulkanrawbuffer
	render.vulkan.uniquehandle)

add_library(render.vulkanframebuffer STATIC "VulkanFrameBuffer.cpp" "VulkanFrameBuffer.hpp")
target_link_libraries(render.vulkanframebuffer
	render.vulkangraphicsdevice
	render.vulkanimageview
	render.vulkanrenderpass
	render.vulkan.uniquehandle)

add_library(render.vulkangraphicsdevice STATIC "VulkanGraphicsDevice.cpp" "VulkanGraphicsDevice.hpp")
target_link_libraries(render.vulkangraphicsdevice
	render.validationlayers
	render.vulkaninstance
	render.vulkansurface
	render.vulkan.uniquehandle
	util.debug)

add_library(render.vulkangraphicspipeline STATIC "VulkanGraphicsPipeline.cpp" "VulkanGraphicsPipeline.hpp")
target_link_libraries(render.vulkangraphicspipeline
	render.vulkandescriptorsetlayout
	render.vulkangraphicsdevice
	render.vulkanpipelinelayout
	render.vulkanrenderpass
	render.vulkanshader
	render.vulkan.uniquehandle)

add_library(render.vulkanimageview STATIC "VulkanImageView.cpp" "VulkanImageView.hpp")
target_link_libraries(render.vulkanimageview
	render.vulkangraphicsdevice
	render.vulkan.uniquehandle)

add_library(render.vulkaninstance STATIC "VulkanInstance.cpp" "VulkanInstance.hpp")
target_link_libraries(render.vulkaninstance INTERFACE
	render.validationlayers
	render.vulkan.uniquehandle)

add_library(render.vulkanpipelinelayout STATIC "VulkanPipelineLayout.cpp" "VulkanPipelineLayout.hpp")
target_link_libraries(render.vulkanpipelinelayout
	render.vulkandescriptorsetlayout
	render.vulkangraphicsdevice
	render.vulkan.uniquehandle)

add_library(render.vulkanpresentstack STATIC "VulkanPresentStack.cpp" "VulkanPresentStack.hpp")
target_link_libraries(render.vulkanpresentstack
	render.vulkanframebuffer
	render.vulkangraphicsdevice
	render.vulkanimageview
	render.vulkanrenderpass
	render.vulkansurface
	render.vulkanswapchain
	util.debug
	util.resettable)

add_library(render.vulkanrawbuffer STATIC "VulkanRawBuffer.cpp" "VulkanRawBuffer.hpp")
target_link_libraries(render.vulkanrawbuffer
	render.vulkangraphicsdevice
	render.vulkan.uniquehandle)

add_library(render.vulkanrenderpass STATIC "VulkanRenderPass.cpp" "VulkanRenderPass.hpp")
target_link_libraries(render.vulkanrenderpass
	render.vulkangraphicsdevice
	render.vulkan.uniquehandle)

add_library(render.vulkanshader STATIC "VulkanShader.cpp" "VulkanShader.hpp")
target_link_libraries(render.vulkanshader
	render.vulkangraphicsdevice
	render.vulkan.uniquehandle
	util.file)

add_library(render.vulkansurface STATIC "VulkanSurface.cpp" "VulkanSurface.hpp")
target_link_libraries(render.vulkansurface
	render.glfw_wrapper.window
	render.vulkaninstance
	render.vulkan.uniquehandle)

add_library(render.vulkanswapchain STATIC "VulkanSwapChain.cpp" "VulkanSwapChain.hpp")
target_link_libraries(render.vulkanswapchain
	render.glfw_wrapper.window
	render.vulkangraphicsdevice
	render.vulkanimageview
	render.vulkansurface
	render.vulkan.uniquehandle)

add_library(render.vulkantexture STATIC "VulkanTexture.cpp" "VulkanTexture.hpp")
target_link_libraries(render.vulkantexture
	loader.image
	loader.image.bitmap
	render.vulkanbuffer
	render.vulkancommandpool
	render.vulkandevicememory
	render.vulkangraphicsdevice
	render.vulkanimageview
	render.vulkan.commandbufferbuilder
	render.vulkan.fencebuilder
	render.vulkan.uniquehandle)
