#include "audio/AudioSubSystem.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace blocks::audio {

namespace {

constexpr size_t kMaxSimultaneousWaves = 256;

void blankBuffer(uint32_t frameCount, void* data, uint32_t numChannels) {
  float* frameData = reinterpret_cast<float*>(data);
  for (uint32_t i = 0; i < frameCount * numChannels; i++) {
    frameData[i] = 0.0f;
  }
}

void fillBuffer(
    SineWave waveData,
    uint32_t frameCount,
    uint32_t frameCountOffset,
    void* data,
    uint32_t numChannels,
    uint32_t samplesPerSecond) {
  size_t samplesPerCycle = samplesPerSecond / waveData.frequency;

  float* frameData = reinterpret_cast<float*>(data);
  for (uint32_t i = 0; i < frameCount; i++) {
    float vol = waveData.volume *
        sinf(2 * 3.14f *
             static_cast<float>((i + frameCountOffset) % samplesPerCycle) /
             static_cast<float>(samplesPerCycle));

    for (uint32_t j = 0; j < numChannels; j++) {
      frameData[i * numChannels + j] = vol;
    }
  }
}

} // namespace

AudioSubSystem::AudioSubSystem()
    : commands_(kMaxSimultaneousWaves),
      finished_(false),
      frameCount_(0),
      audioThread_([this]() { this->run(); }) {}

AudioSubSystem::~AudioSubSystem() {
  finished_.store(true, std::memory_order_relaxed);
}

void AudioSubSystem::playSineWave(SineWave wave) {
  for (size_t i = 0; i < commands_.size(); i++) {
    if (!commands_[i].active.load(std::memory_order_relaxed)) {
      size_t currentFrameCount = frameCount_.load(std::memory_order_relaxed);
      size_t frameDuration =
          (wave.duration.count() * audioClient_.getSamplesPerSecond() / 1000);
      size_t samplesPerCycle =
          audioClient_.getSamplesPerSecond() / wave.frequency;
      frameDuration =
          ((frameDuration + samplesPerCycle - 1) / samplesPerCycle) *
          samplesPerCycle;
      commands_[i].data = wave;
      commands_[i].startFrameCount = currentFrameCount;
      commands_[i].endFrameCount = currentFrameCount + frameDuration;
      commands_[i].active.store(true, std::memory_order_release);
      return;
    }
  }
}

void AudioSubSystem::run() {
  void* dataBuffer = nullptr;
  uint32_t samplesToWrite = 0;
  size_t frameCount = 0;

  while (!finished_.load(std::memory_order_relaxed)) {
    audioClient_.beginNextBlock(dataBuffer, samplesToWrite);

    blankBuffer(samplesToWrite, dataBuffer, audioClient_.getNumChannels());
    frameCount_.store(frameCount + samplesToWrite, std::memory_order_relaxed);

    for (size_t i = 0; i < commands_.size(); i++) {
      if (commands_[i].active.load(std::memory_order_acquire) &&
          commands_[i].startFrameCount <= frameCount_) {
        fillBuffer(
            commands_[i].data,
            static_cast<uint32_t>(std::min(
                static_cast<long long>(samplesToWrite),
                static_cast<long long>(commands_[i].endFrameCount) -
                    static_cast<long long>(frameCount))),
            frameCount - commands_[i].startFrameCount,
            dataBuffer,
            audioClient_.getNumChannels(),
            audioClient_.getSamplesPerSecond());
      }
    }

    audioClient_.completeBlock(samplesToWrite);

    frameCount += samplesToWrite;

    for (size_t i = 0; i < commands_.size(); i++) {
      if (commands_[i].active.load(std::memory_order_acquire) &&
          commands_[i].endFrameCount < frameCount_) {
        commands_[i].active.store(false, std::memory_order_relaxed);
      }
    }
  }
}

} // namespace blocks::audio
