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
protected:
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
        void set_return(Ts&&... params) {
            data.template emplace<RetT>(std::forward<Ts>(params)...);
        }
    };
    
    using key_change_ret_type = method_traits<decltype(iface_activity::on_key_change)>::param_tuple_type;
    using key_signal_ret_type = method_traits<decltype(iface_activity::on_key_signal)>::param_tuple_type;
    using tick_ret_type = method_traits<decltype(iface_activity::on_tick)>::param_tuple_type;

public:
    struct key_change_event : public base_event<key_change_event, key_change_ret_type> {
        void register_pending(coro_host& host) { host.key_change_pending.emplace_back(this); }
    };
    struct key_signal_event : public base_event<key_signal_event, key_signal_ret_type> {
        void register_pending(coro_host& host) { host.key_signal_pending.emplace_back(this); }
    };
    struct tick_event : public base_event<tick_event, tick_ret_type>  {
        void register_pending(coro_host& host) { host.tick_pending.emplace_back(this); }
    };

protected:
    std::list<key_change_event*> key_change_pending;
    std::list<key_signal_event*> key_signal_pending;
    std::list<tick_event*> tick_pending;

    template <typename EventT, typename... ArgTs>
    void process_all_of(std::list<EventT>& pending, ArgTs&&... args) {
        std::list<EventT> tmp;
        pending.swap(tmp);
        for (auto&& evt : tmp) {
            evt->set_return(std::forward<ArgTs>(args)...);
            evt->caller.resume();
        }
    }

public:
    coro_host() {}
    ~coro_host() {
        for (auto&& evt : key_change_pending) { evt->caller.destroy(); }
        for (auto&& evt : key_signal_pending) { evt->caller.destroy(); }
        for (auto&& evt : tick_pending) { evt->caller.destroy(); }
    }


    void on_key_change() { process_all_of(key_change_pending); }
    void on_key_signal() { process_all_of(key_signal_pending); }
    void on_tick(double this_time, double last_time) {
        process_all_of(tick_pending, this_time, last_time);
    }

    auto key_change() { return key_change_event(this); }
    auto key_signal() { return key_signal_event(this); }
    auto tick() { return tick_event(this); }

    template <typename FuncT>
    coutils::async_fn<void> filter_key(FuncT filter) {
        while (true) {
            co_await key_change();
            if (filter()) { break; }
        }
    }
};

} // namespace silly_framework

#endif // __ACTIVITY_COROUTINE_HOST__
