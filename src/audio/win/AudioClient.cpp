#include "audio/win/AudioClient.hpp"

#include <AudioSessionTypes.h>
#include <Audioclient.h>
#include <combaseapi.h>
#include <minwindef.h>
#include <mmdeviceapi.h>
#include <mmeapi.h>
#include <mmreg.h>
#include <winerror.h>
#include <winnt.h>
#include <chrono>
#include <cstdint>
#include <stdexcept>
#include <thread>
#ifndef NDEBUG
#include "util/debug.hpp"
#endif

namespace blocks::audio {

namespace {

constexpr std::chrono::milliseconds kDesiredBufferDuration{10};

void fillBuffer(uint32_t frameCount, uint32_t channelCount, BYTE* data) {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  auto* frameData = reinterpret_cast<float*>(data);
  for (uint32_t i = 0; i < frameCount * channelCount; i++) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    frameData[i] = 0.0f;
  }
}

} // namespace

AudioClient::AudioClient() {
  WinSafeRelease<IMMDeviceEnumerator> enumerator{[](IMMDeviceEnumerator*& ptr) {
    const HRESULT result = CoCreateInstance(
        __uuidof(MMDeviceEnumerator),
        nullptr,
        CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator),
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        reinterpret_cast<void**>(&ptr));
    if (result != S_OK) {
      throw std::runtime_error{"Failed to create audio device enumerator"};
    }
  }};

  WinSafeRelease<IMMDevice> device{[&](IMMDevice*& ptr) {
    const HRESULT result = enumerator->GetDefaultAudioEndpoint(
        EDataFlow::eRender, ERole::eMultimedia, &ptr);
    if (result != S_OK) {
      throw std::runtime_error{"Failed to get default audio device"};
    }
  }};

  winClient_ = WinSafeRelease<IAudioClient>{[&](IAudioClient*& ptr) {
    const HRESULT result = device->Activate(
        __uuidof(IAudioClient),
        CLSCTX_ALL,
        nullptr,
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        reinterpret_cast<void**>(&ptr));
    if (result != S_OK) {
      throw std::runtime_error{"Failed to create audio client"};
    }
  }};

  WinSafeFree<WAVEFORMATEX> wavFormat{[&](WAVEFORMATEX*& ptr) {
    const HRESULT result = winClient_->GetMixFormat(&ptr);
    if (result != S_OK) {
      throw std::runtime_error{"Failed to create mix format"};
    }
  }};

  numChannels_ = wavFormat->nChannels;
  samplesPerSecond_ = wavFormat->nSamplesPerSec;

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  auto* wavFormatEx = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(wavFormat.get());
  if (wavFormatEx->SubFormat != KSDATAFORMAT_SUBTYPE_IEEE_FLOAT) {
    throw std::runtime_error{"Unsupported audio output format"};
  }

  const HRESULT result = winClient_->Initialize(
      AUDCLNT_SHAREMODE_SHARED,
      0,
      std::chrono::duration_cast<std::chrono::nanoseconds>(
          kDesiredBufferDuration)
              .count() /
          10,
      0,
      wavFormat.get(),
      nullptr);

  if (result != S_OK) {
    throw std::runtime_error{"Failed to initialise audio client"};
  }

  renderClient_ =
      WinSafeRelease<IAudioRenderClient>{[&](IAudioRenderClient*& ptr) {
        const HRESULT result = winClient_->GetService(
            __uuidof(IAudioRenderClient),
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
            reinterpret_cast<void**>(&ptr));
        if (result != S_OK) {
          throw std::runtime_error{"Failed to create mix format"};
        }
      }};

  winClient_->GetBufferSize(&bufferSize_);

  BYTE* data = nullptr;
  renderClient_->GetBuffer(bufferSize_, &data);

  fillBuffer(bufferSize_, wavFormat->nChannels, data);

  renderClient_->ReleaseBuffer(bufferSize_, 0);

  bufferDuration_ =
      std::chrono::nanoseconds{bufferSize_ / wavFormat->nSamplesPerSec};

  winClient_->Start();
}

void AudioClient::beginNextBlock(void*& buffer, uint32_t& samplesToWrite) {
#ifndef NDEBUG
  DEBUG_ASSERT(!blockInProgress_);
  blockInProgress_ = true;
#endif
  uint32_t padding = 0;
  winClient_->GetCurrentPadding(&padding);

  uint32_t framesAvailable = bufferSize_ - padding;
  if (framesAvailable < bufferSize_ / 4) {
    std::this_thread::sleep_for(bufferDuration_ / 4);

    winClient_->GetCurrentPadding(&padding);
    framesAvailable = bufferSize_ - padding;
  }

  BYTE* data = nullptr;
  renderClient_->GetBuffer(framesAvailable, &data);

  samplesToWrite = framesAvailable;
  buffer = data;
}

void AudioClient::completeBlock(uint32_t samplesWritten) {
#ifndef NDEBUG
  DEBUG_ASSERT(blockInProgress_);
  blockInProgress_ = false;
#endif
  renderClient_->ReleaseBuffer(samplesWritten, 0);
}

} // namespace blocks::audio
