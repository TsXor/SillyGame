#pragma once
#ifndef __ACTIVITY_COROUTINE_HOST__
#define __ACTIVITY_COROUTINE_HOST__

#include <tuple>
#include <any>
#include <coroutine>
#include <boost/bimap.hpp>
#include <boost/bimap/unordered_multiset_of.hpp>

namespace silly_framework {

using boost::bimap;
using boost::bimaps::unordered_multiset_of;

/**
 * 这个类的作用是放在一个activity中以运行协程。
 * 非常好协程，使我不用手写状态机。
 */
class coro_host_base {
public:
    using enum_set = std::unordered_set<size_t>;
    struct subscriber;
    template <size_t code> struct code_info {};

protected:
    bimap<unordered_multiset_of<size_t>, unordered_multiset_of<subscriber*>> pending;

    template <typename RetT, typename... ArgTs>
    inline void process_all_of(size_t code, ArgTs&&... args);

    template <typename TraitsT, typename... ArgTs>
    void process_all_of(ArgTs&&... args) {
        using return_type = TraitsT::return_type;
        process_all_of<return_type>(TraitsT::code, std::forward<ArgTs>(args)...);
    }

    void add_subscriber(subscriber* sub, enum_set& codes) {
        for (auto&& code : codes) { pending.insert({code, sub}); }
    }

public:


    inline coro_host_base();
    inline ~coro_host_base();
    inline subscriber wait_event(enum_set&& codes);

    template <typename TraitsT>
    static decltype(auto) extract_data(const std::any& data) {
        using return_type = const TraitsT::return_type&;
        return std::any_cast<return_type>(data);
    }
    template <typename TraitsT>
    static decltype(auto) extract_data(std::any& data) {
        using return_type = TraitsT::return_type&;
        return std::any_cast<return_type>(data);
    }
    template <typename TraitsT>
    static decltype(auto) extract_data(std::any&& data) {
        using return_type = TraitsT::return_type&&;
        return std::any_cast<return_type>(data);
    }
};

struct coro_host_base::subscriber {
    using argument_type = std::pair<coro_host_base*, enum_set>;
    using result_type = std::pair<size_t, std::any>;
    
    std::coroutine_handle<> caller;
    std::variant<argument_type, result_type> data;
    
    subscriber(coro_host_base* host, enum_set&& codes):
        data(std::in_place_type<argument_type>, host, std::move(codes)) {}
    
    constexpr bool await_ready() const noexcept { return false; }
    void await_suspend(std::coroutine_handle<> ch) {
        caller = ch;
        auto&& [host, codes] = std::get<argument_type>(data);
        host->add_subscriber(this, codes);
    }
    result_type await_resume() noexcept {
        return std::move(std::get<result_type>(data));
    }

    template <typename RetT, typename... ArgTs>
    void fulfill(size_t code, ArgTs&&... params) {
        if constexpr (std::is_void_v<RetT>) {
            data.template emplace<result_type>(std::piecewise_construct,
                std::forward_as_tuple(code),
                std::forward_as_tuple()
            );
        } else {
            data.template emplace<result_type>(std::piecewise_construct,
                std::forward_as_tuple(code),
                std::forward_as_tuple(std::in_place_type<RetT>, std::forward<ArgTs>(params)...)
            );
        }
        caller.resume();
    }
};

coro_host_base::coro_host_base() {}
coro_host_base::~coro_host_base() {
    for (auto&& [code, sub] : pending) { sub->caller.destroy(); }
}

template <typename RetT, typename... ArgTs>
void coro_host_base::process_all_of(size_t code, ArgTs&&... args) {
    for (auto it = pending.left.find(code); it != pending.left.end() && it->first == code;) {
        auto sub = (it++)->second; pending.right.erase(sub);
        sub->template fulfill<RetT>(code, std::forward<ArgTs>(args)...);
    }
}

coro_host_base::subscriber coro_host_base::wait_event(enum_set&& codes) {
    return {this, std::move(codes)};
}

class coro_host : public coro_host_base {
public:
    enum basic_events {
        EVT_KEY_CHANGE,
        EVT_KEY_SIGNAL,
        EVT_TICK,
        EVT_BASIC_MAX
    };

    struct evt_key_change {
        static constexpr size_t code = EVT_KEY_CHANGE;
        using return_type = void;
    };
    struct evt_key_signal {
        static constexpr size_t code = EVT_KEY_SIGNAL;
        using return_type = void;
    };
    struct evt_tick {
        static constexpr size_t code = EVT_TICK;
        using return_type = std::tuple<double, double>;
    };

    void on_key_change() { process_all_of<evt_key_change>(); }
    void on_key_signal() { process_all_of<evt_key_signal>(); }
    void on_tick(double this_time, double last_time) {
        process_all_of<evt_tick>(this_time, last_time);
    }
};


} // namespace silly_framework

#endif // __ACTIVITY_COROUTINE_HOST__
