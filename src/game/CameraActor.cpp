#include "game/CameraActor.hpp"

#include "GlobalSubSystemStack.hpp"
#include "engine/Actor.hpp"
#include "render/Simple2DCamera.hpp"

namespace blocks::game {

CameraActor::CameraActor(Scene& scene, const CameraActorDefinition& definition)
    : CameraActor(
          scene,
          render::Simple2DCamera{
              render::Simple2DCamera::Target{
                  .centre = definition.centre, .extent = definition.extent},
              render::Simple2DCamera::FIT}) {}

CameraActor::CameraActor(Scene& scene, const render::Simple2DCamera& camera)
    : Actor(scene), camera_(camera) {}

void CameraActor::onActivate() {
  GlobalSubSystemStack::get().renderSystem().getDefaultCamera() = camera_;
}

} // namespace blocks::game
