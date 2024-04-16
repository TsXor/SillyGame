#pragma once
#ifndef __DEMO__
#define __DEMO__

#include "silly_framework.hpp"
#include "engine/simulator.hpp"

class demo : public silly_framework::base_activity {
    std::mutex lock;
    silly_framework::coro_host cohost;
    naive_engine::simulator simu;
    naive_engine::simulator::handle_type person;
    naive_engine::simulator::handle_type obstacle;

public:
    demo(silly_framework::game_window& window);
    ~demo();
    
    void render() override;
    void on_key_event(silly_framework::vkey::code vkc, int rkc, int action, int mods) override;
    void tick(double this_time, double last_time) override;
};

#endif // __DEMO__
