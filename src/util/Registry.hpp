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
 public:
  Registry()
      : pendingInserts_(std::make_unique<AtomicCircularBufferQueue<TItem*>>()) {
  }
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

 private:
  Synchronized<std::vector<TItem*>> items_;
  std::unique_ptr<AtomicCircularBufferQueue<TItem*>> pendingInserts_;
};

template <typename TRegistry, typename TActualItem>
class RegistryItem : private no_copy_move {
 public:
  RegistryItem(Registry<TActualItem, TRegistry>& registry)
      : registry_(&registry) {
    registry.registerItem(*static_cast<TActualItem*>(this));
  }

  ~RegistryItem() {
    registry_->unregisterItem(*static_cast<TActualItem*>(this));
  }

 private:
  Registry<TActualItem, TRegistry>* registry_;
};

} // namespace util
