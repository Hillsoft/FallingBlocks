#pragma once

#include <atomic>
#include <memory>
#include <optional>
#include <utility>
#include <vector>
#include "util/AtomicCircularBufferQueue.hpp"

namespace util {

/** Supports many writes, single reader */
template <typename T>
class IndexedResourceStorage {
 public:
  IndexedResourceStorage()
      : pendingInserts_(
            std::make_unique<
                AtomicCircularBufferQueue<std::pair<size_t, T>>>()) {}

  size_t pushBack(T&& val) {
    const size_t index = nextIndex_.fetch_add(1, std::memory_order_relaxed);
    pendingInserts_->pushBack({index, std::move(val)});
    return index;
  }
  size_t pushBack(const T& val) {
    const size_t index = nextIndex_.fetch_add(1, std::memory_order_relaxed);
    pendingInserts_->pushBack({index, val});
    return index;
  }
  size_t pushBackBlocking(T&& val) {
    const size_t index = nextIndex_.fetch_add(1, std::memory_order_relaxed);
    pendingInserts_->pushBackBlocking({index, std::move(val)});
    return index;
  }
  size_t pushBackBlocking(const T& val) {
    const size_t index = nextIndex_.fetch_add(1, std::memory_order_relaxed);
    pendingInserts_->pushBackBlocking({index, val});
    return index;
  }

  std::vector<std::optional<T>>& get() {
    for (std::optional<std::pair<size_t, T>> insertVal =
             pendingInserts_->tryPopFront();
         insertVal.has_value();
         insertVal = pendingInserts_->tryPopFront()) {
      if (vec_.size() <= insertVal->first) {
        vec_.resize(insertVal->first + 1);
      }
      vec_[insertVal->first].emplace(std::move(insertVal->second));
    }
    return vec_;
  }

 private:
  std::vector<std::optional<T>> vec_;
  std::unique_ptr<AtomicCircularBufferQueue<std::pair<size_t, T>>>
      pendingInserts_;
  std::atomic<size_t> nextIndex_ = 0;
};

} // namespace util
