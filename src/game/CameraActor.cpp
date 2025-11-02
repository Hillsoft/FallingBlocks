#include "game/CameraActor.hpp"

#include <utility>
#include "GlobalSubSystemStack.hpp"
#include "engine/Actor.hpp"
#include "render/Simple2DCamera.hpp"

namespace blocks::game {

CameraActor::CameraActor(Scene& scene, const CameraActorDefinition& definition)
    : CameraActor(
          scene,
          render::Simple2DCamera{
              render::Simple2DCamera::Target{
                  definition.centre, definition.extent},
              render::Simple2DCamera::FIT}) {}

CameraActor::CameraActor(Scene& scene, render::Simple2DCamera camera)
    : Actor(scene), camera_(std::move(camera)) {
  GlobalSubSystemStack::get().renderSystem().getDefaultCamera() = camera_;
}

} // namespace blocks::game
