#pragma once

#include <atomic>
#include <chrono>
#include <thread>
#include <vector>
#include "audio/win/AudioClient.hpp"

namespace blocks::audio {

struct SineWave {
  unsigned int frequency = 0;
  float volume = 0.0f;
  std::chrono::milliseconds duration{};
};

class AudioSubSystem {
 public:
  AudioSubSystem();
  ~AudioSubSystem();

  AudioSubSystem(const AudioSubSystem& other) = delete;
  AudioSubSystem& operator=(const AudioSubSystem& other) = delete;

  AudioSubSystem(AudioSubSystem&& other) = delete;
  AudioSubSystem& operator=(AudioSubSystem&& other) = delete;

  void playSineWave(SineWave wave);

 private:
  void run();

  struct SineWaveCommand {
    std::atomic<bool> active = false;
    size_t startFrameCount = 0;
    size_t endFrameCount = 0;
    SineWave data;
  };

  AudioClient audioClient_;
  std::vector<SineWaveCommand> commands_;
  std::atomic<bool> finished_;
  std::atomic<size_t> frameCount_;
  std::jthread audioThread_;
};

} // namespace blocks::audio
