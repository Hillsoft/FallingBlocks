#include "engine/Timer.hpp"

#include <chrono>
#include <functional>
#include <utility>

namespace blocks {

std::chrono::microseconds Timer::getSceneUptimeMicroseconds() const {
  return currentTime_;
}

float Timer::getSceneUptimeSeconds() const {
  return static_cast<float>(getSceneUptimeMicroseconds().count()) / 1000000.f;
}

void Timer::schedule(
    std::chrono::microseconds delay, std::function<void()> callback) {
  std::chrono::microseconds triggerTime = currentTime_ + delay;
  auto it = pendingCallbacks_.begin();
  for (; it < pendingCallbacks_.end() && it->triggerTime < triggerTime; it++) {
  }
  pendingCallbacks_.insert(it, {triggerTime, std::move(callback)});
}

void Timer::tick(std::chrono::microseconds durationTime) {
  currentTime_ += durationTime;
  int i = 0;
  for (; i < pendingCallbacks_.size() &&
       pendingCallbacks_[i].triggerTime < currentTime_;
       i++) {
    // Move to a local variable to avoid any invalidations if the
    // pendingCallbacks_ array is modified by curFunc
    std::function<void()> curFunc = std::move(pendingCallbacks_[i].callback);
    curFunc();
  }
  pendingCallbacks_.erase(
      pendingCallbacks_.begin(), pendingCallbacks_.begin() + i);
}

} // namespace blocks
