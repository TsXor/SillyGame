#pragma once
#ifndef __SFLIB_HASH_UTILS__
#define __SFLIB_HASH_UTILS__

#include <list>
#include <unordered_map>
#include <unordered_set>

namespace silly_framework::utils {

template <typename T> requires (sizeof(T) == sizeof(void*))
struct hash_as_ptr {
    std::size_t operator()(const T& val) const noexcept {
        return std::hash<void*>{}(std::bit_cast<void*>(val));
    }
};

template <typename T>
using upair = std::pair<T, T>;

template <typename T, template<typename> typename HashT = std::hash>
struct upair_hash {
    std::size_t operator()(const upair<T>& val) const {
        return HashT<T>{}(val.first) ^ HashT<T>{}(val.second);
    }
};

template <typename T>
struct upair_equal {
    constexpr bool operator()(const upair<T>& lhs, const upair<T>& rhs) const {
        return (lhs == rhs) || (lhs.first == rhs.second && lhs.second == rhs.first);
    }
};

template <typename K, typename V>
using unordered_upair_set = std::unordered_set<upair<K>, upair_hash<K>, upair_equal<K>>;

template <typename K, typename V>
using unordered_upair_map = std::unordered_map<upair<K>, V, upair_hash<K>, upair_equal<K>>;

} // namespace silly_framework::utils

#endif // __SFLIB_HASH_UTILS__
