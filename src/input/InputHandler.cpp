#include "input/InputHandler.hpp"

#include "input/InputSubSystem.hpp"

namespace blocks::input {

InputHandler::InputHandler(InputSubSystem& inputSystem)
    : inputSystem_(&inputSystem) {
  inputSystem.registerHandler(*this);
}

InputHandler::~InputHandler() {
  inputSystem_->unregisterHandler(*this);
}

} // namespace blocks::input
