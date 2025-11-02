#pragma once

#include <memory>
#include <string>
#include "engine/Scene.hpp"

namespace blocks::game::scene {

std::unique_ptr<Scene> fromResourceName(std::string sceneName);

}
