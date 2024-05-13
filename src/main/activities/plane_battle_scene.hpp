#pragma once
#ifndef __PLANE_BATTLE_SCENE__
#define __PLANE_BATTLE_SCENE__

#include <silly_framework.hpp>
#include "engine/simulator.hpp"

namespace acts {

namespace eng = naive_engine;
namespace sf = silly_framework;

class plane_battle_scene : public sf::base_activity {
    using simu_t = eng::simulator;
    using entity_t = simu_t::entity_t;
    using entity_node_t = simu_t::entity_node_t;
    using entity_obj = std::unique_ptr<entity_node_t>;
    struct entity_node_with_hp : public entity_node_t {
        ssize_t hp;
        template <typename... ArgTs>
        entity_node_with_hp(ssize_t hp_, ArgTs&&... args):
            hp(hp_), entity_node_t(std::forward<ArgTs>(args)...) {}
    };
public:
    static constexpr double base_velocity = 256;

    eng::simu_coro_host cohost;
    std::optional<simu_t> simu;
    ssize_t score = 0;
    std::optional<entity_node_with_hp> player_plane;
    simu_t::entity_node_ptr_uset<entity_node_with_hp*> enemy_planes;
    simu_t::entity_node_ptr_uset<entity_node_t*> player_bullets, enemy_bullets;
    eng::basics::vec2 velocity_direction = {0, 0};

    plane_battle_scene(sf::game_window& window);
    ~plane_battle_scene();

    void render() override;
    void on_key_change() override;
    void on_key_signal() override;
    void on_tick(double this_time, double last_time) override;
};

} // namespace acts

#endif // __PLANE_BATTLE_SCENE__
