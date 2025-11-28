#pragma once

#include <memory>
#include <optional>
#include <utility>
#include <vector>
#include "util/AtomicCircularBufferQueue.hpp"
#include "util/Synchronized.hpp"
#include "util/debug.hpp"
#include "util/raii_helpers.hpp"

namespace util {

template <typename TItem, typename TActualRegistry>
class Registry : private no_copy_move {
 private:
  Registry()
      : pendingInserts_(std::make_unique<AtomicCircularBufferQueue<TItem*>>()) {
  }

 public:
#ifndef NDEBUG
  ~Registry() { DEBUG_ASSERT(items_.rlock()->empty()); }
#endif

  void registerItem(TItem& item) { pendingInserts_->pushBackBlocking(&item); }
  void unregisterItem(TItem& item) {
    typename decltype(items_)::value_type::iterator it;
    auto itemsLock = getRegisteredItems();
    while ((it = std::find(itemsLock->begin(), itemsLock->end(), &item)),
           it != itemsLock->end()) {
      itemsLock->erase(it);
    }
  }

  auto getRegisteredItems() {
    auto itemsLock = items_.wlock();
    std::optional<TItem*> nextInsert;
    while (
        (nextInsert = pendingInserts_->tryPopFront()), nextInsert.has_value()) {
      itemsLock->push_back(*nextInsert);
    }
    return itemsLock;
  }

  friend TActualRegistry;

 private:
  Synchronized<std::vector<TItem*>> items_;
  std::unique_ptr<AtomicCircularBufferQueue<TItem*>> pendingInserts_;
};

template <typename TRegistry, typename TActualItem>
class RegistryItem {
 private:
  explicit RegistryItem(Registry<TActualItem, TRegistry>& registry)
      : registry_(&registry) {
    registry.registerItem(*static_cast<TActualItem*>(this));
  }

 public:
  ~RegistryItem() {
    try {
      registry_->unregisterItem(*static_cast<TActualItem*>(this));
    } catch (...) {
      // Failing to unregister will result in UB as the item will be wrongly
      // accessed later
      std::abort();
    }
  }

  // NOLINTNEXTLINE(bugprone-crtp-constructor-accessibility)
  RegistryItem(const RegistryItem& other) = delete;
  // NOLINTNEXTLINE(bugprone-crtp-constructor-accessibility)
  RegistryItem(RegistryItem&& other) = delete;

  RegistryItem& operator=(const RegistryItem& other) = delete;
  RegistryItem& operator=(RegistryItem&& other) = delete;

  friend TActualItem;

 private:
  Registry<TActualItem, TRegistry>* registry_;
};

} // namespace util
