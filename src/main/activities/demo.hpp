#pragma once
#ifndef __DEMO__
#define __DEMO__

#include "framework.hpp"
#include "main/engine/simulator.hpp"

class demo : public base_activity {
    std::mutex lock;
    naive_engine::simulator simu;
    naive_engine::simulator::handle_type person;
    naive_engine::simulator::handle_type obstacle;

public:
    demo(game_window& window);
    ~demo();
    
    void render() override;
    void on_key_event(vkey::code vkc, int rkc, int action, int mods) override;
    void tick(double this_time, double last_time) override;
};

#endif // __DEMO__
