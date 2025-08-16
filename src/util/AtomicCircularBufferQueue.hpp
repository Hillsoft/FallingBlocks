#pragma once

#include <array>
#include <atomic>
#include <optional>

#include "util/raii_helpers.hpp"
#include "util/storage.hpp"

namespace util {

template <typename T, uint32_t maxSize = 256>
class AtomicCircularBufferQueue : util::no_copy_move {
 public:
  AtomicCircularBufferQueue()
      : data_(), cursors_({0, 0}), frontPending_(0), backPending_(0) {}

  ~AtomicCircularBufferQueue()
    requires(std::is_trivially_destructible_v<T>)
  = default;
  ~AtomicCircularBufferQueue()
    requires(!std::is_trivially_destructible_v<T>)
  {
    Cursors c = cursors_.load(std::memory_order_acquire);
    for (size_t i = c.front; i != c.back; i = (i + 1) % maxSize) {
      data_[i].destroy();
    }
  }

  void pushBack(T&& val) {
    pushBackFn<false>([&](uint32_t target) { data_[target].emplace(val); });
  }

  void pushBack(const T& val) {
    pushBackFn<false>([&](uint32_t target) { data_[target].emplace(val); });
  }

  void pushBackBlocking(T&& val) {
    pushBackFn<true>([&](uint32_t target) { data_[target].emplace(val); });
  }

  void pushBackBlocking(const T& val) {
    pushBackFn<true>([&](uint32_t target) { data_[target].emplace(val); });
  }

  std::optional<T> tryPopFront() {
    Cursors c;
    do {
      c = cursors_.load(std::memory_order_acquire);
      if (c.front == c.back) {
        return std::nullopt;
      }
    } while (!cursors_.compare_exchange_weak(
        c,
        c.stepFront(),
        std::memory_order_acquire,
        std::memory_order_relaxed));

    std::optional<T> result;
    try {
      result.emplace(std::move(*data_[c.front]));
    } catch (...) {
      uint32_t prevFrontPending;
      do {
        prevFrontPending = c.front;
      } while (!frontPending_.compare_exchange_weak(
          prevFrontPending,
          c.stepFront().front,
          std::memory_order_relaxed,
          std::memory_order_relaxed));
      throw;
    }

    uint32_t prevFrontPending;
    do {
      prevFrontPending = c.front;
    } while (!frontPending_.compare_exchange_weak(
        prevFrontPending,
        c.stepFront().front,
        std::memory_order_relaxed,
        std::memory_order_relaxed));
    return result;
  }

 private:
  static constexpr uint32_t stepIndex(uint32_t i) { return (i + 1) % maxSize; }
  static constexpr uint32_t unstepIndex(uint32_t i) {
    return (i + maxSize - 1) % maxSize;
  }

  struct Cursors {
    alignas(uint64_t) uint32_t front;
    uint32_t back;

    Cursors stepFront() const {
      return Cursors{.front = stepIndex(front), .back = back};
    }
    Cursors stepBack() const {
      return Cursors{.front = front, .back = stepIndex(back)};
    }
  };
  static_assert(std::atomic<Cursors>::is_always_lock_free);

  template <bool block, typename Fn>
  void pushBackFn(Fn&& fn) {
    uint32_t target = backPending_.load(std::memory_order_relaxed);
    do {
      if constexpr (block) {
        while (stepIndex(target) ==
               frontPending_.load(std::memory_order_relaxed)) {
          std::this_thread::yield();
        }
      } else {
        if (stepIndex(target) ==
            frontPending_.load(std::memory_order_relaxed)) {
          throw std::runtime_error{"AtomicCircularBufferQueue full"};
        }
      }
    } while (!backPending_.compare_exchange_weak(
        target,
        stepIndex(target),
        std::memory_order_relaxed,
        std::memory_order_relaxed));

    fn(target);

    Cursors c = cursors_.load(std::memory_order_relaxed);
    do {
      c.back = target;
    } while (!cursors_.compare_exchange_weak(
        c, c.stepBack(), std::memory_order_release, std::memory_order_relaxed));
  }

  std::array<util::StorageFor<T>, maxSize> data_;
  std::atomic<Cursors> cursors_;
  std::atomic<uint32_t> frontPending_;
  std::atomic<uint32_t> backPending_;
};

} // namespace util
