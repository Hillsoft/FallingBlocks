#include "render/GLFWApplication.hpp"

#include <chrono>
#include <iostream>
#include <utility>
#include <GLFW/glfw3.h>

namespace blocks::render {

GLFWApplication::GLFWApplication()
    : render_(),
      window_(render_.createWindow()),
      window2_(render_.createWindow()),
      renderable_(render_.createRenderable(RESOURCE_DIR "/mandelbrot set.png")),
      renderable2_(render_.createRenderable(RESOURCE_DIR "/test_image.bmp")) {}

void GLFWApplication::run() {
  while (!window_->shouldClose()) {
    std::chrono::microseconds maxFrameTime{0};
    std::chrono::microseconds totalFrameTime{0};

    for (int i = 0; i < 1000 && !window_->shouldClose(); i++) {
      auto start = std::chrono::high_resolution_clock::now();
      glfwPollEvents();
      drawFrame();
      auto end = std::chrono::high_resolution_clock::now();

      std::chrono::microseconds curFrameTime =
          std::chrono::duration_cast<std::chrono::microseconds>(end - start);
      totalFrameTime += curFrameTime;
      maxFrameTime = std::max(maxFrameTime, curFrameTime);
    }

    std::cout << "FPS: " << 1000000.0f / (totalFrameTime / 1000).count()
              << "\nAverage frame time: " << totalFrameTime / 1000
              << "\nMax frame time: " << maxFrameTime << "\n";
    shouldDraw_ = !shouldDraw_;
  }

  render_.waitIdle();
}

void GLFWApplication::drawFrame() {
  if (shouldDraw_) {
    render_.drawObject(*window_, *renderable_);
    render_.drawObject(*window2_, *renderable2_);
  } else {
    render_.drawObject(*window_, *renderable2_);
    render_.drawObject(*window2_, *renderable_);
  }
  render_.commitFrame();
}

} // namespace blocks::render
