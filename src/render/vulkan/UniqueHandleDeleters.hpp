#pragma once

#include <vulkan/vulkan_core.h>

namespace blocks::render::vulkan::detail {

void destroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator);

template <typename T>
class HandleDeleter;

template <>
class HandleDeleter<VkBuffer> {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  HandleDeleter(VkDevice device) : device_(device) {}
  void destroy(VkBuffer handle) { vkDestroyBuffer(device_, handle, nullptr); }

 private:
  VkDevice device_;
};

template <>
class HandleDeleter<VkCommandPool> {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  HandleDeleter(VkDevice device) : device_(device) {}
  void destroy(VkCommandPool handle) {
    vkDestroyCommandPool(device_, handle, nullptr);
  }

 private:
  VkDevice device_;
};

template <>
class HandleDeleter<VkCommandBuffer> {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  HandleDeleter(VkDevice device, VkCommandPool commandPool)
      : device_(device), commandPool_(commandPool) {}
  void destroy(VkCommandBuffer handle) {
    vkFreeCommandBuffers(device_, commandPool_, 1, &handle);
  }

 private:
  VkDevice device_;
  VkCommandPool commandPool_;
};

template <>
class HandleDeleter<VkDebugUtilsMessengerEXT> {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  HandleDeleter(VkInstance instance) : instance_(instance) {}
  void destroy(VkDebugUtilsMessengerEXT handle) {
    destroyDebugUtilsMessengerEXT(instance_, handle, nullptr);
  }

 private:
  VkInstance instance_;
};

template <>
class HandleDeleter<VkDescriptorPool> {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  HandleDeleter(VkDevice device) : device_(device) {}
  void destroy(VkDescriptorPool handle) {
    vkDestroyDescriptorPool(device_, handle, nullptr);
  }

 private:
  VkDevice device_;
};

template <>
class HandleDeleter<VkDescriptorSetLayout> {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  HandleDeleter(VkDevice device) : device_(device) {}
  void destroy(VkDescriptorSetLayout handle) {
    vkDestroyDescriptorSetLayout(device_, handle, nullptr);
  }

 private:
  VkDevice device_;
};

template <>
class HandleDeleter<VkDeviceMemory> {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  HandleDeleter(VkDevice device) : device_(device) {}
  void destroy(VkDeviceMemory handle) {
    vkFreeMemory(device_, handle, nullptr);
  }

 private:
  VkDevice device_;
};

template <>
class HandleDeleter<VkFence> {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  HandleDeleter(VkDevice device) : device_(device) {}
  void destroy(VkFence handle) { vkDestroyFence(device_, handle, nullptr); }

 private:
  VkDevice device_;
};

template <>
class HandleDeleter<VkFramebuffer> {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  HandleDeleter(VkDevice device) : device_(device) {}
  void destroy(VkFramebuffer handle) {
    vkDestroyFramebuffer(device_, handle, nullptr);
  }

 private:
  VkDevice device_;
};

template <>
class HandleDeleter<VkDevice> {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  HandleDeleter() = default;
  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  void destroy(VkDevice handle) { vkDestroyDevice(handle, nullptr); }
};

template <>
class HandleDeleter<VkImage> {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  HandleDeleter(VkDevice device) : device_(device) {}
  void destroy(VkImage handle) { vkDestroyImage(device_, handle, nullptr); }

 private:
  VkDevice device_;
};

template <>
class HandleDeleter<VkImageView> {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  HandleDeleter(VkDevice device) : device_(device) {}
  void destroy(VkImageView handle) {
    vkDestroyImageView(device_, handle, nullptr);
  }

 private:
  VkDevice device_;
};

template <>
class HandleDeleter<VkInstance> {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  HandleDeleter() = default;
  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  void destroy(VkInstance handle) { vkDestroyInstance(handle, nullptr); }
};

template <>
class HandleDeleter<VkPipeline> {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  HandleDeleter(VkDevice device) : device_(device) {}
  void destroy(VkPipeline handle) {
    vkDestroyPipeline(device_, handle, nullptr);
  }

 private:
  VkDevice device_;
};

template <>
class HandleDeleter<VkPipelineLayout> {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  HandleDeleter(VkDevice device) : device_(device) {}
  void destroy(VkPipelineLayout handle) {
    vkDestroyPipelineLayout(device_, handle, nullptr);
  }

 private:
  VkDevice device_;
};

template <>
class HandleDeleter<VkRenderPass> {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  HandleDeleter(VkDevice device) : device_(device) {}
  void destroy(VkRenderPass handle) {
    vkDestroyRenderPass(device_, handle, nullptr);
  }

 private:
  VkDevice device_;
};

template <>
class HandleDeleter<VkSampler> {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  HandleDeleter(VkDevice device) : device_(device) {}
  void destroy(VkSampler handle) { vkDestroySampler(device_, handle, nullptr); }

 private:
  VkDevice device_;
};

template <>
class HandleDeleter<VkSemaphore> {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  HandleDeleter(VkDevice device) : device_(device) {}
  void destroy(VkSemaphore handle) {
    vkDestroySemaphore(device_, handle, nullptr);
  }

 private:
  VkDevice device_;
};

template <>
class HandleDeleter<VkShaderModule> {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  HandleDeleter(VkDevice device) : device_(device) {}
  void destroy(VkShaderModule handle) {
    vkDestroyShaderModule(device_, handle, nullptr);
  }

 private:
  VkDevice device_;
};

template <>
class HandleDeleter<VkSurfaceKHR> {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  HandleDeleter(VkInstance instance) : instance_(instance) {}
  void destroy(VkSurfaceKHR handle) {
    vkDestroySurfaceKHR(instance_, handle, nullptr);
  }

 private:
  VkInstance instance_;
};

template <>
class HandleDeleter<VkSwapchainKHR> {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  HandleDeleter(VkDevice device) : device_(device) {}
  void destroy(VkSwapchainKHR handle) {
    vkDestroySwapchainKHR(device_, handle, nullptr);
  }

 private:
  VkDevice device_;
};

} // namespace blocks::render::vulkan::detail
