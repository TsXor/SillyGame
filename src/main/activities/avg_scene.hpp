#pragma once
#ifndef __AVG_SCENE__
#define __AVG_SCENE__

#include <unordered_map>
#include <unordered_set>
#include <map>
#include <optional>
#include <any>
#include <silly_framework.hpp>
#include "engine/simulator.hpp"

namespace acts {

namespace eng = naive_engine;
namespace sf = silly_framework;

class avg_scene : public sf::base_activity {
public:
    template <typename T>
    using string_map = std::unordered_map<std::string, T>;
    using script_t = void(*)(avg_scene&, const std::optional<eng::basics::vec2>&, const std::string&);
    using simu_t = eng::simulator;
    using entity_t = simu_t::entity_t;
    using entity_comp_t = std::function<bool(const entity_t&, const entity_t&)>;


    struct sprite_render_data {
        const sf::sprite2d* spr;
        double scalev;
        sf::glut::coord center_offset = {0, 0};
        
        void render(sf::game_window& wnd, sf::glut::coord pos) {
            auto center = pos + center_offset;
            spr->render(wnd, center.x, center.y, scalev);
        }
    };
    struct map_render_data {
        const sf::map2d* map;
        double scalev;
        sf::glut::coord coord_offset = {0, 0};
        
        void render(sf::game_window& wnd, sf::glut::coord camera_pos) {
            auto center = camera_pos - coord_offset;
            map->render(wnd, center.x, center.y, scalev);
        }
    };


    static constexpr double base_velocity = 256;
    static string_map<script_t> scene_scripts;
    static entity_comp_t default_sprites_sorter();
    
    eng::simu_coro_host cohost;
    std::optional<simu_t> simu;
    std::pair<entity_t*, double> keyboard_controlled = {nullptr, 0.0};
    sf::glut::coord camera_pos = {0, 0};
    entity_t* camera_attached = nullptr;
    std::unordered_map<entity_t*, sprite_render_data> bound_sprites;
    entity_comp_t sprites_sorter = default_sprites_sorter();
    std::optional<map_render_data> bound_map;
    eng::basics::vec2 velocity_direction = {0, 0};
    std::any script_data;


    avg_scene(
        sf::game_window& window, const std::string& name,
        const std::optional<eng::basics::vec2>& spawn = std::nullopt,
        const std::string& arg = ""
    );
    ~avg_scene();
    
    void render() override;
    void on_key_change() override;
    void on_key_signal() override;
    void on_tick(double this_time, double last_time) override;
};

} // namespace acts

#endif // __AVG_SCENE__
