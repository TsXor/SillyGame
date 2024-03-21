#include <thread>
#include <atomic>
#include <uvpp.hpp>
#include "image_loader.hpp"

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
};

uvco::coro_fn<void> load_image(uv_loop_t* loop, const char* filename, stb_decoded_image& out_image);
