#include "render/GLFWApplication.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <utility>
#include <GLFW/glfw3.h>
#include "math/vec.hpp"

namespace blocks::render {

namespace {

math::Vec2 objSize{0.2f, 0.2f};

math::Vec2 stepPos(math::Vec2 pos, math::Vec2& vel, float deltaTimeSeconds) {
  math::Vec2 newPos = pos + deltaTimeSeconds * vel;
  math::Vec2 newPos2 = newPos + objSize;

  if (newPos2.x() > 1.0f) {
    vel.x() *= -1.0f;
    newPos.x() = 1.0f - objSize.x();
  }
  if (newPos.x() < -1.0f) {
    vel.x() *= -1.0f;
    newPos.x() = -1.0f;
  }

  if (newPos2.y() > 1.0f) {
    vel.y() *= -1.0f;
    newPos.y() = 1.0f - objSize.y();
  }
  if (newPos.y() < -1.0f) {
    vel.y() *= -1.0f;
    newPos.y() = -1.0f;
  }

  return newPos;
}

float randFloat(float lo, float hi) {
  float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
  r *= (hi - lo);
  r += lo;
  return r;
}

} // namespace

GLFWApplication::GLFWApplication()
    : render_(),
      window_(render_.createWindow()),
      renderable_(render_.createRenderable(RESOURCE_DIR "/mandelbrot set.png")),
      renderable2_(render_.createRenderable(RESOURCE_DIR "/test_image.bmp")),
      pos1_(
          randFloat(-1.0f, 1.0f - objSize.x()),
          randFloat(-1.0f, 1.0f - objSize.y())),
      v1_(randFloat(-1.0f, 1.0f), randFloat(-1.0f, 1.0f)),
      pos2_(
          randFloat(-1.0f, 1.0f - objSize.x()),
          randFloat(-1.0f, 1.0f - objSize.y())),
      v2_(randFloat(-1.0f, 1.0f), randFloat(-1.0f, 1.0f)) {}

void GLFWApplication::run() {
  float prevFrameTimeSecs = 0;
  while (!window_->shouldClose()) {
    std::chrono::microseconds maxFrameTime{0};
    std::chrono::microseconds totalFrameTime{0};

    for (int i = 0; i < 1000 && !window_->shouldClose(); i++) {
      auto start = std::chrono::high_resolution_clock::now();
      glfwPollEvents();
      update(prevFrameTimeSecs);
      drawFrame();
      auto end = std::chrono::high_resolution_clock::now();

      std::chrono::microseconds curFrameTime =
          std::chrono::duration_cast<std::chrono::microseconds>(end - start);
      totalFrameTime += curFrameTime;
      maxFrameTime = std::max(maxFrameTime, curFrameTime);

      prevFrameTimeSecs =
          std::chrono::duration_cast<std::chrono::microseconds>(curFrameTime)
              .count() /
          1000000.0f;
    }

    std::cout << "FPS: " << 1000000.0f / (totalFrameTime / 1000).count()
              << "\nAverage frame time: " << totalFrameTime / 1000
              << "\nMax frame time: " << maxFrameTime << "\n";
  }

  render_.waitIdle();
}

void GLFWApplication::update(float deltaTimeSeconds) {
  pos1_ = stepPos(pos1_, v1_, deltaTimeSeconds);
  pos2_ = stepPos(pos2_, v2_, deltaTimeSeconds);

  renderable_->setPosition(pos1_, pos1_ + objSize);
  renderable2_->setPosition(pos2_, pos2_ + objSize);
}

void GLFWApplication::drawFrame() {
  render_.drawObject(*window_, *renderable_);
  render_.drawObject(*window_, *renderable2_);
  render_.commitFrame();
}

} // namespace blocks::render
