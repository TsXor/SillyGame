#pragma once
#ifndef __ACTIVITY_COROUTINE_HOST__
#define __ACTIVITY_COROUTINE_HOST__

#include <tuple>
#include <variant>
#include <list>
#include <coroutine>
#include <coutils.hpp>
#include "silly_framework/facilities/iface_activity.hpp"

namespace silly_framework {

template <typename T>
struct method_traits {};
template <typename ClsT, typename RetT, typename... ArgTs>
struct method_traits<RetT(ClsT::*)(ArgTs...)> {
    using class_type = ClsT;
    using return_type = RetT;
    using param_tuple_type = std::tuple<ArgTs...>;
};


/**
 * 这个类的作用是放在一个activity中以运行协程。
 * 非常好协程，使我不用手写状态机。
 */
class coro_host {
    template <typename Derived, typename RetT>
    // CRTP，很神奇吧
    struct base_event {
        std::coroutine_handle<> caller;
        std::variant<coro_host*, RetT> data;
        base_event(coro_host* host) : data(std::in_place_type<coro_host*>, host) {}
        
        constexpr bool await_ready() const noexcept { return false; }
        void await_suspend(std::coroutine_handle<> ch) {
            caller = ch;
            auto& host = std::get<coro_host*>(data);
            static_cast<Derived*>(this)->register_pending(*host);
        }
        RetT await_resume() noexcept {
            return std::move(std::get<RetT>(data));
        }

        template <typename... Ts>
        void set_return(Ts... params) {
            data.template emplace<RetT>(params...);
        }
    };
    
    using keys_ret_type = method_traits<decltype(iface_activity::on_key_event)>::param_tuple_type;
    using tick_ret_type = method_traits<decltype(iface_activity::tick)>::param_tuple_type;

public:
    struct keys_event : public base_event<keys_event, keys_ret_type> {
        void register_pending(coro_host& host) { host.keys_pending.emplace_back(this); }
    };
    struct tick_event : public base_event<tick_event, tick_ret_type>  {
        void register_pending(coro_host& host) { host.tick_pending.emplace_back(this); }
    };

protected:
    std::list<keys_event*> keys_pending;
    std::list<tick_event*> tick_pending;

public:
    coro_host() {}
    ~coro_host() {
        for (auto&& evt : keys_pending) { evt->caller.destroy(); }
        for (auto&& evt : tick_pending) { evt->caller.destroy(); }
    }


    void on_key_event(vkey::code vkc, int rkc, int action, int mods) {
        decltype(keys_pending) pending;
        pending.swap(keys_pending);
        for (auto&& evt : pending) {
            evt->set_return(vkc, rkc, action, mods);
            evt->caller.resume();
        }
    }
    void tick(double this_time, double last_time) {
        decltype(tick_pending) pending;
        pending.swap(tick_pending);
        for (auto&& evt : pending) {
            evt->set_return(this_time, last_time);
            evt->caller.resume();
        }
    }

    auto co_keys() { return keys_event(this); }
    auto co_tick() { return tick_event(this); }

    template <typename FuncT>
    coutils::async_fn<void> co_filter_key(FuncT filter) {
        while (true) {
            auto&& [vkc, rkc, action, mods] = co_await co_keys();
            if (filter(vkc, rkc, action, mods)) { break; }
        }
    }
};

} // namespace silly_framework

#endif // __ACTIVITY_COROUTINE_HOST__
