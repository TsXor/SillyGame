#pragma once
#ifndef __SFLIB_NAIVE_TIMER__
#define __SFLIB_NAIVE_TIMER__

#include <chrono>
#include <thread>

namespace silly_framework::rewheel {

struct naive_timer {
    using base_duration = std::chrono::microseconds;
    using clock = std::chrono::steady_clock;

    clock::time_point start;
    base_duration interval;

    template <typename DurRep, typename DurPer>
    naive_timer(std::chrono::duration<DurRep, DurPer> interval_): start(clock::now()),
        interval(std::chrono::duration_cast<base_duration>(interval_)) {}

    clock::time_point limit_from_now() {
        auto exec_begin = clock::now();
        auto dt = std::chrono::duration_cast<base_duration>(exec_begin - start);
        auto floor_dt = dt / interval * interval;
        auto exec_limit = start + floor_dt + interval;
        return exec_limit;
    };
};

struct time_guard {
    naive_timer::clock::time_point limit;
    time_guard(naive_timer& timer) : limit(timer.limit_from_now()) {}
    ~time_guard() { std::this_thread::sleep_until(limit); }
    // 将时间限制设为0，使guard失效
    void cancel() { limit = naive_timer::clock::time_point(); }
};

} // namespace silly_framework::rewheel

#endif // __SFLIB_NAIVE_TIMER__
