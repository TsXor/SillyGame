#pragma once
#ifndef __SWAP_QUEUE_HPP__
#define __SWAP_QUEUE_HPP__

#include <list>
#include <mutex>

template <typename T>
class swap_queue {
public:
    using list_type = std::list<T>;

private:
    std::mutex swap_lock;
    volatile bool swap_flag = false;
    list_type arr_a, arr_b;

    list_type& fg_arr() { return swap_flag ? arr_a : arr_b; }
    list_type& bg_arr() { return swap_flag ? arr_b : arr_a; }
    
    list_type& swap_and_get_bg() {
        const std::lock_guard guard(swap_lock);
        swap_flag = !swap_flag;
        return bg_arr();
    }

public:
    swap_queue() = default;
    
    // push an element to foreground array
    template <typename ValT>
    void push(ValT&& val) {
        const std::lock_guard guard(swap_lock);
        fg_arr().push_back(std::forward(val));
    }

    // emplace an element to foreground array
    template <typename... ArgTs>
    void emplace(ArgTs&&... args) {
        const std::lock_guard guard(swap_lock);
        fg_arr().emplace_back(std::forward<ArgTs>(args)...);
    }

    // swap arrays and process then clear background array
    template <typename FuncT> requires std::is_invocable_v<FuncT, list_type&>
    void reap(FuncT&& reaper) { 
        list_type& bg = swap_and_get_bg();
        reaper(bg); bg.clear();
    }
};

#endif // __SWAP_QUEUE_HPP__
