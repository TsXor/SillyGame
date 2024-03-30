#pragma once
#ifndef __NAIVE_TIMER__
#define __NAIVE_TIMER__

#include <vector>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>

struct naive_timer {
    const size_t interval;
    std::thread thr;
    std::atomic<bool> running;
    std::binary_semaphore event;
    
    naive_timer(size_t interval_millis);
    ~naive_timer();
    void wait() { event.acquire(); }
};

#endif // __NAIVE_TIMER__
