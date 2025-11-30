#include "input/InputHandler.hpp"

#include "input/InputSubSystem.hpp"
#include "util/Registry.hpp"

namespace blocks::input {

InputHandler::InputHandler(InputRegistry& inputSystem)
    : RegistryItem(inputSystem) {}

} // namespace blocks::input
