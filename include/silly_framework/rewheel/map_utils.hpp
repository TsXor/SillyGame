#pragma once
#ifndef __SFLIB_MAP_UTILS__
#define __SFLIB_MAP_UTILS__

namespace silly_framework::utils {

template <typename MapT>
struct multimap_equal_range {
    MapT::iterator left;
    MapT::iterator right;
    auto begin() { return left; }
    auto end() { return right; }
    template <typename KeyT>
    multimap_equal_range(MapT& mulmap, KeyT key) {
        std::tie(left, right) = mulmap.equal_range(key);
    }
};

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
