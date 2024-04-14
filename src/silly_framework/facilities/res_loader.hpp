#pragma once
#ifndef __IO_RESOURCE_LOADER__
#define __IO_RESOURCE_LOADER__

#include <thread>
#include <atomic>
#include <mutex>
#include <coutils.hpp>
#include <uvpp.hpp>
#include "silly_framework/rewheel/stb_image.hpp"

namespace silly_framework {

/**
 * 资源加载器。使用libuv做IO，在单独的线程上运行。
 */
struct res_loader {
    uvpp::loop uvloop;
    std::thread runner;
    std::atomic<bool> running;
    std::binary_semaphore needed;

    res_loader() : needed(0) {};
    ~res_loader() = default;

    void work();
    // 协程开头需要调用这个函数来激活工作循环
    void activate() { needed.release(); } 
    void start() { running = true; runner = std::thread([&](){work();}); }
    void join() { runner.join(); }
    void stop() { running = false; activate(); }
    void kill() { uvloop.stop(); stop(); }

    auto load_image(const char* filename) -> coutils::async_fn<std::optional<rewheel::stb_decoded_image>>;
};

/**
 * 构造时自动启动，析构时自动停止的资源加载器。
 * 类似std::jthread。
 */
struct res_jloader : res_loader {
    res_jloader() : res_loader() { start(); }
    ~res_jloader() { stop(); join(); }
};

} // namespace silly_framework

#endif // __IO_RESOURCE_LOADER__
