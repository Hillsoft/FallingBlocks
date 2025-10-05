#include "audio/AudioSubSystem.hpp"

#include <cmath>
#include <cstdint>

namespace blocks::audio {

namespace {

size_t frameCountOffset = 0;
void fillBuffer(
    uint32_t frequency,
    uint32_t frameCount,
    void* data,
    uint32_t numChannels,
    uint32_t samplesPerSecond) {
  struct Frame {
    float channelA;
    float channelB;
  };

  int samplesPerCycle = samplesPerSecond / frequency;

  float* frameData = reinterpret_cast<float*>(data);
  for (uint32_t i = 0; i < frameCount; i++) {
    float vol = 1.f +
        sinf(2 * 3.14f *
             static_cast<float>((i + frameCountOffset) % samplesPerCycle) /
             static_cast<float>(samplesPerCycle));

    for (uint32_t j = 0; j < numChannels; j++) {
      frameData[i * numChannels + j] = vol;
    }
  }

  frameCountOffset = frameCountOffset + frameCount;
}

} // namespace

AudioSubSystem::AudioSubSystem()
    : finished_(false), audioThread_([this]() { this->run(); }) {}

AudioSubSystem::~AudioSubSystem() {
  finished_.store(true, std::memory_order_relaxed);
}

void AudioSubSystem::run() {
  void* dataBuffer = nullptr;
  uint32_t samplesToWrite = 0;

  while (!finished_.load(std::memory_order_relaxed)) {
    audioClient_.beginNextBlock(dataBuffer, samplesToWrite);

    fillBuffer(
        250,
        samplesToWrite,
        dataBuffer,
        audioClient_.getNumChannels(),
        audioClient_.getSamplesPerSecond());

    audioClient_.completeBlock(samplesToWrite);
  }
}

} // namespace blocks::audio
