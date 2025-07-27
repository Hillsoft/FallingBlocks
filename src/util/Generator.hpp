#pragma once

#include <coroutine>
#include <exception>
#include <optional>
#include <type_traits>
#include "util/raii_helpers.hpp"

namespace util {

template <typename T>
class Generator : no_copy {
 private:
  struct Promise {
    std::optional<T> value_;
    std::exception_ptr exception_;

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

    void unhandled_exception() { exception_ = std::current_exception(); }

    void return_void() {}
  };

 public:
  using promise_type = Promise;
  using handle_type = std::coroutine_handle<promise_type>;

  struct Iterator {
   public:
    explicit Iterator(Generator* gen) : gen_(gen) {}

    Iterator& operator++() {
      gen_->resume();
      if (gen_->isDone()) {
        gen_ = nullptr;
      }
      return *this;
    }

    T& operator*() { return gen_->peekValue(); }

    bool operator==(const Iterator& other) { return gen_ == other.gen_; }
    bool operator!=(const Iterator& other) { return gen_ != other.gen_; }

   private:
    Generator* gen_;
  };

 private:
  Generator(handle_type h) : h_(h) {}

 public:
  Generator(Generator&& other) noexcept
      : h_(std::move(other.h_)), full_(other.full_) {
    other.h_ = nullptr;
  }
  Generator& operator=(Generator&& other) {
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
    if (h_.promise().exception_) {
      std::rethrow_exception(h_.promise().exception_);
    }
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
    h_();
    full_ = true;
  }

  void fill() {
    if (!full_) {
      resume();
    }
  }

  T& peekValue() {
    DEBUG_ASSERT(full_);
    if (h_.promise().exception_) {
      std::rethrow_exception(h_.promise().exception_);
    }
    return *h_.promise().value_;
  }

  handle_type h_;

  bool full_ = false;
};

} // namespace util
