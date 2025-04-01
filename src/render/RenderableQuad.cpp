#include "render/RenderableQuad.hpp"

#include <cstdint>
#include <GLFW/glfw3.h>
#include "render/Quad.hpp"
#include "render/VulkanCommandPool.hpp"
#include "render/VulkanGraphicsDevice.hpp"

namespace blocks::render {

RenderableQuad::RenderableQuad(
    VulkanGraphicsDevice& device,
    VulkanCommandPool& commandPool,
    uint32_t maxFramesInFlight)
    : vertexShader_(getQuadVertexShader(device)),
      fragmentShader_(device, "shaders/fragment.spv"),
      descriptorSetLayout_(device),
      descriptorPool_(device, descriptorSetLayout_, maxFramesInFlight),
      pipeline_(
          device,
          VK_FORMAT_B8G8R8A8_SRGB,
          vertexShader_,
          fragmentShader_,
          descriptorSetLayout_),
      vertexAttributes_(getQuadVertexAttributesBuffer(device)),
      texture_(device, commandPool, RESOURCE_DIR "/mandelbrot set.png") {}

} // namespace blocks::render
