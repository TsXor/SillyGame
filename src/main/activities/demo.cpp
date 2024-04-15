#include "demo.hpp"
#include "static_sprites.hpp"
#include "static_maps.hpp"


using namespace naive_engine;
using namespace silly_framework;

static simulator::entity::static_data person_info
    {sprites::container_small(), 64, 96, {{16, 48, 72, 96}}, {{}}};

demo::demo(game_window& window) : base_activity(window), simu(512, 1024) {
    parent.renman.vs_size(1024, 768);
    auto&& [vs_w, vs_h] = parent.renman.vs_size();
    auto&& [sc_w, sc_h] = simu.scene_size();
    person = simu.add_entity(person_info, {sc_w / 2, sc_h / 2});
    obstacle = simu.add_entity(person_info, {sc_w / 2, sc_h / 4});
    simu.current_map = {maps::dorm(), 4.0};
    simu.render_sorter = [](const simulator::entity& a, const simulator::entity& b) {
        return a.center().y < b.center().y;
    };
}

demo::~demo() = default;

void demo::tick(double this_time, double last_time) {
    const std::lock_guard guard(lock);
    double dt = this_time - last_time;
    simu.tick(dt);
    for (auto&& [other, mtv] : simu.colldet(person)) {
        simu.teleport_entity_offset(person, mtv);
    }
}

static inline basics::vec2 get_velocity_direction(game_window& wnd) {
    basics::vec2 vel{0, 0};
    wnd.inpman.with_key_states([&]() {
        if (wnd.inpman.pressed(vkey::code::LEFT))  { vel.x -= 1.0; }
        if (wnd.inpman.pressed(vkey::code::RIGHT)) { vel.x += 1.0; }
        if (wnd.inpman.pressed(vkey::code::UP))    { vel.y -= 1.0; }
        if (wnd.inpman.pressed(vkey::code::DOWN))  { vel.y += 1.0; }
    });
    return vel;
} 

void demo::on_key_event(vkey::code vkc, int rkc, int action, int mods) {
    const std::lock_guard guard(lock);
    constexpr double velocity = 256;
    person->velocity = get_velocity_direction(parent) * velocity;
}

void demo::render() {
    const std::lock_guard guard(lock);
    gl::ClearColor(0, 0, 0, 0);
    simu.render(parent, person->center());
}
