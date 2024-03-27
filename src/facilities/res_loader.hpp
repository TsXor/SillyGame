#pragma once
#ifndef __IO_RESOURCE_LOADER__
#define __IO_RESOURCE_LOADER__

#include <thread>
#include <atomic>
#include <uvpp.hpp>
#include "utilities/stb_image_wrap.hpp"

/**
 * 资源加载器。使用libuv做IO，在单独的线程上运行。
 */
struct res_loader {
    uvpp::loop uvloop;
    std::thread runner;
    std::atomic<bool> running;

    res_loader() = default;
    ~res_loader() = default;

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

/**
 * 构造时自动启动，析构时自动停止的资源加载器。
 * 类似std::jthread。
 */
struct res_jloader : res_loader {
    res_jloader() : res_loader() { start(); }
    ~res_jloader() { stop(); join(); }
};

#endif // __IO_RESOURCE_LOADER__
