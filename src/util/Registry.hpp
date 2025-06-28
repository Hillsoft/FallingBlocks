#pragma once

#include <vector>
#include "util/Synchronized.hpp"
#include "util/debug.hpp"
#include "util/raii_helpers.hpp"

namespace util {

template <typename TItem, typename TActualRegistry>
class Registry : private no_copy_move {
 public:
  Registry() {}
#ifndef NDEBUG
  ~Registry() { DEBUG_ASSERT(items_.rlock()->empty()); }
#endif

  void registerItem(TItem& item) {
    auto itemsLock = items_.wlock();
    DEBUG_ASSERT(
        std::find(itemsLock->begin(), itemsLock->end(), &item) ==
        itemsLock->end());
    itemsLock->push_back(&item);
  }
  void unregisterItem(TItem& item) {
    typename decltype(items_)::value_type::iterator it;
    auto itemsLock = items_.wlock();
    while ((it = std::find(itemsLock->begin(), itemsLock->end(), &item)),
           it != itemsLock->end()) {
      itemsLock->erase(it);
    }
  }

  auto getRegisteredItemsRead() const { return items_.rlock(); }

  auto getRegisteredItemsWrite() { return items_.wlock(); }

 private:
  Synchronized<std::vector<TItem*>> items_;
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
