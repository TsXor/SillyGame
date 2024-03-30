#include "naive_timer.hpp"

naive_timer::naive_timer(size_t interval_millis): interval(interval_millis), event(1) {
    running = true;
    thr = std::thread([&](){
        while (running) {
            event.release();
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
        }
    });
}

naive_timer::~naive_timer() {
    running = false;
    thr.join();
}
