#pragma once

#include <Audioclient.h>
#include <chrono>
#include <cstdint>
#include <utility>
#include "util/raii_helpers.hpp"

namespace blocks::audio {

template <typename T>
class WinSafeRelease : util::no_copy_move {
 public:
  WinSafeRelease() : ptr_(nullptr) {}
  WinSafeRelease(T* ptr) : ptr_(ptr) {}

  template <typename Fn>
  WinSafeRelease(Fn&& fn) : ptr_(nullptr) {
    fn(ptr_);
  }

  ~WinSafeRelease() {
    if (ptr_ != nullptr) {
      ptr_->Release();
    }
  }

  WinSafeRelease(WinSafeRelease&& other) : ptr_(other.ptr_) {
    other.ptr_ = nullptr;
  }
  WinSafeRelease& operator=(WinSafeRelease&& other) noexcept {
    std::swap(ptr_, other.ptr_);
    return *this;
  }

  T& operator*() { return *ptr_; }
  T* operator->() { return ptr_; }
  T* get() { return ptr_; }

 private:
  T* ptr_;
};

template <typename T>
class WinSafeFree : util::no_copy {
 public:
  WinSafeFree() : ptr_(nullptr) {}
  WinSafeFree(T* ptr) : ptr_(ptr) {}

  template <typename Fn>
  WinSafeFree(Fn&& fn) : ptr_(nullptr) {
    fn(ptr_);
  }

  ~WinSafeFree() {
    if (ptr_ != nullptr) {
      CoTaskMemFree(ptr_);
    }
  }

  WinSafeFree(WinSafeFree&& other) : ptr_(other.ptr_) { other.ptr_ = nullptr; }
  WinSafeFree& operator=(WinSafeFree&& other) noexcept {
    std::swap(ptr_, other.ptr_);
    return *this;
  }

  T& operator*() { return *ptr_; }
  T* operator->() { return ptr_; }
  T* get() { return ptr_; }

 private:
  T* ptr_;
};

class AudioClient {
 public:
  AudioClient();

  void beginNextBlock(void*& buffer, uint32_t& samplesToWrite);
  void completeBlock(uint32_t samplesWritten);

  uint32_t getNumChannels() const { return numChannels_; }
  uint32_t getSamplesPerSecond() const { return samplesPerSecond_; }

 private:
  WinSafeRelease<IAudioClient> winClient_;
  WinSafeRelease<IAudioRenderClient> renderClient_;
  uint32_t bufferSize_ = 0;
  std::chrono::milliseconds bufferDuration_;
  uint32_t numChannels_ = 0;
  uint32_t samplesPerSecond_ = 0;
#ifndef NDEBUG
  bool blockInProgress_ = false;
#endif
};

} // namespace blocks::audio
