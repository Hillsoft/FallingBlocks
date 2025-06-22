#pragma once

namespace blocks::input {

class InputSubSystem;

class InputHandler {
 public:
  InputHandler(InputSubSystem& inputSystem);

  InputHandler(const InputHandler& other) = delete;
  InputHandler& operator=(const InputHandler& other) = delete;

  InputHandler(InputHandler&& other) = delete;
  InputHandler& operator=(InputHandler&& other) = delete;

  virtual ~InputHandler();

  virtual void onKeyPress(int keyCode) {}
  virtual void onKeyRelease(int keyCode) {}

 private:
  InputSubSystem* inputSystem_;
};

} // namespace blocks::input
