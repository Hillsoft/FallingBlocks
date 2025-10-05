#pragma once

#include <atomic>
#include <thread>
#include "audio/win/AudioClient.hpp"

namespace blocks::audio {

class AudioSubSystem {
 public:
  AudioSubSystem();
  ~AudioSubSystem();

 private:
  void run();

  AudioClient audioClient_;
  std::atomic<bool> finished_;
  std::jthread audioThread_;
};

} // namespace blocks::audio
