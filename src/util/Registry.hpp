#pragma once

#include <span>
#include <vector>
#include "util/debug.hpp"
#include "util/raii_helpers.hpp"

namespace util {

template <typename TItem, typename TActualRegistry>
class Registry : private no_copy_move {
 public:
  Registry() {}
#ifndef NDEBUG
  ~Registry() { DEBUG_ASSERT(items_.empty()); }
#endif

  void registerItem(TItem& item) {
    DEBUG_ASSERT(
        std::find(items_.begin(), items_.end(), &item) == items_.end());
    items_.push_back(&item);
  }
  void unregisterItem(TItem& item) {
    typename decltype(items_)::iterator it;
    while ((it = std::find(items_.begin(), items_.end(), &item)),
           it != items_.end()) {
      items_.erase(it);
    }
  }

  std::span<TItem* const> getRegisteredItems() { return items_; }

 private:
  std::vector<TItem*> items_;
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
