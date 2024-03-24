#pragma once
#ifndef __IO_RESOURCE_LOADER__
#define __IO_RESOURCE_LOADER__

#include <thread>
#include <atomic>
#include <uvpp.hpp>
#include "utilities/stb_image_wrap.hpp"

struct res_loader_thread {
    uvpp::loop uvloop;
    std::thread runner;
    std::atomic<bool> running;

    void start() {
        running = true;
        runner = std::thread([&](){
            while (running) { uvloop.run(UV_RUN_ONCE);}
        });
    }
    void join() { runner.join(); }
    void stop() { running = false; }
    void kill() { stop(); uvloop.stop(); }

    uvco::coro_fn<std::optional<stb_decoded_image>> load_image(const char* filename);
};

#endif // __IO_RESOURCE_LOADER__
