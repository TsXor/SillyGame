#pragma once
#ifndef __SFLIB_NAIVE_LRU_CACHE__
#define __SFLIB_NAIVE_LRU_CACHE__

#include <string>
#include <list>
#include <unordered_map>

namespace silly_framework::rewheel {

template <typename T>
struct naive_lru {
    using value_type = std::pair<std::string, T>;
    using list_type = std::list<value_type>;
    using dict_type = std::unordered_map<std::string_view, typename list_type::iterator>;
    list_type alive_list;
    dict_type lookup_dict;

    T* get(const std::string_view& id) {
        auto dict_it = lookup_dict.find(id);
        if (dict_it == lookup_dict.end()) {
            return nullptr;
        } else {
            auto list_it = dict_it->second;
            // move found element to end
            alive_list.splice(alive_list.end(), alive_list, list_it);
            return &(list_it->second);
        }
    }

    template <typename... ArgTs>
    T& emplace(const std::string_view& id, ArgTs&&... args) {
        T* cur = get(id);
        if (cur != nullptr) { return *cur; }
        auto& store_ref = alive_list.emplace_back(std::piecewise_construct,
            std::forward_as_tuple(id),
            std::forward_as_tuple(std::forward<ArgTs>(args)...)
        );
        lookup_dict[store_ref.first] = --alive_list.end();
        return store_ref.second;
    }

    void splice(list_type& source_list) {
        for (auto it = source_list.begin(); it != source_list.end(); ++it)
            { lookup_dict[it->first] = it; }
        alive_list.splice(alive_list.end(), source_list);
    }

    T* oldest() {
        return alive_list.empty() ? nullptr : &(alive_list.front().second);
    }

    const std::string* oldest_name() {
        return alive_list.empty() ? nullptr : &(alive_list.front().first);
    }

    void pop_oldest() {
        lookup_dict.erase(alive_list.front().first);
        alive_list.pop_front();
    }
};

} // namespace silly_framework::rewheel

#endif // __SFLIB_NAIVE_LRU_CACHE__
