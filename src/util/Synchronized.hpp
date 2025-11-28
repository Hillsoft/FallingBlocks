#pragma once

#include <mutex>
#include <shared_mutex>

namespace util {

namespace detail {

template <typename TMutex>
concept SharedMutex = requires(TMutex& m) { m.lock_shared(); };

template <typename TMutex>
concept UniqueMutex = !SharedMutex<TMutex> && requires(TMutex& m) { m.lock(); };

template <typename TMutex>
struct MutexDetails {};

template <typename TMutex>
  requires SharedMutex<TMutex>
struct MutexDetails<TMutex> {
  using unique_lock = std::unique_lock<TMutex>;
  using shared_lock = std::shared_lock<TMutex>;
};

template <typename TMutex>
  requires UniqueMutex<TMutex>
struct MutexDetails<TMutex> {
  using unique_lock = std::unique_lock<TMutex>;
  using shared_lock = std::unique_lock<TMutex>;
};

} // namespace detail

template <typename TVal, typename TLock>
class SynchronizedView {
 public:
  SynchronizedView(TVal& val, TLock lock)
      : val_(&val), lock_(std::move(lock)) {}

  TVal& operator*() { return *val_; }
  TVal* operator->() { return val_; }

 private:
  TVal* val_;
  TLock lock_;
};

template <typename TVal, typename TMutex = std::mutex>
  requires(detail::UniqueMutex<TMutex> || detail::SharedMutex<TMutex>)
class Synchronized {
 public:
  using value_type = TVal;
  template <typename T>
  using generic_reader_type =
      SynchronizedView<T, typename detail::MutexDetails<TMutex>::shared_lock>;
  using reader_type = generic_reader_type<const TVal>;
  template <typename T>
  using generic_writer_type =
      SynchronizedView<T, typename detail::MutexDetails<TMutex>::unique_lock>;
  using writer_type = generic_writer_type<TVal>;

  template <typename... TArgs>
  explicit Synchronized(TArgs&&... args) : val_(std::forward<TArgs>(args)...) {}

  reader_type rlock() const {
    return {val_, typename detail::MutexDetails<TMutex>::shared_lock{mutex_}};
  }

  writer_type wlock() {
    return {val_, typename detail::MutexDetails<TMutex>::unique_lock{mutex_}};
  }

 private:
  TVal val_;
  mutable TMutex mutex_;
};

} // namespace util
