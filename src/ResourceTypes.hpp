#pragma once

#include "engine/Scene.hpp"
#include "game/Ball.hpp"
#include "game/BallSpawnBlock.hpp"
#include "game/Block.hpp"
#include "game/BlocksScene.hpp"
#include "game/CameraActor.hpp"
#include "game/GameOverUI.hpp"
#include "game/MainMenuUI.hpp"
#include "game/Paddle.hpp"
#include "game/ScoreUI.hpp"
#include "game/StaticImage.hpp"
#include "util/TaggedVariant.hpp"

namespace blocks {

using GameObjects = util::TaggedVariant<
    util::TPair<util::TString<"Ball">, game::BallDefinition>,
    util::
        TPair<util::TString<"BallSpawnBlock">, game::BallSpawnBlockDefinition>,
    util::TPair<util::TString<"Block">, game::BlockDefinition>,
    util::TPair<util::TString<"CameraActor">, game::CameraActorDefinition>,
    util::TPair<util::TString<"GameOverUI">, game::GameOverUIDefinition>,
    util::TPair<util::TString<"MainMenuUI">, game::MainMenuUIDefinition>,
    util::TPair<util::TString<"Paddle">, game::PaddleDefinition>,
    util::TPair<util::TString<"ScoreUI">, game::ScoreUIDefinition>,
    util::TPair<util::TString<"StaticImage">, game::StaticImageDefinition>>;

using SceneObjects = util::TaggedVariant<
    util::TPair<util::TString<"BlocksScene">, game::BlocksSceneDefinition>,
    util::TPair<util::TString<"Scene">, SceneObjectDefinition>>;

} // namespace blocks
