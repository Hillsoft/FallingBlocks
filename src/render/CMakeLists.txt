add_subdirectory(glfw_wrapper)
add_subdirectory(renderables)
add_subdirectory(resource)
add_subdirectory(shaders)
add_subdirectory(vulkan)

add_library(render.font STATIC "Font.cpp" "Font.hpp")
target_link_libraries(render.font
	globalsubsystemstack
	loader.font.font
	math.vec
	render.renderables.renderablefont
	render.rendersubsystem
	render.vulkangraphicsdevice
	render.vulkanbuffer
	util.debug
	util.generator
	util.unicode)

add_library(render.forwardallocatemappedbuffer STATIC "ForwardAllocateMappedBuffer.cpp" "ForwardAllocateMappedBuffer.hpp")
target_link_libraries(render.forwardallocatemappedbuffer
	render.vulkangraphicsdevice
	render.vulkanmappedbuffer)

add_library(render.quad STATIC "Quad.cpp" "Quad.hpp")
target_link_libraries(render.quad
	render.shaders.uvvertex
	render.vulkanbuffer
	render.vulkangraphicsdevice)

add_library(render.renderableobject "RenderableObject.cpp" "RenderableObject.hpp")
target_link_libraries(render.renderableobject
	render.vulkanbuffer
	render.vulkandescriptorpool
	render.vulkanshaderprogram)

add_library(render.rendersubsystem STATIC "RenderSubSystem.cpp" "RenderSubSystem.hpp")
target_link_libraries(render.rendersubsystem
	render.forwardallocatemappedbuffer
	render.renderableobject
	render.resource.shaderprogrammanager
	render.resource.texturemanager
	render.vulkancommandbuffer
	render.vulkancommandpool
	render.vulkandebugmessenger
	render.vulkangraphicsdevice
	render.vulkaninstance
	render.vulkanpresentstack
	render.window
	render.vulkan.fencebuilder
	render.vulkan.renderpassbuilder
	render.vulkan.semaphorebuilder
	render.vulkan.uniquehandle
	util.blockforwardallocatedarena
	util.debug
	util.generator
	util.portability
	util.raii_helpers
	util.vec_generators)

add_library(render.validationlayers INTERFACE "validationLayers.hpp")

add_library(render.vulkanbuffer STATIC "VulkanBuffer.cpp" "VulkanBuffer.hpp")
target_link_libraries(render.vulkanbuffer
	render.vulkandevicememory
	render.vulkangraphicsdevice
	render.vulkanrawbuffer)

add_library(render.vulkancommandbuffer STATIC "VulkanCommandBuffer.cpp" "VulkanCommandBuffer.hpp")
target_link_libraries(render.vulkancommandbuffer
	render.vulkancommandpool
	render.vulkangraphicsdevice
	render.vulkan.commandbufferbuilder
	render.vulkan.uniquehandle)

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
	render.vulkangraphicsdevice
	render.vulkan.uniquehandle)

add_library(render.vulkandevicememory STATIC "VulkanDeviceMemory.cpp" "VulkanDeviceMemory.hpp")
target_link_libraries(render.vulkandevicememory
	render.vulkangraphicsdevice
	render.vulkanrawbuffer
	render.vulkan.uniquehandle)

add_library(render.vulkangraphicsdevice STATIC "VulkanGraphicsDevice.cpp" "VulkanGraphicsDevice.hpp")
target_link_libraries(render.vulkangraphicsdevice
	render.validationlayers
	render.vulkaninstance
	render.vulkan.uniquehandle
	util.debug)

add_library(render.vulkangraphicspipeline STATIC "VulkanGraphicsPipeline.cpp" "VulkanGraphicsPipeline.hpp")
target_link_libraries(render.vulkangraphicspipeline
	render.vulkangraphicsdevice
	render.vulkanpipelinelayout
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

add_library(render.vulkanmappedbuffer STATIC "VulkanMappedBuffer.cpp" "VulkanMappedBuffer.hpp")
target_link_libraries(render.vulkanmappedbuffer
	render.vulkandevicememory
	render.vulkangraphicsdevice
	render.vulkanrawbuffer)

add_library(render.vulkanpipelinelayout STATIC "VulkanPipelineLayout.cpp" "VulkanPipelineLayout.hpp")
target_link_libraries(render.vulkanpipelinelayout
	render.vulkangraphicsdevice
	render.vulkan.uniquehandle)

add_library(render.vulkanpresentstack STATIC "VulkanPresentStack.cpp" "VulkanPresentStack.hpp")
target_link_libraries(render.vulkanpresentstack
	render.glfw_wrapper.window
	render.vulkangraphicsdevice
	render.vulkanimageview
	render.vulkanswapchain
	render.vulkan.framebufferbuilder
	render.vulkan.surfacebuilder
	render.vulkan.uniquehandle
	util.debug
	util.resettable)

add_library(render.vulkanrawbuffer STATIC "VulkanRawBuffer.cpp" "VulkanRawBuffer.hpp")
target_link_libraries(render.vulkanrawbuffer
	render.vulkangraphicsdevice
	render.vulkan.uniquehandle)

add_library(render.vulkanshader STATIC "VulkanShader.cpp" "VulkanShader.hpp")
target_link_libraries(render.vulkanshader
	render.vulkangraphicsdevice
	render.vulkan.uniquehandle
	util.file)

add_library(render.vulkanshaderprogram STATIC "VulkanShaderProgram.cpp" "VulkanShaderProgram.hpp")
target_link_libraries(render.vulkanshaderprogram
	render.vulkangraphicsdevice
	render.vulkangraphicspipeline
	render.vulkanshader
	render.vulkan.uniquehandle)

add_library(render.vulkanswapchain STATIC "VulkanSwapChain.cpp" "VulkanSwapChain.hpp")
target_link_libraries(render.vulkanswapchain
	render.vulkangraphicsdevice
	render.vulkanimageview
	render.vulkan.uniquehandle
	util.debug)

add_library(render.vulkantexture STATIC "VulkanTexture.cpp" "VulkanTexture.hpp")
target_link_libraries(render.vulkantexture
	loader.image
	loader.loadimage
	render.vulkanbuffer
	render.vulkancommandpool
	render.vulkandevicememory
	render.vulkangraphicsdevice
	render.vulkanimageview
	render.vulkan.commandbufferbuilder
	render.vulkan.fencebuilder
	render.vulkan.uniquehandle)

add_library(render.window STATIC "Window.cpp" "Window.hpp")
target_link_libraries(render.window
	render.vulkangraphicsdevice
	render.vulkaninstance
	render.vulkanpresentstack
	render.glfw_wrapper.window)
