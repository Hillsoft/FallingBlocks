#include "engine/Actor.hpp"

#include <memory>

namespace blocks {

Actor::Actor(Scene& scene)
    : control_(std::make_shared<ControlBlock>()), scene_(&scene) {}

} // namespace blocks
