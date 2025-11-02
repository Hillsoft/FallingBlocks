#pragma once

#include "game/Ball.hpp"
#include "game/Paddle.hpp"
#include "util/TaggedVariant.hpp"

namespace blocks {

using GameObjects = util::TaggedVariant<
    util::TPair<util::TString<"Ball">, game::BallDefinition>,
    util::TPair<util::TString<"Paddle">, game::PaddleDefinition>>;

}
