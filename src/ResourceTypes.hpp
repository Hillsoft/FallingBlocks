#pragma once

#include "game/Ball.hpp"
#include "game/BallSpawnBlock.hpp"
#include "game/Block.hpp"
#include "game/CameraActor.hpp"
#include "game/Paddle.hpp"
#include "game/StaticImage.hpp"
#include "util/TaggedVariant.hpp"

namespace blocks {

using GameObjects = util::TaggedVariant<
    util::TPair<util::TString<"Ball">, game::BallDefinition>,
    util::
        TPair<util::TString<"BallSpawnBlock">, game::BallSpawnBlockDefinition>,
    util::TPair<util::TString<"Block">, game::BlockDefinition>,
    util::TPair<util::TString<"CameraActor">, game::CameraActorDefinition>,
    util::TPair<util::TString<"Paddle">, game::PaddleDefinition>,
    util::TPair<util::TString<"StaticImage">, game::StaticImageDefinition>>;

}
