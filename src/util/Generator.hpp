#pragma once

#include <coroutine>
#include <exception>
#include <optional>
#include <type_traits>
#include "util/debug.hpp"

namespace util {

template <typename T>
class Generator {
 private:
  struct Promise {
    std::optional<T> value_;

    Generator get_return_object() {
      return Generator{std::coroutine_handle<Promise>::from_promise(*this)};
    }
    std::suspend_always initial_suspend() { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }

    template <std::convertible_to<T> From>
    std::suspend_always yield_value(From&& from) {
      value_.emplace(std::forward<From>(from));
      return {};
    }

    [[noreturn]] void unhandled_exception() { throw; }

    void return_void() {}
  };

 public:
  using promise_type = Promise;
  using handle_type = std::coroutine_handle<promise_type>;

  struct Iterator {
   public:
    explicit Iterator(Generator* gen)
        : h_(gen != nullptr ? gen->h_ : nullptr), gen_(gen) {}

    Iterator& operator++() {
      h_();
      if (h_.done()) {
        gen_ = nullptr;
        h_ = nullptr;
      }
      return *this;
    }

    T& operator*() { return *h_.promise().value_; }

    bool operator==(const Iterator& other) { return gen_ == other.gen_; }
    bool operator!=(const Iterator& other) { return gen_ != other.gen_; }

   private:
    handle_type h_;
    Generator* gen_;
  };

 private:
  explicit Generator(handle_type h) : h_(h) {}

 public:
  Generator(const Generator& other) = delete;
  Generator& operator=(const Generator& other) = delete;

  Generator(Generator&& other) noexcept
      : h_(std::move(other.h_)), full_(other.full_) {
    other.h_ = nullptr;
  }
  Generator& operator=(Generator&& other) noexcept {
    std::swap(h_, other.h_);
    std::swap(full_, other.full_);
  }

  ~Generator() {
    if (h_ != nullptr) {
      h_.destroy();
    }
  }

  bool isDone() {
    fill();
    return h_.done();
  }

  explicit operator bool() { return !isDone(); }

  T operator()() {
    fill();
    full_ = false;
    return std::move(*h_.promise().value_);
  }

  Iterator begin() {
    if (!full_) {
      fill();
    }
    return Iterator{this};
  }
  Iterator end() { return Iterator{nullptr}; }

 private:
  void resume() {
    full_ = true;
    h_();
  }

  void fill() {
    if (!full_) {
      resume();
    }
  }

  T& peekValue() {
    DEBUG_ASSERT(full_);
    return *h_.promise().value_;
  }

  handle_type h_;

  bool full_ = false;
};

} // namespace util
