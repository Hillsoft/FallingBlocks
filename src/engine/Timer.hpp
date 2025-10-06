#pragma once

#include <chrono>
#include <functional>
#include <vector>

namespace blocks {

class Timer {
 public:
  Timer() = default;

  std::chrono::microseconds getSceneUptimeMicroseconds() const;
  float getSceneUptimeSeconds() const;

  void schedule(
      std::chrono::microseconds delay, std::function<void()> callback);

  void tick(std::chrono::microseconds durationTime);

 private:
  struct PendingEntry {
    std::chrono::microseconds triggerTime;
    std::function<void()> callback;
  };

  std::chrono::microseconds currentTime_{0};
  std::vector<PendingEntry> pendingCallbacks_;
};

} // namespace blocks
