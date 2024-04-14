#pragma once
#ifndef __SFLIB_NAIVE_TIMER__
#define __SFLIB_NAIVE_TIMER__

#include <chrono>
#include <thread>

namespace silly_framework::rewheel {

namespace naive_timer {

using base_duration = std::chrono::microseconds;

template <typename DurRep, typename DurPer, typename PredFT, typename BodyFT>
    requires (!std::is_same_v<std::chrono::duration<DurRep, DurPer>, base_duration>)
static inline void while_loop(std::chrono::duration<DurRep, DurPer> interval, PredFT&& pred, BodyFT&& body) {
    while_loop(std::chrono::duration_cast<base_duration>(interval), std::forward<PredFT>(pred), std::forward<BodyFT>(body));
}

template <typename PredFT, typename BodyFT>
static inline void while_loop(base_duration interval_base, PredFT&& pred, BodyFT&& body) {
    using clock = std::chrono::steady_clock;
    clock::time_point start = clock::now();
    auto floor_time = [&](clock::time_point tp) {
        auto dt = std::chrono::duration_cast<base_duration>(tp - start);
        auto floor_dt = dt / interval_base * interval_base;
        return start + floor_dt;
    };
    while (pred()) {
        auto exec_begin = clock::now();
        body();
        std::this_thread::sleep_until(floor_time(exec_begin) + interval_base);
    }
}

} // namespace naive_timer

} // namespace silly_framework::rewheel

#endif // __SFLIB_NAIVE_TIMER__
