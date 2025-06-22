#include "Application.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <utility>
#include <GLFW/glfw3.h>
#include "GlobalSubSystemStack.hpp"
#include "math/vec.hpp"

namespace blocks {

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

Application::Application()
    : input::InputHandler(GlobalSubSystemStack::get().inputSystem()),
      renderable_(GlobalSubSystemStack::get().renderSystem().createRenderable(
          RESOURCE_DIR "/mandelbrot set.png")),
      renderable2_(GlobalSubSystemStack::get().renderSystem().createRenderable(
          RESOURCE_DIR "/test_image.bmp")),
      paddle_(),
      pos1_(
          randFloat(-1.0f, 1.0f - objSize.x()),
          randFloat(-1.0f, 1.0f - objSize.y())),
      v1_(randFloat(-1.0f, 1.0f), randFloat(-1.0f, 1.0f)),
      pos2_(
          randFloat(-1.0f, 1.0f - objSize.x()),
          randFloat(-1.0f, 1.0f - objSize.y())),
      v2_(randFloat(-1.0f, 1.0f), randFloat(-1.0f, 1.0f)) {}

void Application::run() {
  auto& subsystems = GlobalSubSystemStack::get();
  float prevFrameTimeSecs = 0;
  while (!subsystems.window()->shouldClose()) {
    std::chrono::microseconds maxFrameTime{0};
    std::chrono::microseconds totalFrameTime{0};

    for (int i = 0; i < 1000 && !subsystems.window()->shouldClose(); i++) {
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

  subsystems.renderSystem().waitIdle();
}

void Application::update(float deltaTimeSeconds) {
  pos1_ = stepPos(pos1_, v1_, deltaTimeSeconds);
  pos2_ = stepPos(pos2_, v2_, deltaTimeSeconds);

  renderable_->setPosition(pos1_, pos1_ + objSize);
  renderable2_->setPosition(pos2_, pos2_ + objSize);

  paddle_.update(deltaTimeSeconds);
}

void Application::drawFrame() {
  auto& render = GlobalSubSystemStack::get().renderSystem();
  auto window = GlobalSubSystemStack::get().window();
  render.drawObject(window, *renderable_);
  render.drawObject(window, *renderable2_);
  paddle_.draw();
  render.commitFrame();
}

void Application::onKeyPress(int key) {
  if (key == GLFW_KEY_SPACE) {
    v1_ *= -1.0f;
    v2_ *= -1.0f;
  }
}

} // namespace blocks
