#pragma once
#ifndef __SFLIB_MAP_UTILS__
#define __SFLIB_MAP_UTILS__

#include <utility>

namespace silly_framework::utils {

template <typename MapT, typename KeyT, typename ValueT>
MapT::mapped_type& value_or(MapT& self, KeyT&& key, ValueT& defval) {
    if (auto it = self.find(std::forward<KeyT>(key)); it != self.end()) {
        return it->second;
    } else {
        return defval;
    }
}

template <typename MapT, typename KeyT, typename ValueT>
const MapT::mapped_type& value_or(const MapT& self, KeyT&& key, const ValueT& defval) {
    if (auto it = self.find(std::forward<KeyT>(key)); it != self.end()) {
        return it->second;
    } else {
        return defval;
    }
}

} // namespace silly_framework::utils

#endif // __SFLIB_MAP_UTILS__
