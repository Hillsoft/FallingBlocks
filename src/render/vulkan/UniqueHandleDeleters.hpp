#pragma once

#include <GLFW/glfw3.h>

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
  HandleDeleter(VkDevice device) : device_(device) {}
  void destroy(VkBuffer handle) { vkDestroyBuffer(device_, handle, nullptr); }

 private:
  VkDevice device_;
};

template <>
class HandleDeleter<VkCommandPool> {
 public:
  HandleDeleter(VkDevice device) : device_(device) {}
  void destroy(VkCommandPool handle) {
    vkDestroyCommandPool(device_, handle, nullptr);
  }

 private:
  VkDevice device_;
};

template <>
class HandleDeleter<VkDebugUtilsMessengerEXT> {
 public:
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
  HandleDeleter(VkDevice device) : device_(device) {}
  void destroy(VkFence handle) { vkDestroyFence(device_, handle, nullptr); }

 private:
  VkDevice device_;
};

template <>
class HandleDeleter<VkFramebuffer> {
 public:
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
  HandleDeleter() {}
  void destroy(VkDevice handle) { vkDestroyDevice(handle, nullptr); }
};

template <>
class HandleDeleter<VkImage> {
 public:
  HandleDeleter(VkDevice device) : device_(device) {}
  void destroy(VkImage handle) { vkDestroyImage(device_, handle, nullptr); }

 private:
  VkDevice device_;
};

template <>
class HandleDeleter<VkImageView> {
 public:
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
  HandleDeleter() {}
  void destroy(VkInstance handle) { vkDestroyInstance(handle, nullptr); }
};

template <>
class HandleDeleter<VkPipeline> {
 public:
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
  HandleDeleter(VkDevice device) : device_(device) {}
  void destroy(VkSampler handle) { vkDestroySampler(device_, handle, nullptr); }

 private:
  VkDevice device_;
};

template <>
class HandleDeleter<VkSemaphore> {
 public:
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
  HandleDeleter(VkDevice device) : device_(device) {}
  void destroy(VkSwapchainKHR handle) {
    vkDestroySwapchainKHR(device_, handle, nullptr);
  }

 private:
  VkDevice device_;
};

} // namespace blocks::render::vulkan::detail
