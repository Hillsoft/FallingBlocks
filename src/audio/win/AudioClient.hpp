#pragma once

#include <Audioclient.h>
#include <chrono>
#include <cstdint>
#include <utility>

namespace blocks::audio {

template <typename T>
class WinSafeRelease {
 public:
  WinSafeRelease() : ptr_(nullptr) {}
  explicit WinSafeRelease(T* ptr) : ptr_(ptr) {}

  template <typename Fn>
    requires(std::is_invocable_v<Fn, T*&>)
  explicit WinSafeRelease(Fn&& fn) : ptr_(nullptr) {
    std::forward<Fn>(fn)(ptr_);
  }

  ~WinSafeRelease() {
    if (ptr_ != nullptr) {
      ptr_->Release();
    }
  }

  WinSafeRelease(const WinSafeRelease& other) = delete;
  WinSafeRelease& operator=(const WinSafeRelease& other) = delete;

  WinSafeRelease(WinSafeRelease&& other) noexcept : ptr_(other.ptr_) {
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
class WinSafeFree {
 public:
  WinSafeFree() : ptr_(nullptr) {}
  explicit WinSafeFree(T* ptr) : ptr_(ptr) {}

  template <typename Fn>
    requires(std::is_invocable_v<Fn, T*&>)
  explicit WinSafeFree(Fn&& fn) : ptr_(nullptr) {
    std::forward<Fn>(fn)(ptr_);
  }

  ~WinSafeFree() {
    if (ptr_ != nullptr) {
      CoTaskMemFree(ptr_);
    }
  }

  WinSafeFree(const WinSafeFree& other) = delete;
  WinSafeFree& operator=(const WinSafeFree& other) = delete;

  WinSafeFree(WinSafeFree&& other) noexcept : ptr_(other.ptr_) {
    other.ptr_ = nullptr;
  }
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

  [[nodiscard]] uint32_t getNumChannels() const { return numChannels_; }
  [[nodiscard]] uint32_t getSamplesPerSecond() const {
    return samplesPerSecond_;
  }

 private:
  WinSafeRelease<IAudioClient> winClient_;
  WinSafeRelease<IAudioRenderClient> renderClient_;
  uint32_t bufferSize_ = 0;
  std::chrono::nanoseconds bufferDuration_{};
  uint32_t numChannels_ = 0;
  uint32_t samplesPerSecond_ = 0;
#ifndef NDEBUG
  bool blockInProgress_ = false;
#endif
};

} // namespace blocks::audio
