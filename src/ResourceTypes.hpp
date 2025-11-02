#pragma once

#include "game/Ball.hpp"
#include "util/TaggedVariant.hpp"

namespace blocks {

using GameObjects = util::TaggedVariant<
    util::TPair<util::TString<"Ball">, game::BallDefinition>>;

}
