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

class avg_coro_host : public sf::coro_host {
    using entity_t = eng::simulator::handle_type;
public:
    enum avg_events {
        EVT_COLLISION = EVT_BASIC_MAX
    };
    struct evt_collision {
        static constexpr size_t code = EVT_COLLISION;
        using return_type = std::tuple<entity_t, entity_t, eng::basics::vec2>;
    };
    void on_collision(entity_t ent1, entity_t ent2, eng::basics::vec2 mtv) {
        process_all_of<evt_collision>(ent1, ent2, mtv);
    }
};

class avg_scene : public sf::base_activity {
public:
    template <typename T>
    using string_map = std::unordered_map<std::string, T>;
    using script_t = void(*)(avg_scene&, const std::string&);
    using simu_t = eng::simulator;
    using entity_t = simu_t::handle_type;
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
            auto center = camera_pos + coord_offset;
            map->render(wnd, center.x, center.y, scalev);
        }
    };


    static constexpr double base_velocity = 256;
    static string_map<script_t> scene_scripts;
    static entity_comp_t default_sprites_sorter();
    
    avg_coro_host cohost;
    std::optional<simu_t> simu;
    std::pair<entity_t, double> keyboard_controlled = {simu_t::null_handle(), 0.0};
    sf::glut::coord camera_pos = {0, 0};
    entity_t camera_attached = simu_t::null_handle();
    sf::utils::listit_umap<entity_t, sprite_render_data> bound_sprites;
    entity_comp_t sprites_sorter = default_sprites_sorter();
    std::optional<map_render_data> bound_map;
    eng::basics::vec2 velocity_direction = {0, 0};
    std::any script_data;


    avg_scene(sf::game_window& window, const std::string& name, const std::string& arg);
    ~avg_scene();
    
    void render() override;
    void on_key_change() override;
    void on_key_signal() override;
    void on_tick(double this_time, double last_time) override;
};

} // namespace acts

#endif // __AVG_SCENE__